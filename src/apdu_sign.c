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

    case OPERATION_TAG_PLAIN_TRANSFER: {
        static const uint32_t TYPE_INDEX = 0;
        static const uint32_t AMOUNT_INDEX = 1;
        static const uint32_t FEE_INDEX = 2;
        static const uint32_t DESTINATION_INDEX = 3;
        static const char *const transaction_prompts[] = {PROMPT("Confirm"), PROMPT("Amount"),      PROMPT("Fee"),
                                                          PROMPT("Destination"), NULL};
        REGISTER_STATIC_UI_VALUE(TYPE_INDEX, "Transaction");
        //register_ui_callback(SOURCE_INDEX, lock_arg_to_address, &G.maybe_transaction.v.source);
        register_ui_callback(DESTINATION_INDEX, lock_arg_to_address, &G.maybe_transaction.v.destination);
        register_ui_callback(FEE_INDEX, frac_ckb_to_string_indirect, &G.maybe_transaction.v.total_fee);
        register_ui_callback(AMOUNT_INDEX, frac_ckb_to_string_indirect, &G.maybe_transaction.v.amount);

        ui_prompt(transaction_prompts, ok_c, sign_reject);

    } break;
    case OPERATION_TAG_DAO_DEPOSIT: {
        static const uint32_t TYPE_INDEX = 0;
        static const uint32_t AMOUNT_INDEX = 1;
        static const uint32_t FEE_INDEX = 2;
        static const uint32_t DESTINATION_INDEX = 3;
        static const char *const transaction_prompts[] = {PROMPT("Confirm DAO"), PROMPT("Amount"), PROMPT("Fee"),
                                                          NULL};
        REGISTER_STATIC_UI_VALUE(TYPE_INDEX, "Deposit");
        // register_ui_callback(SOURCE_INDEX, lock_arg_to_address, &G.maybe_transaction.v.source);
        register_ui_callback(DESTINATION_INDEX, lock_arg_to_address, &G.maybe_transaction.v.destination);
        register_ui_callback(FEE_INDEX, frac_ckb_to_string_indirect, &G.maybe_transaction.v.total_fee);
        register_ui_callback(AMOUNT_INDEX, frac_ckb_to_string_indirect, &G.maybe_transaction.v.dao_amount);

        ui_prompt(transaction_prompts, ok_c, sign_reject);

    } break;
    case OPERATION_TAG_DAO_PREPARE: {
        static const uint32_t TYPE_INDEX = 0;
        static const uint32_t AMOUNT_INDEX = 1;
        static const uint32_t FEE_INDEX = 2;
        static const char *const prepare_prompts_full[] = {
            PROMPT("Confirm DAO"), PROMPT("Amount"),    PROMPT("Fee"), NULL };
        REGISTER_STATIC_UI_VALUE(TYPE_INDEX, "Prepare");
        register_ui_callback(AMOUNT_INDEX, frac_ckb_to_string_indirect, &G.maybe_transaction.v.dao_amount);
        register_ui_callback(FEE_INDEX, frac_ckb_to_string_indirect, &G.maybe_transaction.v.total_fee);
        ui_prompt(prepare_prompts_full,
                  ok_c, sign_reject);
        break;
    }
    case OPERATION_TAG_DAO_WITHDRAW: {
        static const uint32_t TYPE_INDEX = 0;
        static const uint32_t DEPOSIT_INDEX = 1;
        static const uint32_t RETURN_INDEX = 2;
        //static const uint32_t SOURCE_INDEX = 3;
        static const char *const transaction_prompts[] = {PROMPT("Confirm DAO"),
                                                          PROMPT("Deposit"),
                                                          PROMPT("Return"),
        //                                                  PROMPT("Source"),
                                                          NULL};
        REGISTER_STATIC_UI_VALUE(TYPE_INDEX, "Withdrawal");
        register_ui_callback(DEPOSIT_INDEX, frac_ckb_to_string_indirect, &G.maybe_transaction.v.dao_amount);
        register_ui_callback(RETURN_INDEX, frac_ckb_to_string_indirect, &G.maybe_transaction.v.total_fee);
        //register_ui_callback(SOURCE_INDEX, lock_arg_to_address, &G.maybe_transaction.v.dao_source);

        ui_prompt(transaction_prompts, ok_c, sign_reject);

    } break;
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
    extended_public_key_t ext_public_key;
    generate_public_key(&ext_public_key, key);
    generate_lock_arg_for_pubkey(&ext_public_key.public_key, destination);
}

/* Start of parser callbacks */

void blake2b_chunk(uint8_t* buf, mol_num_t len) {
    blake2b_incremental_hash(buf, len, &G.hash_state);
}

void input_start() {
    explicit_bzero(&G.cell_state, sizeof(G.cell_state));
    explicit_bzero((void*)&G.input_state, sizeof(G.input_state));
}

void input_save_tx_hash(uint8_t *hash, mol_num_t hash_length) {
    (void) hash_length;// hash_length guaranteed by parser
    memcpy(G.input_state.tx_hash, hash, sizeof(G.input_state.tx_hash));
}

void input_save_index(uint8_t *index, mol_num_t index_length) {
    (void) index_length; // guaranteed by parser
    memcpy(&G.input_state.index, index, sizeof(G.input_state.index));
}

void context_blake2b_chunk(uint8_t *chunk, mol_num_t length) {
    blake2b_incremental_hash(chunk, length, &G.input_state.hash_state);
}

void validate_context_txn(void) {
    uint8_t tx_hash[32];
    blake2b_finish_hash(tx_hash, 32, &G.input_state.hash_state);
    if(memcmp(tx_hash, G.input_state.tx_hash, 32))
        REJECT_HARD("Context transaction does not match hash");
}

void input_context_start_idx(mol_num_t idx) {
    // Enable/disable the remaining input callbacks based on whether we're on that output.
    G.cell_state.active = idx == G.input_state.index;
}

void cell_capacity(uint8_t* capacity, mol_num_t len) {
    (void)len; // constant from the parser
    if(!G.cell_state.active) return;
    G.cell_state.capacity = *(uint64_t*) capacity;
}

void cell_lock_code_hash(uint8_t* buf, mol_num_t len) {
    (void)len;
    if(!G.cell_state.active) return;
    static const uint8_t defaultLockScript[] = {0x9b, 0xd7, 0xe0, 0x6f, 0x3e, 0xcf, 0x4b, 0xe0, 0xf2, 0xfc, 0xd2,
                                                0x18, 0x8b, 0x23, 0xf1, 0xb9, 0xfc, 0xc8, 0x8e, 0x5d, 0x4b, 0x65,
                                                0xa8, 0x63, 0x7b, 0x17, 0x72, 0x3b, 0xbd, 0xa3, 0xcc, 0xe8};

    if(memcmp(buf, defaultLockScript, 32)) REJECT("Only the standard lock script is currently supported");
}

void cell_script_hash_type(uint8_t hash_type) {
    if(!G.cell_state.active) return;
    if (hash_type != 1) REJECT("Incorrect hash type for standard lock or dao script");
}

void script_arg_start_input() {
    if(!G.cell_state.active) return;
    G.cell_state.lock_arg_index = 0;
    G.cell_state.lock_arg_nonequal = 0;
    G.lock_arg_cmp = G.current_lock_arg;
}

void script_arg_chunk(uint8_t* buf, mol_num_t buflen) {
    if(!G.cell_state.active) return;
    uint32_t current_offset = G.cell_state.lock_arg_index;
    if(G.cell_state.lock_arg_index+buflen > 20) { // Probably not possible.
        G.cell_state.lock_arg_nonequal |= true;
        return;
    }
    memcpy(G.lock_arg_tmp+current_offset, buf, buflen);
    G.cell_state.lock_arg_index+=buflen;

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
    if(G.cell_state.lock_arg_nonequal)
        REJECT("Can't securely sign transactions containing inputs we don't control");
}

void cell_type_code_hash(uint8_t* buf, mol_num_t len) {
    if(!G.cell_state.active) return;
    (void) len; // Guaranteed to be 32
    static const uint8_t dao_type_script_hash[] = {0x82, 0xd7, 0x6d, 0x1b, 0x75, 0xfe, 0x2f, 0xd9, 0xa2, 0x7d, 0xfb,
                                                   0xaa, 0x65, 0xa0, 0x39, 0x22, 0x1a, 0x38, 0x0d, 0x76, 0xc9, 0x26,
                                                   0xf3, 0x78, 0xd3, 0xf8, 0x1c, 0xf3, 0xe7, 0xe1, 0x3f, 0x2e};
    if(!G.cell_state.active) return;

    // If this exists, we require it to be the DAO for now. Verify.
    if(memcmp(buf, dao_type_script_hash, sizeof(dao_type_script_hash)))
        REJECT("Only the DAO type script is supported");
    G.cell_state.is_dao = true;
}

void cell_type_arg_length(mol_num_t length) {
    if(!G.cell_state.active) return;
    // DAO is empty.
    if(length != 4) REJECT("DAO cell has nonempty args");
}

void set_cell_data_size(mol_num_t size) {
    if(!G.cell_state.active) return;
    G.cell_state.data_size = size-4; // size includes the 4-byte size header in Bytes
}

void check_cell_data_data_chunk(uint8_t *buf, mol_num_t length) {
    if(!G.cell_state.active) return;
    for(mol_num_t i=0;i<length;i++)
        G.cell_state.dao_data_is_nonzero |= buf[i];
}

void finish_input_cell_data() {
    if(!G.cell_state.active) return;
    if(G.cell_state.is_dao) {
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
        if(G.cell_state.data_size !=0) {
            REJECT("Data found in non-dao cell");
        }
        G.plain_input_amount += G.cell_state.capacity;
    }
}

const struct byte_callbacks hash_type_cb = { cell_script_hash_type };

// Process one input, provided as an AnnotatedCellInput.
const AnnotatedCellInput_cb annotatedCellInput_callbacks = {
    .start = input_start,
    .input = &(CellInput_cb) {
        .chunk = blake2b_chunk,
        .previous_output = &(OutPoint_cb) {
            .tx_hash = &(Byte32_cb) { { input_save_tx_hash } },
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
        .end = validate_context_txn,
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
    G.current_output_index=index;
    explicit_bzero((void*) &G.cell_state, sizeof(G.cell_state));
    G.cell_state.active = true;
    G.lock_arg_cmp=G.change_lock_arg;
}

void output_end(void) {
    if(G.cell_state.is_dao) {
        G.dao_output_amount += G.cell_state.capacity;
        G.dao_bitmask |= 1<<G.current_output_index;
    } else {
        if(G.cell_state.lock_arg_nonequal) {
            G.plain_output_amount += G.cell_state.capacity;
            if((G.maybe_transaction.v.flags & HAS_DESTINATION_ADDRESS) && memcmp(G.maybe_transaction.v.destination, G.lock_arg_tmp, 20)) {
                REJECT("Can't handle transactions with multiple non-change destination addresses");
            } else {
                G.maybe_transaction.v.flags |= HAS_DESTINATION_ADDRESS;
                memcpy(G.maybe_transaction.v.destination, G.lock_arg_tmp, 20);
            }

        } else {
            G.change_amount += G.cell_state.capacity;
        }
    }
}

void validate_output_data_start(mol_num_t idx) {
    G.cell_state.is_dao = !((G.dao_bitmask & 1<<idx) == 0);
}

void finish_output_cell_data(void) {
    if(G.cell_state.is_dao) {
        if(G.cell_state.data_size != 8) REJECT("DAO data must be 8 bytes");
        if(G.cell_state.dao_data_is_nonzero) {
            if(G.maybe_transaction.v.tag != OPERATION_TAG_DAO_PREPARE && G.maybe_transaction.v.tag != 0) REJECT("Can't mix deposit, prepare, and withdraw in one transaction");
            G.maybe_transaction.v.tag = OPERATION_TAG_DAO_PREPARE;
        } else {
            if(G.maybe_transaction.v.tag != OPERATION_TAG_DAO_DEPOSIT && G.maybe_transaction.v.tag != 0) REJECT("Can't mix deposit, prepare, and withdraw in one transaction");
            G.maybe_transaction.v.tag = OPERATION_TAG_DAO_DEPOSIT;
        }
    } else {
        if(G.cell_state.data_size !=0) REJECT("Data found in non-dao cell");
    }
}

void finalize_raw_transaction(void) {
    switch(G.maybe_transaction.v.tag) {
        case OPERATION_TAG_NONE:
            break;
        case OPERATION_TAG_NOT_SET:
        case OPERATION_TAG_PLAIN_TRANSFER:
            G.maybe_transaction.v.tag = OPERATION_TAG_PLAIN_TRANSFER;
            G.maybe_transaction.v.amount = G.plain_output_amount;
            break;
        case OPERATION_TAG_DAO_DEPOSIT:
            G.maybe_transaction.v.dao_amount = G.dao_output_amount;
            break;
        case OPERATION_TAG_DAO_PREPARE:
            G.maybe_transaction.v.dao_amount = G.dao_output_amount;
            if(G.dao_output_amount != G.dao_input_amount) REJECT("DAO input and output amounts do not match for prepare operation"); // Not a complete check; full DAO requirement is that _each cell_ match exactly. Just providing some fail-fast here.
            break;
        case OPERATION_TAG_DAO_WITHDRAW:
            G.maybe_transaction.v.dao_amount = G.dao_input_amount;
            if(G.dao_output_amount != 0) REJECT("Can't mix DAO withdraw and other DAO operations");
            if(G.plain_output_amount != 0) REJECT("DAO withdrawals cannot be sent directly to another account");
            break;
    }
    G.maybe_transaction.v.total_fee = (G.plain_input_amount + G.dao_input_amount) - (G.plain_output_amount + G.dao_output_amount + G.change_amount);
    if(G.maybe_transaction.v.tag == OPERATION_TAG_DAO_WITHDRAW) {
        // Can't compute fee without a bunch more info, calculating return instead and putting that in this slot so the user can get equivalent info.
        G.maybe_transaction.v.total_fee = -G.maybe_transaction.v.total_fee;
    }
    blake2b_finish_hash(G.transaction_hash, sizeof(G.transaction_hash), &G.hash_state);
}

// Process a whole transaction to sign.
const struct AnnotatedRawTransaction_callbacks AnnotatedRawTransaction_callbacks = {
    .offsets = computeNewOffsetsToHash,
    .version = &(Uint32_cb) { { blake2b_chunk } },
    .cell_deps = &(CellDepVec_cb) { .chunk = blake2b_chunk },
    .header_deps = &(Byte32Vec_cb) { .chunk = blake2b_chunk },
    .inputs = &(AnnotatedCellInputVec_cb) {
        .length = blake2b_input_count,
        .item = &annotatedCellInput_callbacks
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
    if(size-4 > sizeof(G.temp_key.components)) REJECT_HARD("Too many components in bip32 path");
    G.temp_key.length=0;
}

void bip32_component(uint8_t* buf, mol_num_t len) {
    (void) len;
    G.temp_key.components[G.temp_key.length++]=*(uint32_t*)buf;
}

void set_sign_path(void) {
    memcpy(&G.key, &G.temp_key, sizeof(G.key));
    prep_lock_arg(&G.key, &G.current_lock_arg);
    // Default the change lock arg to the one we're currently going to sign for
    memcpy(&G.change_lock_arg, G.current_lock_arg, 20);
}

void set_change_path(void) {
    prep_lock_arg(&G.temp_key, &G.change_lock_arg);
}

void witness_offsets(struct WitnessArgs_state *state) {
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

void witness_chunk(uint8_t* chunk, mol_num_t length) {
    blake2b_incremental_hash(chunk, length, &G.hash_state);
}

const WitnessArgs_cb WitnessArgs_rewrite_callbacks = {
    .offsets = witness_offsets,
    .input_type = &(BytesOpt_cb) { .chunk = witness_chunk },
    .output_type = &(BytesOpt_cb) { .chunk = witness_chunk }
};

void begin_witness(mol_num_t index) {
    G.witness_idx = index;
    if(G.witness_idx==0) {
        explicit_bzero(&G.hash_state, sizeof(G.hash_state));
        blake2b_incremental_hash(G.transaction_hash, SIGN_HASH_SIZE, &G.hash_state);
        MolReader_WitnessArgs_init_state(G.witness_stack+sizeof(G.witness_stack), (struct WitnessArgs_state*)G.witness_stack, &WitnessArgs_rewrite_callbacks);
    }
}

void hash_witness_length(mol_num_t size) {
    if(!(G.witness_idx==0)) {
        uint64_t size_as_64 = size-4;
        blake2b_incremental_hash((void*) &size_as_64, 8, &G.hash_state);
    }
}

void process_witness(uint8_t *buff, mol_num_t buff_size) {
  if(G.witness_idx == 0) { // First witness handling

    struct mol_chunk chunk = { buff, buff_size, 0 };
    mol_rv rv = MolReader_WitnessArgs_parse(G.witness_stack+sizeof(G.witness_stack), (struct WitnessArgs_state*)G.witness_stack, &chunk, &WitnessArgs_rewrite_callbacks, MOL_NUM_MAX);

    if(rv == COMPLETE) {
        G.first_witness_done=1;
    }
  } else {
      blake2b_incremental_hash(buff, buff_size, &G.hash_state);
  }
}

void process_witness_end() {
    // If something went wrong parsing the first arg, just assume that it's the usual empty one.
    //
    //
    if(G.witness_idx==0 && G.first_witness_done!=1) {
        G.first_witness_done=1;
        explicit_bzero(&G.hash_state, sizeof(G.hash_state));
        blake2b_incremental_hash(G.transaction_hash, SIGN_HASH_SIZE, &G.hash_state);
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
    blake2b_finish_hash(G.final_hash, sizeof(G.final_hash), &G.hash_state);
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
            MolReader_AnnotatedTransaction_init_state(G.transaction_stack+sizeof(G.transaction_stack), (struct AnnotatedTransaction_state*)G.transaction_stack, &annotatedTransaction_callbacks);
            PRINTF("Initialized parser\n");
            // NO BREAK
        case P1_NEXT:
            if(G.maybe_transaction.hard_reject) THROW(EXC_PARSE_ERROR);
            PRINTF("Calling parser\n");
            rv = MolReader_AnnotatedTransaction_parse(G.transaction_stack+sizeof(G.transaction_stack), (struct AnnotatedTransaction_state*)G.transaction_stack, &chunk, &annotatedTransaction_callbacks, MOL_NUM_MAX);

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

size_t handle_apdu_sign(uint8_t instruction) {
    return handle_apdu(instruction);
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

    uint8_t const *const data = G.final_hash;        // on_hash ? G.final_hash : G.message_data;
    size_t const data_length = sizeof(G.final_hash); // on_hash ? sizeof(G.final_hash) : G.message_data_length;

    tx += WITH_KEY_PAIR(G.key, key_pair, size_t,
                        ({ sign(&G_io_apdu_buffer[tx], MAX_SIGNATURE_SIZE, key_pair, data, data_length); }));

    clear_data();
    return finalize_successful_send(tx);
}

/***********************************************************/
static inline void clear_message_data(void) {
  memset(&global.apdu.u.sign_msg, 0, sizeof(global.apdu.u.sign_msg));
}

static int perform_message_signature() {
  apdu_sign_message_state_t *g_sign_msg = &global.apdu.u.sign_msg;
  uint8_t *const buff = g_sign_msg -> final_hash;
  uint8_t const buff_size = sizeof(g_sign_msg -> final_hash);
  size_t final_size = 0;
  final_size+=WITH_KEY_PAIR(g_sign_msg->key, key_pair, size_t,
                      ({ sign(&G_io_apdu_buffer[0], MAX_SIGNATURE_SIZE, key_pair, buff, buff_size); }));
  clear_message_data();
  return finalize_successful_send(final_size);
}

static bool sign_message_ok(void) {
  delayed_send(perform_message_signature());
  return true; 
}

static bool check_magic_bytes(uint8_t const * message, uint8_t message_len) {
  // TODO: Make global and deduplicate
  const char nervos_magic[] = "Nervos Message:";
  /* if(message_len == 0 || message_len < (sizeof(nervos_magic)- 1)) return false; //If the message is shorter, it dont work */
  int cmp = memcmp(message, &nervos_magic, sizeof(nervos_magic)-1);
  return (0 == cmp); //cut off the str's nullbyte
}


/* typedef void (*string_generation_callback)(/1* char *buffer, size_t buffer_size, const void *data *1/); */
static void copy_buffer(char *const out, size_t const out_size, buffer_t const *const in) {
  if(in -> size > out_size) THROW(EXC_MEMORY_ERROR);

  // if we dont do this we have stuff from the old buffer getting displayed
  memset(out, 0, out_size);
  memcpy(out, in->bytes, in->size);
}

static void slice_magic_bytes(buffer_t *buff) {
  const char magic_bytes[] = "Nervos Message:";
  //remove string terminator when comparing
  size_t magic_bytes_size = sizeof(magic_bytes)- 1;
  if(0 == memcmp(buff->bytes, magic_bytes, magic_bytes_size)) {
    size_t num_bytes_to_copy = buff->size - magic_bytes_size;
    memmove(buff->bytes, &buff->bytes[magic_bytes_size], num_bytes_to_copy);
    buff -> size = num_bytes_to_copy;
    buff -> length = num_bytes_to_copy;
  }
  else THROW(EXC_PARSE_ERROR);

}
/***********************************************************************/
static size_t handle_apdu_sign_message_impl(uint8_t const _instruction) {
  uint8_t *const buff = &G_io_apdu_buffer[OFFSET_CDATA];
  uint8_t const buff_size = READ_UNALIGNED_BIG_ENDIAN(uint8_t, &G_io_apdu_buffer[OFFSET_LC]);
  /* uint8_t const p1 = READ_UNALIGNED_BIG_ENDIAN(uint8_t, &G_io_apdu_buffer[OFFSET_P1]); */

  //TODO: If message - magic-bytes is > than "VALUE_WIDTH", show hash instead
  apdu_sign_message_state_t *g_sign_msg = &global.apdu.u.sign_msg;

  if (buff_size > MAX_APDU_SIZE) THROW(EXC_WRONG_LENGTH_FOR_INS);

  // All messages MUST have the "Nervos Message:" prefix
  if(!check_magic_bytes(buff, buff_size)) THROW(EXC_PARSE_ERROR);

  // Use the root account for now
  uint32_t const account_index = 0x80000000 + 0x00;
  bip32_path_t root_path = {3, {0x8000002C, 0x80000135, account_index }};

  g_sign_msg -> key = root_path;

  //Hash data to sign and store in global
  blake2b_incremental_hash(buff, buff_size, &g_sign_msg->hash_state);
  blake2b_finish_hash(g_sign_msg -> final_hash, sizeof(g_sign_msg -> final_hash), &g_sign_msg -> hash_state);

  // Display the message
  static const char *const message_prompts[] = {PROMPT("Sign Message: "), NULL};
  g_sign_msg -> message_data_as_buffer.bytes = buff;
  g_sign_msg -> message_data_as_buffer.length = buff_size;
  g_sign_msg -> message_data_as_buffer.size = buff_size;
  //Remove magic bytes, because, even though we sign them, the user should not be aware of their existence
  slice_magic_bytes(&g_sign_msg -> message_data_as_buffer);
  register_ui_callback(0, copy_buffer, &g_sign_msg->message_data_as_buffer);
  ui_callback_t const ok_sign = sign_message_ok;

  // Prompt and sign hash
  ui_prompt(message_prompts, ok_sign, sign_reject);
}


size_t handle_apdu_sign_message(uint8_t instruction) {
  return handle_apdu_sign_message_impl(instruction);
}
