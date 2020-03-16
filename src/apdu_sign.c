#include "apdu_sign.h"

#include "apdu.h"
#include "globals.h"
#include "key_macros.h"
#include "keys.h"
#include "memory.h"
#include "to_string.h"
#include "protocol.h"
#include "ui.h"

#include "cx.h"
#include "blockchain.h"

#include <string.h>

#define G global.apdu.u.sign

#define PARSE_ERROR() THROW(EXC_PARSE_ERROR)


static const uint8_t blake2b_personalization[]="ckb-default-hash";

static inline void conditional_init_hash_state(blake2b_hash_state_t *const state) {
    check_null(state);
    if (!state->initialized) {
      cx_blake2b_init2(&state->state, SIGN_HASH_SIZE*8, NULL, 0, (uint8_t*) blake2b_personalization, sizeof(blake2b_personalization)-1);
      state->initialized = true;
    }
}

static void blake2b_incremental_hash(
    /*in/out*/ uint8_t *const out, size_t const out_size,
    /*in/out*/ size_t *const out_length,
    /*in/out*/ blake2b_hash_state_t *const state
) {
    check_null(out);
    check_null(out_length);
    check_null(state);

    conditional_init_hash_state(state);
    cx_hash((cx_hash_t *) &state->state, 0, out, out_size, NULL, 0);
}

static void blake2b_finish_hash(
    /*out*/ uint8_t *const out, size_t const out_size,
    /*in/out*/ uint8_t *const buff, size_t const buff_size,
    /*in/out*/ size_t *const buff_length,
    /*in/out*/ blake2b_hash_state_t *const state
) {
    check_null(out);
    check_null(buff);
    check_null(buff_length);
    check_null(state);

    conditional_init_hash_state(state);
    cx_hash((cx_hash_t *) &state->state, CX_LAST, NULL, 0, out, out_size);
}

static int perform_signature(bool const on_hash, bool const send_hash);

static inline void clear_data(void) {
    memset(&G, 0, sizeof(G));
}

static bool sign_without_hash_ok(void) {
    delayed_send(perform_signature(true, false));
    return true;
}

static bool sign_with_hash_ok(void) {
    delayed_send(perform_signature(true, true));
    return true;
}

static bool sign_reject(void) {
    clear_data();
    delay_reject();
    return true; // Return to idle
}

#define MAX_NUMBER_CHARS (MAX_INT_DIGITS + 2) // include decimal point and terminating null

static size_t sign_complete(uint8_t instruction) {
    static size_t const TYPE_INDEX = 0;
    static size_t const HASH_INDEX = 1;

    static const char *const parse_fail_prompts[] = {
        PROMPT("Unrecognized"),
        PROMPT("Sign Hash"),
        NULL,
    };

    REGISTER_STATIC_UI_VALUE(TYPE_INDEX, "Operation");

    ui_callback_t const ok_c = instruction == INS_SIGN_WITH_HASH ? sign_with_hash_ok : sign_without_hash_ok;

    switch (G.maybe_transaction.v.tag) {

	    case OPERATION_TAG_PLAIN_TRANSFER:
		    {
		    static const uint32_t TYPE_INDEX = 0;
		    static const uint32_t AMOUNT_INDEX = 1;
		    static const uint32_t FEE_INDEX = 2;
		    static const uint32_t SOURCE_INDEX = 3;
		    static const uint32_t DESTINATION_INDEX = 4;
		    static const char *const transaction_prompts[] = {
			    PROMPT("Confirm"),
			    PROMPT("Amount"),
			    PROMPT("Fee"),
			    PROMPT("Source"),
			    PROMPT("Destination"),
			    NULL
		    };
		    REGISTER_STATIC_UI_VALUE(TYPE_INDEX, "Transaction");
		    register_ui_callback(SOURCE_INDEX, lock_arg_to_string, &G.maybe_transaction.v.source);
		    register_ui_callback(DESTINATION_INDEX, lock_arg_to_string,
				    &G.maybe_transaction.v.destination);
		    register_ui_callback(FEE_INDEX, frac_ckb_to_string_indirect, &G.maybe_transaction.v.total_fee);
		    register_ui_callback(AMOUNT_INDEX, frac_ckb_to_string_indirect, &G.maybe_transaction.v.amount);

		    ui_prompt(transaction_prompts, ok_c, sign_reject);

		    }
		    break;
	    case OPERATION_TAG_DAO_DEPOSIT:
		    {
		    static const uint32_t TYPE_INDEX = 0;
		    static const uint32_t AMOUNT_INDEX = 1;
		    static const uint32_t FEE_INDEX = 2;
		    static const uint32_t SOURCE_INDEX = 3;
		    static const uint32_t DESTINATION_INDEX = 4;
		    static const char *const transaction_prompts[] = {
			    PROMPT("Confirm DAO"),
			    PROMPT("Amount"),
			    PROMPT("Fee"),
			    PROMPT("Source"),
			    NULL
		    };
		    REGISTER_STATIC_UI_VALUE(TYPE_INDEX, "Deposit");
		    register_ui_callback(SOURCE_INDEX, lock_arg_to_string, &G.maybe_transaction.v.source);
		    register_ui_callback(DESTINATION_INDEX, lock_arg_to_string, &G.maybe_transaction.v.destination);
		    register_ui_callback(FEE_INDEX, frac_ckb_to_string_indirect, &G.maybe_transaction.v.total_fee);
		    register_ui_callback(AMOUNT_INDEX, frac_ckb_to_string_indirect, &G.maybe_transaction.v.dao_amount);

		    ui_prompt(transaction_prompts, ok_c, sign_reject);

		    }
		    break;
	    case OPERATION_TAG_DAO_PREPARE:
		    {
			    static const uint32_t TYPE_INDEX = 0;
			    static const uint32_t AMOUNT_INDEX = 1;
			    static const uint32_t FEE_INDEX = 2;
			    static const uint32_t SOURCE_INDEX = 3;
			    static const uint32_t PAYER_INDEX = 4;
			    static const uint32_t CHANGE_TO_INDEX = 5;
			    static const uint32_t CHANGE_INDEX = 6;
			    static const char *const prepare_prompts_full[] = {
				    PROMPT("Confirm DAO"),
				    PROMPT("Amount"),
				    PROMPT("Fee"),
				    PROMPT("Owner"),
				    PROMPT("Fee payer"),
				    PROMPT("Change to"),
				    PROMPT("Change"),
				    NULL
			    };
			    static const char *const prepare_prompts_no_change[] = {
				    PROMPT("Confirm DAO"),
				    PROMPT("Amount"),
				    PROMPT("Fee"),
				    PROMPT("Owner"),
				    PROMPT("Fee payer"),
				    NULL
			    };
			    REGISTER_STATIC_UI_VALUE(TYPE_INDEX, "Prepare");
			    register_ui_callback(AMOUNT_INDEX, frac_ckb_to_string_indirect, &G.maybe_transaction.v.dao_amount);
			    register_ui_callback(FEE_INDEX, frac_ckb_to_string_indirect, &G.maybe_transaction.v.total_fee);
			    register_ui_callback(SOURCE_INDEX, lock_arg_to_string, &G.maybe_transaction.v.dao_source);
			    register_ui_callback(PAYER_INDEX, lock_arg_to_string, &G.maybe_transaction.v.source);
			    register_ui_callback(CHANGE_TO_INDEX, lock_arg_to_string, &G.maybe_transaction.v.destination);
			    register_ui_callback(CHANGE_INDEX, frac_ckb_to_string_indirect, &G.maybe_transaction.v.amount);

			    ui_prompt(G.maybe_transaction.v.flags&HAS_CHANGE_ADDRESS?prepare_prompts_full:prepare_prompts_no_change,
					    ok_c, sign_reject);
		    break;
		    }
	    case OPERATION_TAG_DAO_WITHDRAW:
		    {
		    static const uint32_t TYPE_INDEX = 0;
		    static const uint32_t AMOUNT_INDEX = 1;
		    static const uint32_t DESTINATION_INDEX = 2;
		    static const uint32_t DEPOSIT_INDEX = 3;
		    static const uint32_t RETURN_INDEX = 4;
		    static const uint32_t SOURCE_INDEX = 5;
		    static const char *const transaction_prompts[] = {
			    PROMPT("Confirm DAO"),
			    PROMPT("Amount"),
			    PROMPT("Destination"),
			    PROMPT("Deposit"),
			    PROMPT("Return"),
			    PROMPT("Source"),
			    NULL
		    };
		    REGISTER_STATIC_UI_VALUE(TYPE_INDEX, "Withdrawal");
		    register_ui_callback(SOURCE_INDEX, lock_arg_to_string, &G.maybe_transaction.v.dao_source);
		    register_ui_callback(DESTINATION_INDEX, lock_arg_to_string, &G.maybe_transaction.v.destination);
		    register_ui_callback(DEPOSIT_INDEX, frac_ckb_to_string_indirect, &G.maybe_transaction.v.dao_amount);
		    register_ui_callback(RETURN_INDEX, frac_ckb_to_string_indirect, &G.maybe_transaction.v.total_fee);
		    register_ui_callback(AMOUNT_INDEX, frac_ckb_to_string_indirect, &G.maybe_transaction.v.amount);

		    ui_prompt(transaction_prompts, ok_c, sign_reject);

		    }
		    break;
	    default:
		    goto unsafe;
    }

  unsafe:
    G.message_data_as_buffer.bytes = (uint8_t *)&G.final_hash;
    G.message_data_as_buffer.size = sizeof(G.final_hash);
    G.message_data_as_buffer.length = sizeof(G.final_hash);
    // Base58 encoding of 32-byte hash is 43 bytes long.
    register_ui_callback(HASH_INDEX, buffer_to_hex, &G.message_data_as_buffer);
    ui_prompt(parse_fail_prompts, ok_c, sign_reject);
}

#define REJECT(msg, ...) { PRINTF("Rejecting: " msg "\n", ##__VA_ARGS__); G.maybe_transaction.is_valid = false; return; }

bool is_standard_lock_script(mol_seg_t *lockScript) {
	static const uint8_t defaultLockScript[] = { 0x9b, 0xd7, 0xe0, 0x6f, 0x3e, 0xcf, 0x4b, 0xe0, 0xf2, 0xfc, 0xd2, 0x18, 0x8b, 0x23, 0xf1,0xb9,0xfc,0xc8,0x8e,0x5d,0x4b,0x65,0xa8,0x63,0x7b,0x17,0x72,0x3b,0xbd,0xa3,0xcc,0xe8 };
	mol_seg_t hash_type = MolReader_Script_get_hash_type(lockScript);
	if(*hash_type.ptr != 1) return false;
	mol_seg_t codeHash = MolReader_Script_get_code_hash(lockScript);
	return memcmp(defaultLockScript, codeHash.ptr, 32) == 0;
}

bool is_dao_type_script(mol_seg_t *typeScript) {
	static const uint8_t defaultTypeScript[] = {
		0x82, 0xd7, 0x6d, 0x1b, 0x75, 0xfe, 0x2f, 0xd9, 0xa2, 0x7d, 0xfb, 0xaa, 0x65, 0xa0, 0x39, 0x22, 0x1a, 0x38, 0x0d, 0x76, 0xc9, 0x26, 0xf3, 0x78, 0xd3, 0xf8, 0x1c, 0xf3, 0xe7, 0xe1, 0x3f, 0x2e };
	mol_seg_t hash_type = MolReader_Script_get_hash_type(typeScript);
	if(*hash_type.ptr != 1) return false;
	mol_seg_t codeHash = MolReader_Script_get_code_hash(typeScript);
	return memcmp(defaultTypeScript, codeHash.ptr, 32) == 0;
}

enum dao_data_type {
	DAO_DATA_NONE=0,
	DAO_DATA_DEPOSIT,
	DAO_DATA_PREPARED
};

enum dao_data_type get_dao_data_type(mol_seg_t* outputs_data, int i) {
	mol_seg_res_t data=MolReader_BytesVec_get(outputs_data, i);
	if(data.errno != MOL_OK) return DAO_DATA_NONE;
	mol_seg_t rawData=MolReader_Bytes_raw_bytes(&data.seg);
	static const uint8_t eightZeros[8]={ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	if(MolReader_Bytes_length(&data.seg)!=8)
		return DAO_DATA_NONE;
	if(memcmp(rawData.ptr, eightZeros, 8)==0)
		return DAO_DATA_DEPOSIT;
	return DAO_DATA_PREPARED;
}

void parse_context_inner(struct maybe_transaction* _U_ dest, bip32_path_t* _U_ key_derivation, uint8_t *const buff, uint16_t const buff_size);

void parse_context(struct maybe_transaction* _U_ dest, bip32_path_t* _U_ key_derivation, uint8_t *const buff, uint16_t const buff_size) {
	mol_seg_t seg;
	seg.ptr=buff;
	seg.size=buff_size;
	DBGOUT();
	uint8_t mol_result=MolReader_RawTransaction_verify(&seg,true);
	DBGOUT();
	if(mol_result != MOL_OK)
	  REJECT("Transaction verification returned %d; parse failed\nbody: %.*h\n", mol_result, buff_size, buff);

	parse_context_inner(dest, key_derivation, buff, buff_size);
	DBGOUT();
}

void parse_context_inner(struct maybe_transaction* _U_ dest, bip32_path_t* _U_ key_derivation, uint8_t *const buff, uint16_t const buff_size) {
	mol_seg_t seg;
	seg.ptr=buff;
	seg.size=buff_size;

	mol_seg_t outputs = MolReader_RawTransaction_get_outputs(&seg);
	unsigned int outputs_len=MolReader_CellOutputVec_length(&outputs);

	if(outputs_len>3) REJECT("Too many output cells");
	G.context_transactions[G.context_transactions_fill_idx].num_outputs=outputs_len;

	mol_seg_t outputs_data = MolReader_RawTransaction_get_outputs_data(&seg);

	for(mol_num_t i=0;i<outputs_len; i++) {
		mol_seg_res_t output=MolReader_CellOutputVec_get(&outputs, i);
		mol_seg_t capacity = MolReader_CellOutput_get_capacity(&output.seg);

	        // Need to do a memcpy because of alignment issues.
	        memcpy(&G.context_transactions[G.context_transactions_fill_idx].outputs[i].amount, capacity.ptr, 8);

		mol_seg_t lockScript = MolReader_CellOutput_get_lock(&output.seg);
		mol_seg_t lockArg = MolReader_Script_get_args(&lockScript);
		mol_seg_t lockArgBytes = MolReader_Bytes_raw_bytes(&lockArg);

		if(!is_standard_lock_script(&lockScript)) {
                  return;
	       	} else {
 		  G.context_transactions[G.context_transactions_fill_idx].outputs[i].flags|=OUTPUT_FLAGS_KNOWN_LOCK;
		}

		memcpy(G.context_transactions[G.context_transactions_fill_idx].outputs[i].lock_arg, lockArgBytes.ptr, 20);

		mol_seg_t type_script=MolReader_CellOutput_get_type_(&output.seg);
                if(!MolReader_ScriptOpt_is_none(&type_script)) {
                    if(is_dao_type_script(&type_script)) {
			G.context_transactions[G.context_transactions_fill_idx].outputs[i].flags|=OUTPUT_FLAGS_IS_DAO;
			switch(get_dao_data_type(&outputs_data, i)) {
				case DAO_DATA_NONE:
				  REJECT("DAO cells require 8 bytes of data");
				  break;
				case DAO_DATA_DEPOSIT:
				  G.context_transactions[G.context_transactions_fill_idx].outputs[i].flags|=OUTPUT_FLAGS_IS_DAO_DEPOSIT;
				  break;
				case DAO_DATA_PREPARED:
				  break;
			}
                    } else {
                        REJECT("Cannot parse transactions with non-DAO type scripts");
                    }
                }
	}
}

bool is_change(mol_num_t num_inputs, mol_seg_t* inputs, mol_seg_t* lockScript) {
	if(!is_standard_lock_script(lockScript))
		return false;
	mol_seg_t lockArg = MolReader_Script_get_args(lockScript);
	mol_seg_t lockArgBytes = MolReader_Bytes_raw_bytes(&lockArg);

	return memcmp(G.change_lock_arg, lockArgBytes.ptr, 20)==0;
}

void parse_operation_inner(struct maybe_transaction* _U_ dest, bip32_path_t* _U_ key_derivation, uint8_t *const buff, uint16_t const buff_size);

void parse_operation(struct maybe_transaction* _U_ dest, bip32_path_t* _U_ key_derivation, uint8_t *const buff, uint16_t const buff_size) {
	mol_seg_t seg;
	seg.ptr=buff;
	seg.size=buff_size;
	uint8_t mol_result=MolReader_RawTransaction_verify(&seg,true);
	if(mol_result != MOL_OK)
	  REJECT("Transaction verification returned %d; parse failed\nbody: %.*h\n", mol_result, buff_size, buff);

	parse_operation_inner(dest, key_derivation, buff, buff_size);
}

void parse_operation_inner(struct maybe_transaction* _U_ dest, bip32_path_t* _U_ key_derivation, uint8_t *const buff, uint16_t const buff_size) {
	mol_seg_t seg;
	seg.ptr=buff;
	seg.size=buff_size;

	{ // New context for stack.

	mol_seg_t inputs = MolReader_RawTransaction_get_inputs(&seg);
	unsigned int inputs_len=MolReader_CellInputVec_length(&inputs);

	G.maybe_transaction.v.group_input_count = 0;

	if(inputs_len>5) REJECT("Too many input cells");
	if(inputs_len>G.context_transactions_fill_idx) REJECT("Not enough context transactions for inputs");

	uint64_t amount=0;
	uint8_t input_idxes[5];

	uint64_t withdraw_amount=0;

        for(mol_num_t i=0;i<inputs_len; i++) {
		mol_seg_res_t input=MolReader_CellInputVec_get(&inputs, i);
		mol_seg_t outpoint=MolReader_CellInput_get_previous_output(&input.seg);
		mol_seg_t txhash=MolReader_OutPoint_get_tx_hash(&outpoint);
		mol_seg_t out_idx_seg=MolReader_OutPoint_get_index(&outpoint);
		uint32_t out_idx;
		memcpy(&out_idx, out_idx_seg.ptr, 4);
		if(memcmp(G.context_transactions[i].outputs[out_idx].lock_arg,G.current_lock_arg,20) == 0) { // Is the input in the group we are signing for right now
			G.maybe_transaction.v.group_input_count++;
		}
		if(out_idx>3) REJECT("Can't access outputs higher than 3");
		input_idxes[i]=out_idx;
		if(out_idx>G.context_transactions[i].num_outputs) REJECT("Context transaction doesn't have that output");
		if(memcmp(txhash.ptr, G.context_transactions[i].hash, SIGN_HASH_SIZE) != 0) REJECT("Hash of context %d does not match input transaction hash", i);
		amount+=G.context_transactions[i].outputs[out_idx].amount;
		if(G.context_transactions[i].outputs[out_idx].flags&OUTPUT_FLAGS_IS_DAO) {
			memcpy(G.maybe_transaction.v.dao_source, G.context_transactions[i].outputs[i].lock_arg, 20);
			if(G.context_transactions[i].outputs[out_idx].flags&OUTPUT_FLAGS_IS_DAO_DEPOSIT) {
			 // prepare_amounts[i]=amount;
			} else {
			  withdraw_amount+=G.context_transactions[i].outputs[out_idx].amount;
			}
		} else {
			memcpy(G.maybe_transaction.v.source, G.context_transactions[i].outputs[i].lock_arg, 20);
		}
	}

	mol_seg_t outputs = MolReader_RawTransaction_get_outputs(&seg);
	mol_seg_t outputs_data = MolReader_RawTransaction_get_outputs_data(&seg);

	uint64_t output_amounts=0;
	uint64_t sent_amounts=0;
	uint64_t dao_deposit_amount=0;
	uint64_t total_prepare_amount=0;


	for(unsigned int i=0;i<MolReader_CellOutputVec_length(&outputs); i++) {
          mol_seg_res_t output = MolReader_CellOutputVec_get(&outputs, i);
	  mol_seg_t capacity = MolReader_CellOutput_get_capacity(&output.seg);
	  uint64_t capacity_val;
	  memcpy(&capacity_val, capacity.ptr, 8);

	  output_amounts+=capacity_val;
	  mol_seg_t lockScript = MolReader_CellOutput_get_lock(&output.seg);
	  mol_seg_t type_script=MolReader_CellOutput_get_type_(&output.seg);

	  bool isChange=is_change(inputs_len, &inputs, &lockScript);

	  bool isDao = false;
          if (!MolReader_ScriptOpt_is_none(&type_script)) {
              if (is_dao_type_script(&type_script)) {
                  isDao = true;
              } else {
                  REJECT("Cannot parse transactions with non-DAO type scripts");
              }
          }

	  mol_seg_t lockArg = MolReader_Script_get_args(&lockScript);
	  mol_seg_t lockArgBytes = MolReader_Bytes_raw_bytes(&lockArg);

	  if(!isChange && !isDao) {
            sent_amounts+=capacity_val;
	    memcpy(G.maybe_transaction.v.destination, lockArgBytes.ptr, 20);
	    G.maybe_transaction.v.flags|=HAS_DESTINATION_ADDRESS;
	  }
	  if(isDao) {
            switch(get_dao_data_type(&outputs_data, i)) {
		    case DAO_DATA_NONE:
			    REJECT("DAO cells require eight bytes of data");
			    break;
		    case DAO_DATA_DEPOSIT:
			    dao_deposit_amount+=capacity_val;
	                    memcpy(G.maybe_transaction.v.dao_destination, lockArgBytes.ptr, 20);
			    break;
		    case DAO_DATA_PREPARED:
			    if (capacity_val!=G.context_transactions[i].outputs[input_idxes[i]].amount)
				    REJECT("Input and output capacity has to match for a prepare: %d vs %d", capacity_val, G.context_transactions[i].outputs[input_idxes[i]].amount);
			    total_prepare_amount+=capacity_val;
	                    memcpy(G.maybe_transaction.v.dao_destination, lockArgBytes.ptr, 20);

	    }
	  }
	}

	if(((sent_amounts!=0) + (dao_deposit_amount!=0) + (total_prepare_amount!=0) + (withdraw_amount!=0)) > 1) {
            //REJECT("Only handle one of dao deposit, prepare, withdraw, and normal transfer in the same transaction");
	}
	
	G.maybe_transaction.v.total_fee=amount-output_amounts;
	
	if(dao_deposit_amount!=0) {
	    G.maybe_transaction.v.tag=OPERATION_TAG_DAO_DEPOSIT;
	    G.maybe_transaction.v.dao_amount=dao_deposit_amount;
	    if(sent_amounts!=0) REJECT("Deposit with change to a different account is not supported");
	} else if(total_prepare_amount!=0) {
            G.maybe_transaction.v.tag=OPERATION_TAG_DAO_PREPARE;
	    G.maybe_transaction.v.dao_amount=total_prepare_amount;
	    G.maybe_transaction.v.amount=sent_amounts;
	    if(sent_amounts!=0) G.maybe_transaction.v.flags|=HAS_CHANGE_ADDRESS;
	} else if(withdraw_amount!=0) {
            G.maybe_transaction.v.tag=OPERATION_TAG_DAO_WITHDRAW;
	    G.maybe_transaction.v.amount=sent_amounts;
	    G.maybe_transaction.v.dao_amount=withdraw_amount;
	    // Can't currently calculate fee because that requires knowing total return; this is return claimed by the transaction instead.
	    G.maybe_transaction.v.total_fee= -G.maybe_transaction.v.total_fee; 
	} else {
	    G.maybe_transaction.v.tag=OPERATION_TAG_PLAIN_TRANSFER;
	    G.maybe_transaction.v.amount=sent_amounts;
	}

	G.maybe_transaction.is_valid = true;
	}
}

int set_lock_arg(key_pair_t *key_pair, uint8_t *destination) {
  check_null(key_pair);
  check_null(destination);
  cx_blake2b_t lock_arg_state;
  cx_blake2b_init2(&lock_arg_state, 20*8, NULL, 0, (uint8_t*) blake2b_personalization, sizeof(blake2b_personalization)-1);
  cx_hash((cx_hash_t *) &lock_arg_state, 0, key_pair->public_key.W, key_pair->public_key.W_len, NULL, 0);
  cx_hash((cx_hash_t *) &lock_arg_state, CX_LAST, NULL, 0, destination, sizeof(G.current_lock_arg));
  return 0;
}

#define P1_FIRST 0x00
#define P1_NEXT 0x01
#define P1_HASH_ONLY_NEXT 0x03 // You only need it once
#define P1_CHANGE_PATH 0x10
#define P1_CHANGE_PATH_CASE 0x11
#define P1_IS_CONTEXT 0x20
#define P1_NO_FALLBACK 0x40
#define P1_LAST_MARKER 0x80
#define P1_MASK (~(P1_LAST_MARKER|P1_NO_FALLBACK|P1_IS_CONTEXT))

void prep_lock_arg(bip32_path_t *key, uint8_t *destination) {
	int fail=0;
	fail=WITH_KEY_PAIR(*key, key_pair, int, ({
				set_lock_arg(key_pair, destination);
				}));
}

static size_t handle_apdu(bool const enable_hashing, bool const enable_parsing, uint8_t const instruction) {

    uint8_t *const buff = &G_io_apdu_buffer[OFFSET_CDATA];
    uint8_t const p1 = READ_UNALIGNED_BIG_ENDIAN(uint8_t, &G_io_apdu_buffer[OFFSET_P1]);
    uint8_t const buff_size = READ_UNALIGNED_BIG_ENDIAN(uint8_t, &G_io_apdu_buffer[OFFSET_LC]);
    if (buff_size > MAX_APDU_SIZE) THROW(EXC_WRONG_LENGTH_FOR_INS);

    bool last = (p1 & P1_LAST_MARKER) != 0;
    bool is_ctxd = (p1 & P1_IS_CONTEXT) != 0;
    switch (p1 & P1_MASK) {
    case P1_FIRST:
	    {
        clear_data();
        read_bip32_path(&G.key, buff, buff_size);

	prep_lock_arg(&G.key, &G.current_lock_arg);
       	
	// Default the change lock arg to the one we're currently going to sign for
	memcpy(&G.change_lock_arg, &G.current_lock_arg, 20);

        return finalize_successful_send(0);
	    }
    
    case P1_CHANGE_PATH_CASE:
	    {
        bip32_path_t change_key;
        read_bip32_path(&change_key, buff, buff_size);

	prep_lock_arg(&change_key, &G.change_lock_arg);

        return finalize_successful_send(0);
	    }

    case P1_NEXT:
        if (G.key.length == 0) THROW(EXC_WRONG_LENGTH_FOR_INS);

        // Guard against overflow
        if (G.packet_index >= 0xFF) PARSE_ERROR();
        G.packet_index++;

        break;
    default:
        THROW(EXC_WRONG_PARAM);
    }

    if (enable_parsing) {
	    if (!G.maybe_transaction.parse_failed && G.to_parse_fill_idx+buff_size > MAX_TOSIGN_PARSED) {
		    PRINTF("Transaction body too big; can't parse.");
		    G.maybe_transaction.parse_failed = true;
	    } else {
		    memcpy(G.to_parse+G.to_parse_fill_idx, buff, buff_size);
		    G.to_parse_fill_idx+=buff_size;
	    }

	    if (last && !G.maybe_transaction.parse_failed) {
		    G.maybe_transaction.is_valid = false;

		    if(is_ctxd) {
			    parse_context(&G.maybe_transaction, &G.key, G.to_parse, G.to_parse_fill_idx);
		    } else {
			    parse_operation(&G.maybe_transaction, &G.key, G.to_parse, G.to_parse_fill_idx);
			    if (G.maybe_transaction.is_valid==false && (p1 & P1_NO_FALLBACK)) {
				    PRINTF("Strict checking requested and parse failed; bailing.\n");
				    THROW(EXC_PARSE_ERROR);
			    }
		    }
	    }
    }

    if (enable_hashing)
      blake2b_incremental_hash(buff, buff_size, &buff_size, &G.hash_state);

    if (last) {
        if (enable_hashing) {
	    blake2b_finish_hash(G.final_hash, sizeof(G.final_hash), buff, buff_size, &buff_size, &G.hash_state);
        }

	if(is_ctxd) {
          memcpy(G.context_transactions[G.context_transactions_fill_idx].hash, G.final_hash, SIGN_HASH_SIZE);
	  G.context_transactions_fill_idx++;
	  G.to_parse_fill_idx=0;
	  G.hash_state.initialized=false;
          return finalize_successful_send(0);
	} else {
          // Double-hash for the lock script.
      
          cx_blake2b_t double_state;
          cx_blake2b_init2(&double_state, SIGN_HASH_SIZE*8, NULL, 0, (uint8_t*) blake2b_personalization, sizeof(blake2b_personalization)-1);
          cx_hash((cx_hash_t *) &double_state, 0, G.final_hash, sizeof(G.final_hash), NULL, 0);

	  static const uint8_t self_witness[] = {
		  0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Length of WitnessArg, 
		  0x55, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x55, 0x00, // WitnessArg
		  0x00, 0x00, 0x55, 0x00, 0x00, 0x00, 0x41, 0x00, 0x00, 0x00,
		  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		  0x00, 0x00, 0x00, 0x00, 0x00
	  };

          cx_hash((cx_hash_t *) &double_state, 0, self_witness, sizeof(self_witness), NULL, 0);

	  static const uint8_t empty_witness_len[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	  for(uint32_t i=1;i<G.maybe_transaction.v.group_input_count;i++)
            cx_hash((cx_hash_t *) &double_state, 0, empty_witness_len, sizeof(empty_witness_len), NULL, 0);

          cx_hash((cx_hash_t *) &double_state, CX_LAST, NULL, 0, G.final_hash, sizeof(G.final_hash));

          return sign_complete(instruction);
	}
    } else {
        return finalize_successful_send(0);
    }
}

size_t handle_apdu_sign(uint8_t instruction) {
    bool const enable_hashing = instruction != INS_SIGN_UNSAFE;
    bool const enable_parsing = enable_hashing;
    return handle_apdu(enable_hashing, enable_parsing, instruction);
}

size_t handle_apdu_sign_with_hash(uint8_t instruction) {
    bool const enable_hashing = true;
    bool const enable_parsing = true;
    return handle_apdu(enable_hashing, enable_parsing, instruction);
}

static int perform_signature(bool const on_hash, bool const send_hash) {
    if (on_hash && G.hash_only) {
        memcpy(G_io_apdu_buffer, G.final_hash, sizeof(G.final_hash));
        clear_data();
        return finalize_successful_send(sizeof(G.final_hash));
    }


    size_t tx = 0;
    if (send_hash && on_hash) {
        memcpy(&G_io_apdu_buffer[tx], G.final_hash, sizeof(G.final_hash));
        tx += sizeof(G.final_hash);
    }

    uint8_t const *const data = G.final_hash; // on_hash ? G.final_hash : G.message_data;
    size_t const data_length = sizeof(G.final_hash); // on_hash ? sizeof(G.final_hash) : G.message_data_length;

    tx += WITH_KEY_PAIR(G.key, key_pair, size_t, ({
        sign(&G_io_apdu_buffer[tx], MAX_SIGNATURE_SIZE, key_pair, data, data_length);
    }));


    clear_data();
    return finalize_successful_send(tx);
}
