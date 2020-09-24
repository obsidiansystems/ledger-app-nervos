#include "apdu_sign.h"

#include "apdu.h"
#include "globals.h"
#include "key_macros.h"
#include "keys.h"
#include "memory.h"
#include "to_string.h"
#include "protocol.h"
#include "ui.h"

#define MOL_PIC(x) ((void (*)()) PIC(x))
#define MOL_PIC_STRUCT(t,x) (x?((t*) PIC(x)):NULL)
#define mol_printf(...) PRINTF(__VA_ARGS__)
#define mol_emerg_reject THROW(EXC_MEMORY_ERROR)

#include "cx.h"
#include "annotated.h"

#include <string.h>

#define G global.apdu.u.sign

#define PARSE_ERROR() THROW(EXC_PARSE_ERROR)

static inline void conditional_init_hash_state(blake2b_hash_state_t *const state) {
    check_null(state);
    if (!state->initialized) {
        cx_blake2b_init2(&state->state, SIGN_HASH_SIZE * 8, NULL, 0, (uint8_t *)blake2b_personalization,
                         sizeof(blake2b_personalization) - 1);
        state->initialized = true;
    }
}

static void blake2b_incremental_hash(
    /*in*/ const uint8_t *const out, size_t const out_size,
    /*in/out*/ blake2b_hash_state_t *const state) {
    check_null(out);
    check_null(state);

    conditional_init_hash_state(state);
    cx_hash((cx_hash_t *)&state->state, 0, out, out_size, NULL, 0);
}

static void blake2b_finish_hash(
    /*out*/ uint8_t *const out, size_t const out_size,
    /*in/out*/ blake2b_hash_state_t *const state) {
    check_null(out);
    check_null(state);

    conditional_init_hash_state(state);
    cx_hash((cx_hash_t *)&state->state, CX_LAST, NULL, 0, out, out_size);
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

static void multi_output_prompts_cb(size_t which) {
    switch(which) {
        case 0:
            {
                static const char prompt[]="Confirm";
                static const char value[]="Transaction";
                memcpy(global.ui.prompt.active_prompt, (const void*)PIC(prompt), sizeof(prompt));
                memcpy(global.ui.prompt.active_value, (const void*)PIC(value), sizeof(value));
            }
        break;
        case 1:
            {
                static const char prompt[]="Amount";
                memcpy(global.ui.prompt.active_prompt, (const void*)PIC(prompt), sizeof(prompt));
                frac_ckb_to_string_indirect(global.ui.prompt.active_value, sizeof(global.ui.prompt.active_value), &G.maybe_transaction.v.amount.snd);
            }
        break;
        case 2:
            {
                static const char prompt[]="Fee";
                memcpy(global.ui.prompt.active_prompt, (const void*)PIC(prompt), sizeof(prompt));
                frac_ckb_to_string_indirect(global.ui.prompt.active_value, sizeof(global.ui.prompt.active_value), &G.maybe_transaction.v.total_fee);
            }
        break;
        default:
            {
                if(sizeof(global.ui.prompt.active_prompt)<13) THROW(EXC_WRONG_LENGTH);
                if(sizeof(global.ui.prompt.active_value)<29) THROW(EXC_WRONG_LENGTH);
                static const char prompt[]="Output ";
                memcpy(global.ui.prompt.active_prompt, (const void*)PIC(prompt), sizeof(prompt));
                size_t prompt_fill=sizeof(prompt)-1;
                if(which<100) prompt_fill+=number_to_string(global.ui.prompt.active_prompt+prompt_fill, which-2);
                if(G.u.tx.output_count<100) {
                    global.ui.prompt.active_prompt[prompt_fill++]='/';
                    number_to_string(global.ui.prompt.active_prompt+prompt_fill, G.u.tx.output_count);
                }

                frac_ckb_to_string_indirect(global.ui.prompt.active_value, sizeof(global.ui.prompt.active_value), &G.u.tx.outputs[which-3].capacity);
                // Maximum of 20
                size_t value_fill=strnlen(global.ui.prompt.active_value, sizeof(global.ui.prompt.active_value));
                static const char separator[]=" CKB -> ";
                memcpy(global.ui.prompt.active_value+value_fill, separator, sizeof(separator));
                // Maximum of 28
                value_fill+=sizeof(separator)-1;

                void (*lock_arg_to_destination_address)(char *const, size_t const, lock_arg_t const *const) = G.u.tx.sending_to_multisig_output ? lock_arg_to_multisig_address : lock_arg_to_sighash_address;
                lock_arg_to_destination_address(global.ui.prompt.active_value+value_fill, sizeof(global.ui.prompt.active_value), &G.u.tx.outputs[which-3].destination);
            }
    }
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
    void *lock_arg_to_destination_address_cb = G.u.tx.sending_to_multisig_output ? lock_arg_to_multisig_address : lock_arg_to_sighash_address;

    switch (G.maybe_transaction.v.tag) {

    case OPERATION_TAG_PLAIN_TRANSFER: {
        static const uint32_t TYPE_INDEX = 0;
        static const uint32_t AMOUNT_INDEX = 1;
        static const uint32_t FEE_INDEX = 2;
        static const uint32_t DESTINATION_INDEX = 3;
        static const uint32_t CONTRACT_INDEX = 4;

        if (N_data.contract_data_type == ALLOW_CONTRACT_DATA) {
            static const char *const transaction_prompts[] = {PROMPT("Confirm"), PROMPT("Amount"), PROMPT("Fee"),
                                                              PROMPT("Destination"), PROMPT("Contract"), NULL};
            REGISTER_STATIC_UI_VALUE(TYPE_INDEX, "Transaction");
            //register_ui_callback(SOURCE_INDEX, lock_arg_to_address, &G.maybe_transaction.v.source);
            register_ui_callback(DESTINATION_INDEX, lock_arg_to_destination_address_cb, &G.u.tx.outputs[0].destination);
            register_ui_callback(FEE_INDEX, frac_ckb_to_string_indirect, &G.maybe_transaction.v.total_fee);
            register_ui_callback(AMOUNT_INDEX, frac_ckb_to_string_indirect, &G.maybe_transaction.v.amount.snd);
            register_ui_callback(CONTRACT_INDEX, contract_type_to_string_indirect, &G.maybe_transaction.v.contract_type);

            ui_prompt(transaction_prompts, ok_c, sign_reject);
        } else {
            static const char *const transaction_prompts[] = {PROMPT("Confirm"), PROMPT("Amount"), PROMPT("Fee"),
                                                              PROMPT("Destination"), NULL};
            REGISTER_STATIC_UI_VALUE(TYPE_INDEX, "Transaction");
            //register_ui_callback(SOURCE_INDEX, lock_arg_to_address, &G.maybe_transaction.v.source);
            register_ui_callback(DESTINATION_INDEX, lock_arg_to_destination_address_cb, &G.u.tx.outputs[0].destination);
            register_ui_callback(FEE_INDEX, frac_ckb_to_string_indirect, &G.maybe_transaction.v.total_fee);
            register_ui_callback(AMOUNT_INDEX, frac_ckb_to_string_indirect, &G.maybe_transaction.v.amount.snd);

            ui_prompt(transaction_prompts, ok_c, sign_reject);
        }
    } break;
    case OPERATION_TAG_MULTI_OUTPUT_TRANSFER: {
        ui_prompt_with_cb(&multi_output_prompts_cb, 3 + G.u.tx.output_count, ok_c, sign_reject);
    } break;
    case OPERATION_TAG_SELF_TRANSFER: {
        static const uint32_t TYPE_INDEX = 0;
        static const uint32_t AMOUNT_INDEX = 1;
        static const uint32_t FEE_INDEX = 2;
        static const uint32_t DESTINATION_INDEX = 3;
        static const char *const transaction_prompts[] = {PROMPT("Confirm"), PROMPT("Amount"),      PROMPT("Fee"),
                                                          PROMPT("Destination"), NULL};
        REGISTER_STATIC_UI_VALUE(TYPE_INDEX, "Self-Transfer");
        register_ui_callback(DESTINATION_INDEX, lock_arg_to_destination_address_cb, &G.u.tx.outputs[0].destination);
        register_ui_callback(FEE_INDEX, frac_ckb_to_string_indirect, &G.maybe_transaction.v.total_fee);
        register_ui_callback(AMOUNT_INDEX, frac_ckb_to_string_indirect, &G.maybe_transaction.v.amount.snd);

        ui_prompt(transaction_prompts, ok_c, sign_reject);

    } break;
    case OPERATION_TAG_MULTI_INPUT_TRANSFER: {
        static const uint32_t TYPE_INDEX = 0;
        static const uint32_t INPUT_COUNT_INDEX = 1;
        static const uint32_t SOURCE_INDEX = 2;
        static const uint32_t AMOUNT_INDEX = 3;
        static const uint32_t FEE_INDEX = 4;
        static const uint32_t DESTINATION_INDEX = 5;
        static const char *const transaction_prompts[] = {PROMPT("Confirm"), PROMPT("Input"), PROMPT("Source"),
                                                          PROMPT("Amount"), PROMPT("Fee"),
                                                          PROMPT("Destination"), NULL};
        REGISTER_STATIC_UI_VALUE(TYPE_INDEX, "Multi-Input Transaction");
        register_ui_callback(INPUT_COUNT_INDEX, uint64_tuple_to_string, &G.maybe_transaction.v.input_count);
        register_ui_callback(SOURCE_INDEX, lock_arg_to_sighash_address, &G.current_lock_arg);
        register_ui_callback(AMOUNT_INDEX, frac_ckb_tuple_to_string_indirect, &G.maybe_transaction.v.amount);
        register_ui_callback(FEE_INDEX, frac_ckb_to_string_indirect, &G.maybe_transaction.v.total_fee);
        register_ui_callback(DESTINATION_INDEX, lock_arg_to_destination_address_cb, &G.u.tx.outputs[0].destination);

        ui_prompt(transaction_prompts, ok_c, sign_reject);

    } break;
    case OPERATION_TAG_DAO_DEPOSIT: {
        static const uint32_t TYPE_INDEX = 0;
        static const uint32_t AMOUNT_INDEX = 1;
        static const uint32_t FEE_INDEX = 2;
        static const uint32_t DESTINATION_INDEX = 3;
        static const uint32_t OWNER_INDEX = 4;
        static const char *const transaction_prompts[] = {PROMPT("Confirm DAO"), PROMPT("Deposit Amount"), PROMPT("Fee"), PROMPT("Cell Owner"),
                                                          NULL};
        REGISTER_STATIC_UI_VALUE(TYPE_INDEX, "Deposit");
        // register_ui_callback(SOURCE_INDEX, lock_arg_to_address, &G.maybe_transaction.v.source);
        register_ui_callback(DESTINATION_INDEX, lock_arg_to_destination_address_cb, &G.u.tx.outputs[0].destination);
        register_ui_callback(FEE_INDEX, frac_ckb_to_string_indirect, &G.maybe_transaction.v.total_fee);
        register_ui_callback(AMOUNT_INDEX, frac_ckb_to_string_indirect, &G.maybe_transaction.v.dao_amount);
        register_ui_callback(OWNER_INDEX, lock_arg_to_sighash_address, &G.dao_cell_owner);

        ui_prompt(transaction_prompts, ok_c, sign_reject);

    } break;
    case OPERATION_TAG_DAO_PREPARE: {
        static const uint32_t TYPE_INDEX = 0;
        static const uint32_t AMOUNT_INDEX = 1;
        static const uint32_t FEE_INDEX = 2;
        static const uint32_t OWNER_INDEX = 3;
        static const char *const prepare_prompts_full[] = {
                                                            PROMPT("Confirm DAO"),
                                                            PROMPT("Deposit Amount"),
                                                            PROMPT("Fee"),
                                                            PROMPT("Cell Owner"),
                                                            NULL };
        REGISTER_STATIC_UI_VALUE(TYPE_INDEX, "Prepare");
        register_ui_callback(AMOUNT_INDEX, frac_ckb_to_string_indirect, &G.maybe_transaction.v.dao_amount);
        register_ui_callback(FEE_INDEX, frac_ckb_to_string_indirect, &G.maybe_transaction.v.total_fee);
        register_ui_callback(OWNER_INDEX, lock_arg_to_sighash_address, &G.dao_cell_owner);
        ui_prompt(prepare_prompts_full,
                  ok_c, sign_reject);
        break;
    }
    case OPERATION_TAG_DAO_WITHDRAW: {
        static const uint32_t TYPE_INDEX = 0;
        static const uint32_t AMOUNT_INDEX = 1;
        static const uint32_t COMPENSATION_INDEX = 2;
        static const uint32_t OWNER_INDEX = 3;
        static const char *const transaction_prompts[] = {PROMPT("Confirm DAO"),
                                                          PROMPT("Deposit Amount"),
                                                          PROMPT("Compensation"),
                                                          PROMPT("Cell Owner"),
                                                          NULL};
        REGISTER_STATIC_UI_VALUE(TYPE_INDEX, "Withdrawal");
        register_ui_callback(AMOUNT_INDEX, frac_ckb_to_string_indirect, &G.maybe_transaction.v.dao_amount);
        register_ui_callback(OWNER_INDEX, lock_arg_to_sighash_address, &G.dao_cell_owner);
        register_ui_callback(COMPENSATION_INDEX, frac_ckb_to_string_indirect, &G.maybe_transaction.v.total_fee);
        ui_prompt(transaction_prompts, ok_c, sign_reject);

    } break;
    default:
        goto unsafe;
    }

unsafe:
  THROW(EXC_REJECT);
}

/***********************************************************/

#define REJECT(msg, ...)                                                                                               \
    {                                                                                                                  \
        PRINTF("Rejecting: " msg "\n", ##__VA_ARGS__);                                                                 \
        G.maybe_transaction.unsafe = true;                                                                             \
        return;                                                                                                        \
    }

#define REJECT_HARD(msg, ...)                                                                                          \
    {                                                                                                                  \
        PRINTF("Can't sign: " msg "\n", ##__VA_ARGS__);                                                                   \
        G.maybe_transaction.hard_reject = true;                                                                        \
        return;                                                                                                        \
    }

void prep_lock_arg(bip32_path_t *key, standard_lock_arg_t *destination) {
    cx_ecfp_public_key_t public_key;
    generate_public_key(&public_key, key);
    generate_lock_arg_for_pubkey(&public_key, destination);
}

/* Start of parser callbacks */

void blake2b_chunk(uint8_t* buf, mol_num_t len) {
    blake2b_incremental_hash(buf, len, &G.hash_state);
}

void inputs_start() {
    explicit_bzero((void*)&G.u.inp.last_input_lock_arg, sizeof(G.u.inp.last_input_lock_arg));
}

void input_start() {
    explicit_bzero(&G.cell_state, sizeof(G.cell_state));
    explicit_bzero((void*) &G.lock_arg_tmp, sizeof(G.lock_arg_tmp));
    explicit_bzero((void*)&G.u.inp.input_state, sizeof(G.u.inp.input_state));
}

void input_save_index(uint8_t *index, mol_num_t index_length) {
    (void) index_length; // guaranteed by parser
    memcpy(&G.u.inp.input_state.index, index, sizeof(G.u.inp.input_state.index));
}

void context_blake2b_chunk(uint8_t *chunk, mol_num_t length) {
    blake2b_incremental_hash(chunk, length, &G.u.inp.input_state.hash_state);
}

void finish_context_txn(void) {
    uint8_t tx_hash[32];
    blake2b_finish_hash(tx_hash, 32, &G.u.inp.input_state.hash_state);
    blake2b_chunk(tx_hash, 32);
    blake2b_chunk(&G.u.inp.input_state.index, sizeof(G.u.inp.input_state.index));
    explicit_bzero(&G.u.inp.input_state, sizeof(G.u.inp.input_state));
}

void finish_inputs(void) {
    explicit_bzero(&G.u, sizeof(G.u));
}

void input_context_start_idx(mol_num_t idx) {
    // Enable/disable the remaining input callbacks based on whether we're on that output.
    G.cell_state.active = idx == G.u.inp.input_state.index;
}

void cell_capacity(uint8_t* capacity, mol_num_t len) {
    (void)len; // constant from the parser
    if(!G.cell_state.active) return;
    G.cell_state.capacity = *(uint64_t*) capacity;
}

const uint8_t defaultLockScript[] = {0x9b, 0xd7, 0xe0, 0x6f, 0x3e, 0xcf, 0x4b, 0xe0, 0xf2, 0xfc, 0xd2,
                                     0x18, 0x8b, 0x23, 0xf1, 0xb9, 0xfc, 0xc8, 0x8e, 0x5d, 0x4b, 0x65,
                                     0xa8, 0x63, 0x7b, 0x17, 0x72, 0x3b, 0xbd, 0xa3, 0xcc, 0xe8};

const uint8_t multisigLockScript[] = { 0x5c, 0x50, 0x69, 0xeb, 0x08, 0x57, 0xef, 0xc6, 0x5e, 0x1b, 0xca,
                                       0x0c, 0x07, 0xdf, 0x34, 0xc3, 0x16, 0x63, 0xb3, 0x62, 0x2f, 0xd3,
                                       0x87, 0x6c, 0x87, 0x63, 0x20, 0xfc, 0x96, 0x34, 0xe2, 0xa8 };

const uint8_t dao_type_script_hash[] = {0x82, 0xd7, 0x6d, 0x1b, 0x75, 0xfe, 0x2f, 0xd9, 0xa2, 0x7d, 0xfb,
                                        0xaa, 0x65, 0xa0, 0x39, 0x22, 0x1a, 0x38, 0x0d, 0x76, 0xc9, 0x26,
                                        0xf3, 0x78, 0xd3, 0xf8, 0x1c, 0xf3, 0xe7, 0xe1, 0x3f, 0x2e};

void cell_lock_code_hash(uint8_t* buf, mol_num_t len) {
    (void)len;
    if(!G.cell_state.active) return;

    if(!memcmp(buf, defaultLockScript, 32)) {
        G.cell_state.is_multisig = false;
    } else if (!memcmp(buf, multisigLockScript, 32)) {
        G.cell_state.is_multisig = true;
    } else if (N_data.contract_data_type == DISALLOW_CONTRACT_DATA) {
        REJECT("The lock script is unsupported");
    }
}

void cell_script_hash_type(uint8_t hash_type) {
    if(!G.cell_state.active) return;
    if (hash_type != 1 && N_data.contract_data_type == DISALLOW_CONTRACT_DATA)
        REJECT("Incorrect hash type for standard lock or dao script");
}

void script_arg_start_input() {
    if(!G.cell_state.active) return;
    G.cell_state.lock_arg_index = 0;
    G.cell_state.lock_arg_nonequal = 0;
    // current_lock_arg is the one we are signing with
    G.lock_arg_cmp = G.current_lock_arg;
}

void script_arg_chunk(uint8_t* buf, mol_num_t buflen) {
    if(!G.cell_state.active) return;
    uint32_t current_offset = G.cell_state.lock_arg_index;
    if(G.cell_state.lock_arg_index+buflen > 28) { // Unknown arg
        G.cell_state.lock_arg_nonequal |= true;
        G.cell_state.is_change = false;
        return;
    }

    memcpy(((uint8_t*) &G.lock_arg_tmp) + current_offset, buf, buflen);
    G.cell_state.lock_arg_index+=buflen;

    for(mol_num_t i=0;i<buflen;i++) {
        // Change address cannot be timelock, ie more than 20 bytes long
        if ((current_offset+i > 20) || (G.change_lock_arg[current_offset+i] != buf[i])) {
            G.cell_state.is_change = false;
            break;
        }
    }

    if(!G.lock_arg_cmp) {
        G.cell_state.lock_arg_nonequal=true;
        return;
    }

    for(mol_num_t i=0;i<buflen;i++) {
        G.cell_state.lock_arg_nonequal |= (G.lock_arg_cmp[current_offset+i] != buf[i]);
        if(G.cell_state.lock_arg_nonequal) return;
    }
}

void input_lock_arg_end() {
    if(!G.cell_state.active) return;

    if (memcmp(&G.u.inp.last_input_lock_arg, &G.lock_arg_tmp.hash, sizeof(G.u.inp.last_input_lock_arg))) {
        G.distinct_input_sources += 1;
        if (G.cell_state.lock_arg_nonequal == 0) {
            // We are signing this input
            // Store the 'index' of this input wrt all other inputs
            // The 'index' starts from 1
            if (G.maybe_transaction.v.input_count.fst != 0)
                REJECT("Input cells in AnnotatedTransaction are not in expected order");
            G.maybe_transaction.v.input_count.fst = G.distinct_input_sources;
        }
    }
    memcpy(&G.u.inp.last_input_lock_arg, &G.lock_arg_tmp.hash, sizeof(G.u.inp.last_input_lock_arg));
}

void cell_type_code_hash(uint8_t* buf, mol_num_t len) {
    if(!G.cell_state.active) return;
    (void) len; // Guaranteed to be 32

    // If this exists, we require it to be the DAO for now. Verify.
    if(!memcmp(buf, dao_type_script_hash, sizeof(dao_type_script_hash)))
        G.cell_state.is_dao = true;
    else if(N_data.contract_data_type == DISALLOW_CONTRACT_DATA)
        REJECT("Only the DAO type script is supported");
}

void cell_type_arg_length(mol_num_t length) {
    if(!G.cell_state.active) return;
    // DAO is empty.
    if(length != 4 && G.cell_state.is_dao)
        REJECT("DAO cell has nonempty args");
    if(length != 4 && N_data.contract_data_type == DISALLOW_CONTRACT_DATA)
        REJECT("Cannot handle cell arguments");
}

void set_cell_data_size(mol_num_t size) {
    if(!G.cell_state.active) return;
    G.cell_state.data_size = MIN(15, size-4); // size includes the 4-byte size header in Bytes
}

void check_cell_data_data_chunk(uint8_t *buf, mol_num_t length) {
    if(!G.cell_state.active) return;
    for(mol_num_t i=0;i<length;i++)
        G.cell_state.dao_data_is_nonzero |= buf[i];
}

void finish_input_cell_data() {
    if(!G.cell_state.active) return;
    if(G.cell_state.is_dao) {
        memcpy(&G.dao_cell_owner, &G.lock_arg_tmp.hash, sizeof(G.lock_arg_tmp.hash));
        if(G.cell_state.data_size != 8) REJECT("DAO data must be 8 bytes");
        G.dao_input_amount += G.cell_state.capacity;
        if(G.cell_state.dao_data_is_nonzero) {
            if(G.maybe_transaction.v.tag != OPERATION_TAG_DAO_WITHDRAW && G.maybe_transaction.v.tag != 0) REJECT("Can't mix deposit, prepare, and withdraw in one transaction");
            G.maybe_transaction.v.tag = OPERATION_TAG_DAO_WITHDRAW;
        } else {
            if(G.maybe_transaction.v.tag != OPERATION_TAG_DAO_PREPARE && G.maybe_transaction.v.tag != 0) REJECT("Can't mix deposit, prepare, and withdraw in one transaction");
            G.maybe_transaction.v.tag = OPERATION_TAG_DAO_PREPARE;
        }
    } else {
        if(G.cell_state.data_size != 0 && N_data.contract_data_type == DISALLOW_CONTRACT_DATA) {
            REJECT("Data found in non-dao cell");
        }
        if (G.cell_state.data_size > 0) // TODO: parse contract data here
            G.maybe_transaction.v.contract_type = CONTRACT_UNKNOWN;
        // total input amount
        G.input_amount.snd += G.cell_state.capacity;
        if(!G.cell_state.lock_arg_nonequal) {
            // amount we are signing
            G.input_amount.fst += G.cell_state.capacity;
        }
        G.signing_multisig_input |= G.cell_state.is_multisig;
    }
}

const struct byte_callbacks hash_type_cb = { cell_script_hash_type };

// Process one input, provided as an AnnotatedCellInput.
const AnnotatedCellInput_cb annotatedCellInput_callbacks = {
    .start = input_start,
    .input = &(CellInput_cb) {
    .since = &(Uint64_cb) { { blake2b_chunk } },
        .previous_output = &(OutPoint_cb) {
            .index = &(Uint32_cb) { { input_save_index } }
        }
    },
    .source = &(RawTransaction_cb) {
        .chunk = context_blake2b_chunk,
        .outputs = &(CellOutputVec_cb) {
            .index = input_context_start_idx,
            .item = &(CellOutput_cb) {
                .capacity = &(Uint64_cb) { { cell_capacity } },
                .lock = &(Script_cb) {
                    .code_hash = &(Byte32_cb) { { cell_lock_code_hash } },
                    .hash_type = &hash_type_cb,
                    .args = &(Bytes_cb) { .start = script_arg_start_input, .body_chunk = script_arg_chunk },
                    .end = input_lock_arg_end
                },
                .type_ = &(ScriptOpt_cb) { .item = &(Script_cb) {
                    .code_hash = &(Byte32_cb) { { cell_type_code_hash } },
                    .hash_type = &hash_type_cb,
                    .args = &(Bytes_cb) { .size = cell_type_arg_length }
                } }
            }
        },
        .outputs_data = &(BytesVec_cb) {
            .index = input_context_start_idx,
            .item = &(Bytes_cb) { .size = set_cell_data_size, .body_chunk = check_cell_data_data_chunk, .end = finish_input_cell_data }
        },
        .end = finish_context_txn,
    }
};

// Add the number of inputs to the raw transaction hash; used at the start of the inputs. This is the header for the implicit FixVec<CellInput> we are writing into the hash.
void blake2b_input_count(mol_num_t length) {
    if(length != G.input_count) REJECT_HARD("Number of input cells reported in AnnotatedTransaction does not match the actual number of input cells");
    blake2b_incremental_hash((uint8_t*) &G.input_count, 4, &G.hash_state);
}

void computeNewOffsetsToHash(struct AnnotatedRawTransaction_state *s) {
    mol_num_t to_subtract = s->outputs_offset - s->inputs_offset - ( 44 * G.input_count + 4);
    mol_num_t scratch;

    scratch=s->total_size - to_subtract;
    blake2b_incremental_hash((uint8_t*) &scratch, 4, &G.hash_state);

    blake2b_incremental_hash((uint8_t*) &s->version_offset, 4, &G.hash_state);
    blake2b_incremental_hash((uint8_t*) &s->cell_deps_offset, 4, &G.hash_state);
    blake2b_incremental_hash((uint8_t*) &s->header_deps_offset, 4, &G.hash_state);
    blake2b_incremental_hash((uint8_t*) &s->inputs_offset, 4, &G.hash_state);

    scratch=s->outputs_offset - to_subtract;
    blake2b_incremental_hash((uint8_t*) &scratch, 4, &G.hash_state);
    scratch=s->outputs_data_offset - to_subtract;
    blake2b_incremental_hash((uint8_t*) &scratch, 4, &G.hash_state);
}

void output_start(mol_num_t index) {
    G.u.tx.current_output_index=index;
    explicit_bzero((void*) &G.cell_state, sizeof(G.cell_state));
    explicit_bzero((void*) &G.lock_arg_tmp, sizeof(G.lock_arg_tmp));
    G.cell_state.active = true;
    G.lock_arg_cmp=G.change_lock_arg;
    G.cell_state.is_change = true;
}

void output_end(void) {
    bool is_second_change = G.u.tx.processed_change_cell && G.cell_state.is_change;
    uint64_t zero_val = 0;
    bool dest_is_src = !G.cell_state.is_change
        && (0 == memcmp(G.current_lock_arg, G.lock_arg_tmp.hash, sizeof(G.lock_arg_tmp.hash)))
        && (0 == memcmp(&zero_val, G.lock_arg_tmp.lock_period, sizeof(G.lock_arg_tmp.lock_period)));

    G.u.tx.is_self_transfer |=  is_second_change || dest_is_src;

    // Have we now processed at least 1 change cell?
    G.u.tx.processed_change_cell |= G.cell_state.is_change;

    if(G.cell_state.is_dao) {
        memcpy(&G.dao_cell_owner, &G.lock_arg_tmp.hash, sizeof(G.lock_arg_tmp.hash));
        G.u.tx.dao_output_amount += G.cell_state.capacity;
        G.u.tx.dao_bitmask |= 1<<G.u.tx.current_output_index;
        if(!G.cell_state.is_change && G.cell_state.lock_arg_nonequal)
            REJECT("Not allowing DAO outputs to be sent to a non-self address");
    } else {
        if(G.cell_state.is_multisig) {
            G.u.tx.sending_to_multisig_output = true;
        }
        if(G.u.tx.is_self_transfer) {
            // The normal rules no longer apply
            if(is_second_change) {
                // It is now a self_txn so no change exists
                G.u.tx.plain_output_amount += G.u.tx.change_amount;
                G.u.tx.change_amount = 0;
            }
            G.u.tx.plain_output_amount += G.cell_state.capacity;
            uint8_t *dest_to_show = dest_is_src ? G.lock_arg_tmp.hash : G.change_lock_arg;

            // Doesn't cover the case where dst is src, but different change address, which sets maybe-txn, even
            if((G.maybe_transaction.v.flags & HAS_DESTINATION_ADDRESS) && memcmp(G.u.tx.outputs[0].destination.hash, dest_to_show, 20)) {
                // If here either, the destination is the signer, but the change address is different, or we need to reject it because of multiple output cells
                if((dest_is_src || is_second_change) && N_data.contract_data_type == DISALLOW_CONTRACT_DATA) {
                    REJECT("Can't handle self-transactions with multiple non-change destination addresses");
                }
            } else {
                G.maybe_transaction.v.flags |= HAS_DESTINATION_ADDRESS;
                memcpy(G.u.tx.outputs[0].destination.hash, dest_to_show, 20);
            }
        } else if(G.cell_state.lock_arg_nonequal) {
            if((G.maybe_transaction.v.flags & HAS_DESTINATION_ADDRESS) && memcmp(G.u.tx.outputs[0].destination.hash, G.lock_arg_tmp.hash, 20)) {
                if(G.maybe_transaction.v.tag != OPERATION_TAG_NOT_SET && G.maybe_transaction.v.tag != OPERATION_TAG_MULTI_OUTPUT_TRANSFER)
                    REJECT("Can't handle mixed transaction types with multiple non-change destination addresses. Tag: %d", G.maybe_transaction.v.tag);
                G.maybe_transaction.v.tag = OPERATION_TAG_MULTI_OUTPUT_TRANSFER;
                if(G.u.tx.output_count>=MAX_OUTPUTS) REJECT("Can't handle more than five outputs");
                G.u.tx.output_count++;
                memcpy(&G.u.tx.outputs[G.u.tx.output_count - 1].destination, &G.lock_arg_tmp, sizeof(lock_arg_t));
            } else if( !(G.maybe_transaction.v.flags & HAS_DESTINATION_ADDRESS) ) {
                G.maybe_transaction.v.flags |= HAS_DESTINATION_ADDRESS;
                G.u.tx.output_count++;
                memcpy(&G.u.tx.outputs[G.u.tx.output_count - 1].destination, &G.lock_arg_tmp, sizeof(lock_arg_t));
            }
            G.u.tx.plain_output_amount += G.cell_state.capacity;
            G.u.tx.outputs[G.u.tx.output_count - 1].capacity+=G.cell_state.capacity;
        } else {
            G.u.tx.change_amount += G.cell_state.capacity;
        }
    }
}

void validate_output_data_start(mol_num_t idx) {
    G.cell_state.is_dao = !((G.u.tx.dao_bitmask & 1<<idx) == 0);
}

void finish_output_cell_data(void) {
    if(G.cell_state.is_dao) {
        if(G.cell_state.data_size != 8) REJECT("DAO data must be 8 bytes");
        if(G.cell_state.dao_data_is_nonzero) {
            if(G.maybe_transaction.v.tag != OPERATION_TAG_DAO_PREPARE && G.maybe_transaction.v.tag != 0) REJECT("Can't mix deposit, prepare, and withdraw in one transaction");
            G.maybe_transaction.v.tag = OPERATION_TAG_DAO_PREPARE;
        } else {
            PRINTF("%d\n", G.maybe_transaction.v.tag);
            if(G.maybe_transaction.v.tag != OPERATION_TAG_DAO_DEPOSIT && G.maybe_transaction.v.tag != 0) REJECT("Can't mix deposit, prepare, and withdraw in one transaction");
            G.maybe_transaction.v.tag = OPERATION_TAG_DAO_DEPOSIT;
        }
    } else {
        if (G.cell_state.data_size != 0 && N_data.contract_data_type == DISALLOW_CONTRACT_DATA)
            REJECT("Data found in non-dao cell");
        if (G.cell_state.data_size > 0) // TODO: parse contract data here
            G.maybe_transaction.v.contract_type = CONTRACT_UNKNOWN;
    }
}

void finalize_raw_transaction(void) {
    switch(G.maybe_transaction.v.tag) {
        case OPERATION_TAG_NONE:
            break;
        case OPERATION_TAG_NOT_SET:
        case OPERATION_TAG_PLAIN_TRANSFER:
            if (G.distinct_input_sources > 1) {
                if (G.signing_multisig_input)
                    REJECT("Signing multi-input transaction with multisig input is not supported");
                G.maybe_transaction.v.tag = OPERATION_TAG_MULTI_INPUT_TRANSFER;
                // The input 'index' we are signing should have been set to non-zero value
                if (G.maybe_transaction.v.input_count.fst == 0)
                    REJECT("Internal error in handling multi-input transaction");
                G.maybe_transaction.v.input_count.snd = G.distinct_input_sources;
                // Display the complete input amount we are signing, without deducting change
                G.maybe_transaction.v.amount.fst = G.input_amount.fst;
                // In plain_output_amount, the change has been deducted
                G.maybe_transaction.v.amount.snd = G.u.tx.plain_output_amount;
            } else {
                G.maybe_transaction.v.tag = G.u.tx.is_self_transfer ?
                    OPERATION_TAG_SELF_TRANSFER : OPERATION_TAG_PLAIN_TRANSFER;
                G.maybe_transaction.v.amount.snd = G.u.tx.plain_output_amount;
            }
            break;
        // Shouldn't actually hit this case because of the handling of TAG_NOT_SET above
        case OPERATION_TAG_SELF_TRANSFER:
            G.maybe_transaction.v.amount.snd = G.u.tx.plain_output_amount;
            break;
        case OPERATION_TAG_MULTI_OUTPUT_TRANSFER:
            if(G.distinct_input_sources > 1) REJECT("Multi-input multi-output transactions are not supported");
            G.maybe_transaction.v.amount.snd = G.u.tx.plain_output_amount;
            break;
        case OPERATION_TAG_DAO_DEPOSIT:
            G.maybe_transaction.v.dao_amount = G.u.tx.dao_output_amount;
            break;
        case OPERATION_TAG_DAO_PREPARE:
            G.maybe_transaction.v.dao_amount = G.u.tx.dao_output_amount;
            if(G.u.tx.dao_output_amount != G.dao_input_amount) REJECT("DAO input and output amounts do not match for prepare operation"); // Not a complete check; full DAO requirement is that _each cell_ match exactly. Just providing some fail-fast here.
            break;
        case OPERATION_TAG_DAO_WITHDRAW:
            G.maybe_transaction.v.dao_amount = G.dao_input_amount;
            if(G.u.tx.dao_output_amount != 0) REJECT("Can't mix DAO withdraw and other DAO operations");
            if(G.u.tx.plain_output_amount != 0) REJECT("DAO withdrawals cannot be sent directly to another account");
            break;
    }
    G.maybe_transaction.v.total_fee = (G.input_amount.snd + G.dao_input_amount) - (G.u.tx.plain_output_amount + G.u.tx.dao_output_amount + G.u.tx.change_amount);
    if(G.maybe_transaction.v.tag == OPERATION_TAG_DAO_WITHDRAW) {
        // Can't compute fee without a bunch more info, calculating return instead and putting that in this slot so the user can get equivalent info.
        G.maybe_transaction.v.total_fee = -G.maybe_transaction.v.total_fee;
    }
    blake2b_finish_hash(G.u.tx.transaction_hash, sizeof(G.u.tx.transaction_hash), &G.hash_state);
}

// Process a whole transaction to sign.
const struct AnnotatedRawTransaction_callbacks AnnotatedRawTransaction_callbacks = {
    .offsets = computeNewOffsetsToHash,
    .version = &(Uint32_cb) { { blake2b_chunk } },
    .cell_deps = &(CellDepVec_cb) { .chunk = blake2b_chunk },
    .header_deps = &(Byte32Vec_cb) { .chunk = blake2b_chunk },
    .inputs = &(AnnotatedCellInputVec_cb) {
        .start = inputs_start,
        .length = blake2b_input_count,
        .item = &annotatedCellInput_callbacks,
        .end = finish_inputs
    },
    .outputs = &(CellOutputVec_cb) {
        .chunk = blake2b_chunk,
        .index = output_start,
        .item = &(CellOutput_cb) {
            .capacity = &(Uint64_cb) { { cell_capacity } },
            .lock = &(Script_cb) {
                .code_hash = &(Byte32_cb) { { cell_lock_code_hash } },
                .hash_type = &hash_type_cb,
                .args = &(Bytes_cb) { .body_chunk = script_arg_chunk }
            },
            .type_ = &(ScriptOpt_cb) {
                .item = &(Script_cb) {
                    .code_hash = &(Byte32_cb) { { cell_type_code_hash } },
                    .hash_type = &hash_type_cb,
                    .args = &(Bytes_cb) { .size = cell_type_arg_length }
                }
            },
            .end = output_end
        }
    },
    .outputs_data = &(BytesVec_cb) {
        .chunk = blake2b_chunk,
        .index = validate_output_data_start,
        .item = &(Bytes_cb) { .size = set_cell_data_size, .body_chunk = check_cell_data_data_chunk, .end = finish_output_cell_data }
    },
    .end = finalize_raw_transaction
};

void init_bip32(mol_num_t size) {
    if(size-4 > sizeof(G.u.temp_key.components)) REJECT_HARD("Too many components in bip32 path");
    G.u.temp_key.length=0;
}

void bip32_component(uint8_t* buf, mol_num_t len) {
    (void) len;
    G.u.temp_key.components[G.u.temp_key.length++]=*(uint32_t*)buf;
}

void set_sign_path(void) {
    bip32_path_t key;
    memcpy(&key, &G.u.temp_key, sizeof(key));
    prep_lock_arg(&key, &G.current_lock_arg);

    memcpy(G.key_path_components, key.components + 2, sizeof(G.key_path_components));
    G.key_length = key.length;

    // Default the change lock arg to the one we're currently going to sign for
    memcpy(&G.change_lock_arg, G.current_lock_arg, 20);
}

void set_change_path(void) {
    prep_lock_arg(&G.u.temp_key, &G.change_lock_arg);
}

void witness_offsets(struct WitnessArgs_state *state) {
    if (G.signing_multisig_input) {
        // Assume the WitnessArgs are correct
        mol_num_t header[4];
        header[0] = state->total_size;
        header[1] = state->lock_offset;
        header[2] = state->input_type_offset;
        header[3] = state->output_type_offset;

        uint64_t len64 = header[0];
        blake2b_incremental_hash((uint8_t*) &len64, sizeof(uint64_t), &G.hash_state);
        blake2b_incremental_hash((uint8_t*) header, sizeof(header), &G.hash_state);
        /* PRINTF("witness_multisig header %.*h\n", 16, header ); */
    } else {
        int lock_wit_len = state->input_type_offset-state->lock_offset;
        int shift = 69-lock_wit_len;
        mol_num_t new_header[4];
        new_header[0] = state->total_size + shift;
        new_header[1] = state->lock_offset;
        new_header[2] = state->input_type_offset + shift;
        new_header[3] = state->output_type_offset + shift;

        uint64_t len64 = new_header[0];
        blake2b_incremental_hash((uint8_t*) &len64, sizeof(uint64_t), &G.hash_state);
        blake2b_incremental_hash((uint8_t*) new_header, sizeof(new_header), &G.hash_state);
        static const uint8_t zero_witness[] = {
            0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        blake2b_incremental_hash(zero_witness, sizeof(zero_witness), &G.hash_state);
    }
}

void witness_lock_arg_size(mol_num_t size) {
    if (!G.signing_multisig_input) return;
    uint32_t size_adjusted = size-4;
    blake2b_incremental_hash((void*) &size_adjusted, 4, &G.hash_state);
}

// multisig_script | Signature1 | Signature2 | ...
//
// Where the components are of the following format:
//
// multisig_script: S | R | M | N | PubKeyHash1 | PubKeyHash2 | ...
//
// +-------------+------------------------------------+-------+
// |             |           Description              | Bytes |
// +-------------+------------------------------------+-------+
// | S           | reserved field, must be zero       |     1 |
// | R           | first nth public keys must match   |     1 |
// | M           | threshold                          |     1 |
// | N           | total public keys                  |     1 |
// | PubkeyHashN | blake160 hash of compressed pubkey |    20 |
// | SignatureN  | recoverable signature              |    65 |
// +-------------+------------------------------------+-------+

void witness_lock_arg_body_chunk(uint8_t *buf, mol_num_t buflen) {
    if (!G.signing_multisig_input) return;
    mol_num_t consumed = G.u.tx.witness_multisig_lock_arg_consumed;
    if (consumed == 0 && (buf[0] != 0))
        REJECT("Reserved field of multisig_script is non-zero");
    if (consumed < 3) {
        mol_num_t threshold_index = 2 - consumed;
        G.u.tx.witness_multisig_threshold = buf[threshold_index];
    }
    if (consumed < 4) {
        mol_num_t pubkeys_cnt_index = 3 - consumed;
        G.u.tx.witness_multisig_pubkeys_cnt = buf[pubkeys_cnt_index];
    }
    if (G.u.tx.witness_multisig_pubkeys_cnt > 0) {
        size_t multisig_script_len = 4 /* FLAGS_SIZE */ + 20 /* BLAKE160_SIZE */ * G.u.tx.witness_multisig_pubkeys_cnt;
        size_t signatures_start = multisig_script_len - consumed;
        if (signatures_start > 0 && buflen > signatures_start) {
            // Zero signatures
            for(mol_num_t i = signatures_start; i < buflen; i++) {
                buf[i] = 0;
            }
        }
    }
    /* PRINTF("witness threshold %d\n", G.u.tx.witness_multisig_threshold); */
    /* PRINTF("witness pubkeys_cnt %d\n", G.u.tx.witness_multisig_pubkeys_cnt); */
    /* PRINTF("witness_lock_arg_body_chunk %.*h\n", buflen, buf); */
    blake2b_chunk(buf, buflen);
    G.u.tx.witness_multisig_lock_arg_consumed += buflen;
}

const WitnessArgs_cb WitnessArgs_rewrite_callbacks = {
    .offsets = witness_offsets,
    .lock = &(BytesOpt_cb) { .item = &(Bytes_cb) {
        .size = witness_lock_arg_size,
        .body_chunk = witness_lock_arg_body_chunk
    }},
    .input_type = &(BytesOpt_cb) { .chunk = blake2b_chunk },
    .output_type = &(BytesOpt_cb) { .chunk = blake2b_chunk }
};

void begin_witness(mol_num_t index) {
    G.u.tx.is_first_witness = index == 0;
    if(G.u.tx.is_first_witness) {
        G.u.tx.witness_multisig_threshold = 0;
        G.u.tx.witness_multisig_pubkeys_cnt = 0;
        G.u.tx.witness_multisig_lock_arg_consumed = 0;
        explicit_bzero(&G.hash_state, sizeof(G.hash_state));
        blake2b_incremental_hash(G.u.tx.transaction_hash, SIGN_HASH_SIZE, &G.hash_state);
        MolReader_WitnessArgs_init_state((struct WitnessArgs_state*)G.u.tx.witness_stack, &WitnessArgs_rewrite_callbacks);
    }
}

void hash_witness_length(mol_num_t size) {
    if(!(G.u.tx.is_first_witness)) {
        uint64_t size_as_64 = size-4;
        blake2b_incremental_hash((void*) &size_as_64, 8, &G.hash_state);
    }
}

void process_witness(uint8_t *buff, mol_num_t buff_size) {
  if(G.u.tx.is_first_witness) { // First witness handling

    struct mol_chunk chunk = { buff, buff_size, 0 };
    mol_rv rv = MolReader_WitnessArgs_parse((struct WitnessArgs_state*)G.u.tx.witness_stack, &chunk, &WitnessArgs_rewrite_callbacks, MOL_NUM_MAX);

    if(rv == COMPLETE) {
        G.u.tx.first_witness_done=1;
    }
  } else {
      blake2b_incremental_hash(buff, buff_size, &G.hash_state);
  }
}

void process_witness_end() {
    // If something went wrong parsing the first arg, just assume that it's the usual empty one.
    if(G.u.tx.is_first_witness && G.u.tx.first_witness_done!=1) {
        G.u.tx.first_witness_done=1;
        explicit_bzero(&G.hash_state, sizeof(G.hash_state));
        blake2b_incremental_hash(G.u.tx.transaction_hash, SIGN_HASH_SIZE, &G.hash_state);
        static const uint8_t self_witness[] = {
            0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,             // Length of WitnessArg,
            0x55, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x55, 0x00, // WitnessArg
            0x00, 0x00, 0x55, 0x00, 0x00, 0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        blake2b_incremental_hash(self_witness, sizeof(self_witness), &G.hash_state);
    }
}

void finalize_witnesses() {
    blake2b_finish_hash(G.u.tx.final_hash, sizeof(G.u.tx.final_hash), &G.hash_state);
}

void set_input_count(uint8_t *buf, mol_num_t len) {
    (void) len;
    G.input_count=*(uint32_t*) buf;
}

const struct AnnotatedTransaction_callbacks annotatedTransaction_callbacks = {
    .signPath = &(Bip32_cb) { .size = init_bip32, .item = &(Uint32_cb) { { bip32_component } }, .end = set_sign_path },
    .changePath = &(Bip32_cb) { .size = init_bip32, .item = &(Uint32_cb) { { bip32_component } }, .end = set_change_path },
    .inputCount = &(Uint32_cb) { { set_input_count } },
    .raw = &AnnotatedRawTransaction_callbacks,
    .witnesses = &(BytesVec_cb) {
        .index = begin_witness,
        .item = &(Bytes_cb) { .size = hash_witness_length, .body_chunk = process_witness, .end = process_witness_end },
        .end = finalize_witnesses
    }
};

#define P1_FIRST            0x00
#define P1_NEXT             0x01
#define P1_NO_FALLBACK      0x40
#define P1_LAST_MARKER      0x80
#define P1_MASK             (~(P1_LAST_MARKER | P1_NO_FALLBACK))

static size_t handle_apdu(uint8_t const instruction) {

    uint8_t *const buff = &G_io_apdu_buffer[OFFSET_CDATA];
    uint8_t const p1 = READ_UNALIGNED_BIG_ENDIAN(uint8_t, &G_io_apdu_buffer[OFFSET_P1]);
    uint8_t const buff_size = READ_UNALIGNED_BIG_ENDIAN(uint8_t, &G_io_apdu_buffer[OFFSET_LC]);
    if (buff_size > MAX_APDU_SIZE)
        THROW(EXC_WRONG_LENGTH_FOR_INS);

    bool last = (p1 & P1_LAST_MARKER) != 0;
    bool secure_only = (p1 & P1_NO_FALLBACK) != 0;
    struct mol_chunk chunk = { buff, buff_size, 0 };
    mol_rv rv;

    switch (p1 & P1_MASK) {
        case P1_FIRST:
            clear_data();

            PRINTF("Initializing parser\n");
            MolReader_AnnotatedTransaction_init_state((struct AnnotatedTransaction_state*) &G.transaction_stack, &annotatedTransaction_callbacks);
            PRINTF("Initialized parser\n");
            // NO BREAK
        case P1_NEXT:
            if(G.maybe_transaction.hard_reject) THROW(EXC_PARSE_ERROR);
            PRINTF("Calling parser\n");
            rv = MolReader_AnnotatedTransaction_parse((struct AnnotatedTransaction_state*) &G.transaction_stack, &chunk, &annotatedTransaction_callbacks, MOL_NUM_MAX);

            if(last) {
                if(rv != COMPLETE || G.maybe_transaction.hard_reject) {
                    PRINTF("Parse failed. %02d\n", rv);
                    G.maybe_transaction.hard_reject = true;
                    THROW(EXC_PARSE_ERROR);
                }
                if(G.maybe_transaction.unsafe && secure_only) THROW(EXC_SECURITY);
                // Override if we have an unsafe but valid transaction, so we don't prompt the user like it's a transfer.
                if(G.maybe_transaction.unsafe) G.maybe_transaction.v.tag=OPERATION_TAG_NONE;
                G.maybe_transaction.is_valid = true;
            } else {
                if(rv != INCOMPLETE) {
                    G.maybe_transaction.hard_reject = true;
                    THROW(EXC_PARSE_ERROR);
                }
            }

            break;
    }

    if (last && G.maybe_transaction.is_valid) {
        // We already computed the hash above, so just proceed to sign_complete.
        return sign_complete(instruction);
    } else {
        return finalize_successful_send(0);
    }
}

_Static_assert(sizeof G.transaction_stack == sizeof(struct AnnotatedTransaction_state), "Size of transaction_stack is not equal to sizeof(struct AnnotatedTransaction_state)");
_Static_assert(sizeof G.u.tx.witness_stack == sizeof(struct WitnessArgs_state), "Size of witness_stack is not equal to sizeof(struct WitnessArgs_state)");

size_t handle_apdu_sign(uint8_t instruction) {
    return handle_apdu(instruction);
}

static int perform_signature(bool const on_hash, bool const send_hash) {
    if (on_hash && G.u.tx.hash_only) {
        memcpy(G_io_apdu_buffer, G.u.tx.final_hash, sizeof(G.u.tx.final_hash));
        clear_data();
        return finalize_successful_send(sizeof(G.u.tx.final_hash));
    }

    size_t tx = 0;
    if (send_hash && on_hash) {
        memcpy(&G_io_apdu_buffer[tx], G.u.tx.final_hash, sizeof(G.u.tx.final_hash));
        tx += sizeof(G.u.tx.final_hash);
    }

    uint8_t const *const data = G.u.tx.final_hash;        // on_hash ? G.u.tx.final_hash : G.message_data;
    size_t const data_length = sizeof(G.u.tx.final_hash); // on_hash ? sizeof(G.u.tx.final_hash) : G.message_data_length;

    bip32_path_t key;
    key.components[0] = 0x8000002C;
    key.components[1] = 0x80000135;
    key.length = G.key_length;
    memcpy(key.components + 2, G.key_path_components, sizeof(G.key_path_components));

    tx += WITH_KEY_PAIR(key, key_pair, size_t,
                        ({ sign(&G_io_apdu_buffer[tx], MAX_SIGNATURE_SIZE, key_pair, data, data_length); }));

    clear_data();
    return finalize_successful_send(tx);
}

/***********************************************************/
static inline void clear_message_data(void) {
  memset(&global.apdu.u.sign_msg, 0, sizeof(global.apdu.u.sign_msg));
}

static inline void clear_message_hash_data(void) {
  memset(&global.apdu.u.sign_msg_hash, 0, sizeof(global.apdu.u.sign_msg_hash));
}

static int perform_message_signature() {
  apdu_sign_message_state_t *g_sign_msg = &global.apdu.u.sign_msg;
  uint8_t *const data = g_sign_msg->final_hash;
  uint8_t const data_size = sizeof(g_sign_msg->final_hash);
  size_t final_size = 0;

  final_size+=WITH_KEY_PAIR(g_sign_msg->key, key_pair, size_t,
                      ({ sign(&G_io_apdu_buffer[final_size], MAX_SIGNATURE_SIZE, key_pair, data, data_size); }));
  clear_message_data();
  return finalize_successful_send(final_size);
}

static bool sign_message_ok(void) {
  delayed_send(perform_message_signature());
  return true;
}

static bool check_magic_bytes(uint8_t const * message, uint8_t message_len) {
  const char nervos_magic[] = "Nervos Message:";
  if(message_len == 0 || message_len < (sizeof(nervos_magic)- 1)) return false; //If the message is shorter, it dont work
  int cmp = memcmp(message, &nervos_magic, sizeof(nervos_magic)-1);
  return (0 == cmp); //cut off the str's nullbyte
}

static void copy_buffer(char *const out, size_t const out_size, buffer_t const *const in) {
  if(in->size > out_size) THROW(EXC_MEMORY_ERROR);

  // if we dont do this then we have stuff from the old buffer getting displayed
  memset(out, 0, out_size);
  memcpy(out, in->bytes, in->size);
}

static void slice_magic_bytes(char *buff, uint8_t *buff_size) {
  const char magic_bytes[] = "Nervos Message:";
  //remove string terminator when comparing
  size_t magic_bytes_size = sizeof(magic_bytes)- 1;
  if(0 == memcmp(buff, magic_bytes, magic_bytes_size)) {
    size_t num_bytes_to_copy = *buff_size - magic_bytes_size;
    memmove(buff, &buff[magic_bytes_size], num_bytes_to_copy);
    *buff_size = num_bytes_to_copy;
  }
  else THROW(EXC_PARSE_ERROR);

}

static void replace_undisplayable(uint8_t *buff, uint8_t *buff_size) {
  const uint8_t four_bytes = 240; // 1111 0000
  const uint8_t three_bytes = 224; // 1110 0000
  const uint8_t two_bytes = 192;  // 1100 0000
  uint8_t tmp_buff [*buff_size];
  memcpy(tmp_buff, buff, *buff_size);
  memset(buff, 0, *buff_size);
  const uint8_t tmp_size = *buff_size;
  for(size_t i = 0, j = 0; i < tmp_size; i++, j++) {
    bool cant_display = tmp_buff[i] > 126 || tmp_buff[i] < 32;
    if(cant_display) {
      buff[j] = '*';
      // Check if char is represented by multiple bytes and remove them
      if((tmp_buff[i] & four_bytes) == four_bytes){
        i+=3;
        *buff_size -=3;
      }
      else if((tmp_buff[i] & three_bytes) == three_bytes){
        i+=2;
        *buff_size -=2;
      }
      else if((tmp_buff[i] & two_bytes) == two_bytes){
        i+=1;
        *buff_size -=1;
      }
    }
    else {
      buff[j] = tmp_buff[i];
    }
  }
}
static void handle_long_message(uint8_t *buff, uint8_t *buff_size) {
  if(*buff_size > 64) {
    *buff_size = 64;
    buff[61] = '.';
    buff[62] = '.';
    buff[63] = '.';
  }
}

/* Sign message                                                        */
/***********************************************************************/
static size_t handle_apdu_sign_message_impl(uint8_t const __attribute__((unused)) _instruction) {
  uint8_t *const buff = &G_io_apdu_buffer[OFFSET_CDATA];
  uint8_t const buff_size = READ_UNALIGNED_BIG_ENDIAN(uint8_t, &G_io_apdu_buffer[OFFSET_LC]);
  uint8_t const p1 = READ_UNALIGNED_BIG_ENDIAN(uint8_t, &G_io_apdu_buffer[OFFSET_P1]);
  if (buff_size > MAX_APDU_SIZE) THROW(EXC_WRONG_LENGTH_FOR_INS);
  apdu_sign_message_state_t *g_sign_msg = &global.apdu.u.sign_msg;
  bool last = (p1 & P1_LAST_MARKER) != 0;
  switch (p1 & ~P1_LAST_MARKER) {
    case P1_FIRST:
      // Must contain 1 byte as display-flag + bip32path
      clear_message_data();
      if(buff_size <= 1) THROW(EXC_REJECT);
      g_sign_msg->display_as_hex = READ_UNALIGNED_BIG_ENDIAN(bool, buff);
      read_bip32_path(&g_sign_msg->key, buff+1, buff_size-1);
      return finalize_successful_send(0);
    case P1_NEXT:
      // Guard against overflow
      if (g_sign_msg->packet_index >= 0xFF) PARSE_ERROR();
      g_sign_msg->packet_index++;
      break;
    default:
        THROW(EXC_WRONG_PARAM);
  }

  if (g_sign_msg->packet_index == 1) {
    // Ensure the message begins with "Nervos Message:"
    if(!check_magic_bytes(buff, buff_size)) THROW(EXC_PARSE_ERROR);

    uint8_t tmp_msg_buff[buff_size];
    uint8_t tmp_msg_buff_size = buff_size;

    // Move msg, so we dont hash the same data that we mutate
    memcpy(tmp_msg_buff, buff, tmp_msg_buff_size);

    //Remove magic bytes, because, even though we sign them, the user should not be aware of their existence
    slice_magic_bytes((char*)tmp_msg_buff, &tmp_msg_buff_size);
    if(!g_sign_msg->display_as_hex) {
      // If we are not displaying the hex, then replace all the non-displayable chars with '*'
      replace_undisplayable(tmp_msg_buff, &tmp_msg_buff_size);
    }
    handle_long_message(tmp_msg_buff, &tmp_msg_buff_size);

    // Move tmp to global storage
    memcpy(&g_sign_msg->display, tmp_msg_buff, tmp_msg_buff_size);

    //Convert to buffer
    g_sign_msg->display_as_buffer.bytes = g_sign_msg->display;
    g_sign_msg->display_as_buffer.size = tmp_msg_buff_size;
    g_sign_msg->display_as_buffer.length = tmp_msg_buff_size;
  }
  blake2b_incremental_hash(buff, buff_size, &g_sign_msg->hash_state);
  if(last) {
    blake2b_finish_hash(g_sign_msg->final_hash, sizeof(g_sign_msg->final_hash), &g_sign_msg->hash_state);

    // Display the message
    static const char *const message_prompts[] = { PROMPT("Sign"), PROMPT("Message: "), NULL};
    REGISTER_STATIC_UI_VALUE(0, "Message");
    if(g_sign_msg->display_as_hex) {
      register_ui_callback(1, buffer_to_hex, &g_sign_msg->display_as_buffer);
    } else {
      register_ui_callback(1, copy_buffer, &g_sign_msg->display_as_buffer);
    }
    ui_callback_t const ok_sign = sign_message_ok;

    // Prompt and sign hash
    ui_prompt(message_prompts, ok_sign, sign_reject);
  }
  else {
      return finalize_successful_send(0);
  }
}

size_t handle_apdu_sign_message(uint8_t instruction) {
  return handle_apdu_sign_message_impl(instruction);
}

/***********************************************************************/
/* Sign message hash                                                   */
/***********************************************************************/
static int perform_message_hash_signature() {
  // g_smh --> Global_Sign_Message_Hash
  apdu_sign_message_hash_state_t *g_smh = &global.apdu.u.sign_msg_hash;
  uint8_t *const data = g_smh->hash_to_sign;
  uint8_t const data_size = g_smh->hash_to_sign_size;
  size_t final_size = 0;
  final_size+=WITH_KEY_PAIR(g_smh->key, key_pair, size_t,
                      ({ sign(&G_io_apdu_buffer[final_size], MAX_SIGNATURE_SIZE, key_pair, data, data_size); }));
  clear_message_hash_data();
  return finalize_successful_send(final_size);
}

static bool sign_message_hash_ok(void) {
  delayed_send(perform_message_hash_signature());
  return true;
}

static size_t handle_apdu_sign_message_hash_impl(void) {
  uint8_t *const buff = &G_io_apdu_buffer[OFFSET_CDATA];
  uint8_t const buff_size = READ_UNALIGNED_BIG_ENDIAN(uint8_t, &G_io_apdu_buffer[OFFSET_LC]);
  uint8_t const p1 = READ_UNALIGNED_BIG_ENDIAN(uint8_t, &G_io_apdu_buffer[OFFSET_P1]);
  if (buff_size > MAX_APDU_SIZE) THROW(EXC_WRONG_LENGTH_FOR_INS);

  // g_smh --> Global_Sign_Message_Hash
  apdu_sign_message_hash_state_t *g_smh = &global.apdu.u.sign_msg_hash;
  switch (p1) {
    case P1_FIRST:
      clear_message_hash_data();
      read_bip32_path(&g_smh->key, buff, buff_size);
      return finalize_successful_send(0);
    case P1_LAST_MARKER:
      // If the hash is > than 32 bytes, we can't display it (64 chars is the limit, and each byte gets displayed as 2 chars)
      if(buff_size > 32) PARSE_ERROR();
      memcpy(g_smh->hash_to_sign, buff, buff_size);
      g_smh->hash_to_sign_size = buff_size;
      break;
    default:
        THROW(EXC_WRONG_PARAM);
  }
  g_smh->display_as_buffer.bytes = g_smh->hash_to_sign;
  g_smh->display_as_buffer.size = g_smh->hash_to_sign_size;
  g_smh->display_as_buffer.length = g_smh->hash_to_sign_size;

  static const char *const message_prompts[] = { PROMPT("Sign"), PROMPT("Message Hash: "), NULL};
  REGISTER_STATIC_UI_VALUE(0, "Message Hash");
  register_ui_callback(1, buffer_to_hex, &g_smh->display_as_buffer);
  ui_callback_t const ok_sign = sign_message_hash_ok;
  // Prompt and sign hash
  ui_prompt(message_prompts, ok_sign, sign_reject);
}

size_t handle_apdu_sign_message_hash(uint8_t instruction) {
  if(N_data.sign_hash_type == SIGN_HASH_ON)
    return handle_apdu_sign_message_hash_impl();
  else
    THROW(EXC_REJECT);
}

