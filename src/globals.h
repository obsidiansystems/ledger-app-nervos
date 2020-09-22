#pragma once

#include "types.h"
#include "bolos_target.h"

// Zeros out all globals that can keep track of APDU instruction state.
// Notably this does *not* include UI state.
void clear_apdu_globals(void);

// Zeros out all application-specific globals and SDK-specific UI/exchange buffers.
void init_globals(void);

#define MAX_APDU_SIZE 230 // Maximum number of bytes in a single APDU

// Our buffer must accommodate any remainder from hashing and the next message at once.
#define NERVOS_BUFSIZE (BLAKE2B_BLOCKBYTES + MAX_APDU_SIZE)

#define PRIVATE_KEY_DATA_SIZE 32

#define MAX_SIGNATURE_SIZE 100

struct priv_generate_key_pair {
    uint8_t private_key_data[PRIVATE_KEY_DATA_SIZE];
    key_pair_t res;
};

typedef struct {
    cx_blake2b_t state;
    uint8_t initialized;
} blake2b_hash_state_t;

struct maybe_transaction {
    uint8_t is_valid : 1;
    uint8_t unsafe : 1;
    uint8_t hard_reject : 1;
    struct parsed_transaction v;
};

#define OUTPUT_FLAGS_KNOWN_LOCK     0x01
#define OUTPUT_FLAGS_IS_DAO         0x02
#define OUTPUT_FLAGS_IS_DAO_DEPOSIT 0x04

typedef struct {
    uint32_t index;
    blake2b_hash_state_t hash_state;
} input_state_t;

typedef struct {
    uint64_t capacity;
    uint8_t dao_data_is_nonzero;
    uint8_t lock_arg_index : 5;
    uint8_t data_size : 4;
    uint8_t active : 1;
    uint8_t is_dao : 1;
    uint8_t is_change : 1;
    uint8_t is_multisig : 1;
    uint8_t lock_arg_nonequal : 1;
} cell_state_t;

typedef struct {
    union {
        bip32_path_t temp_key;
        struct {
            input_state_t input_state;
            standard_lock_arg_t last_input_lock_arg;
        } inp;

        // Things we need exclusively after doing finish_inputs
        struct {
            uint32_t witness_multisig_lock_arg_consumed;
            _Alignas(uint32_t) uint8_t witness_stack[40];
            uint32_t current_output_index;

            uint8_t transaction_hash[SIGN_HASH_SIZE];
            uint8_t final_hash[SIGN_HASH_SIZE];

            struct output_t outputs[MAX_OUTPUTS];

            uint64_t dao_bitmask;
            uint64_t change_amount;
            uint64_t plain_output_amount;
            uint64_t dao_output_amount;
            // threshold and pubkey_cnt are actually uint32_t
            // but here we save space as we expect them to be < 256
            uint8_t witness_multisig_threshold;
            uint8_t witness_multisig_pubkeys_cnt;
            // Counting just outputs which we deem "non-change" outputs because
            // they don't match the change bip32 path given as part of the
            // transaction annotations.
            uint8_t output_count;
            uint8_t is_first_witness : 1;
            uint8_t hash_only : 1;
            uint8_t first_witness_done : 1;
            uint8_t is_self_transfer : 1;
            uint8_t processed_change_cell : 1; // Has at least one change-address been processed?
            uint8_t sending_to_multisig_output : 1;
        } tx;
    } u;

    standard_lock_arg_t current_lock_arg;
    standard_lock_arg_t change_lock_arg;
    standard_lock_arg_t dao_cell_owner;

    struct maybe_transaction maybe_transaction;

    blake2b_hash_state_t hash_state;

    uint32_t input_count;
    uint32_t distinct_input_sources; // distinct input lock_args

    cell_state_t cell_state;

    _Alignas(uint32_t) uint8_t transaction_stack[240];
    // struct AnnotatedTransaction_state transaction_stack; - not just replacing because the "headers" are badly formed.

    uint64_t dao_input_amount;
    uint64_tuple_t input_amount;

    uint8_t *lock_arg_cmp;
    lock_arg_t lock_arg_tmp;

    uint32_t key_path_components[3];
    uint8_t key_length;

    uint8_t signing_multisig_input;
} apdu_sign_state_t;

typedef struct {
    buffer_t display_as_buffer;
    bip32_path_t key;
    blake2b_hash_state_t hash_state;
    uint8_t packet_index; // 0-index is the initial setup packet, 1 is first packet to hash, etc.
    uint8_t display_as_hex;
    uint8_t display[64];
    uint8_t final_hash[SIGN_HASH_SIZE];
} apdu_sign_message_state_t;

typedef struct {
    bip32_path_t key;
    buffer_t display_as_buffer;
    uint8_t hash_to_sign[64];  // Max message hash size we accept = 64 bytes
    uint8_t hash_to_sign_size;
} apdu_sign_message_hash_state_t;

typedef struct {
    bip32_path_t key;
    extended_public_key_t ext_public_key;
    cx_blake2b_t hash_state;
    standard_lock_arg_t render_address_lock_arg;
} apdu_pubkey_state_t;

typedef struct {
    void *stack_root;
    apdu_handler handlers[INS_MAX + 1];

    struct {
        ui_callback_t ok_callback;
        ui_callback_t cxl_callback;

        uint32_t ux_step;
        uint32_t ux_step_count;

        uint32_t timeout_cycle_count;
        void (*switch_screen)(uint32_t which);

        struct {
            string_generation_callback callbacks[MAX_SCREEN_COUNT];
            const void *callback_data[MAX_SCREEN_COUNT];

            char active_prompt[PROMPT_WIDTH + 1];
            char active_value[VALUE_WIDTH + 1];

            // This will and must always be static memory full of constants
            const char *const *prompts;
	    size_t offset;
        } prompt;
    } ui;

    struct {
        union {
            apdu_pubkey_state_t pubkey;
            apdu_sign_state_t sign;
            apdu_sign_message_state_t sign_msg;
            apdu_sign_message_hash_state_t sign_msg_hash;
        } u;

        struct {
            struct priv_generate_key_pair generate_key_pair;
        } priv;
    } apdu;
    nvram_data new_data;
} globals_t;

extern globals_t global;

extern const uint8_t defaultLockScript[];
extern const uint8_t multisigLockScript[];

extern const uint8_t blake2b_personalization[17];

extern unsigned int volatile app_stack_canary; // From SDK

// Used by macros that we don't control.
#ifdef TARGET_NANOX
extern ux_state_t G_ux;
extern bolos_ux_params_t G_ux_params;
#else
extern ux_state_t ux;
#endif
extern unsigned char G_io_seproxyhal_spi_buffer[IO_SEPROXYHAL_BUFFER_SIZE_B];

static inline void throw_stack_size() {
    uint8_t st;
    // uint32_t tmp1 = (uint32_t)&st - (uint32_t)&app_stack_canary;
    uint32_t tmp2 = (uint32_t)global.stack_root - (uint32_t)&st;
    THROW(0x9000 + tmp2);
}

void calculate_baking_idle_screens_data(void);
void update_baking_idle_screens(void);

#ifdef TARGET_NANOX
    extern nvram_data const N_data_real;
#   define N_data (*(volatile nvram_data *)PIC(&N_data_real))
#else
    extern nvram_data N_data_real;
#   define N_data (*(nvram_data*)PIC(&N_data_real))
#endif


// Properly updates NVRAM data to prevent any clobbering of data.
// 'out_param' defines the name of a pointer to the nvram_data struct
// that 'body' can change to apply updates.
#define UPDATE_NVRAM(out_name, body)                                                                                   \
    ({                                                                                                                 \
        nvram_data *const out_name = &global.new_data;                                                \
        memcpy(&global.new_data, (nvram_data const *const) & N_data,                                  \
               sizeof(global.new_data));                                                              \
        body;                                                                                                          \
        nvm_write((void *)&N_data, &global.new_data, sizeof(N_data));                                 \
    })

void switch_network();
void switch_sign_hash();

#ifdef NERVOS_DEBUG
// Aid for tracking down app crashes
#define STRINGIFY(x) #x
#define TOSTRING(x)  STRINGIFY(x)
#define AT           __FILE__ ":" TOSTRING(__LINE__)
inline void dbgout(char *at) {
    int i;
    PRINTF("%s - sp %p spg %p %d\n", at, &i, &app_stack_canary, app_stack_canary);
}
#define DBGOUT() dbgout(AT)
#endif
