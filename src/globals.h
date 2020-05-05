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
    bool initialized;
} blake2b_hash_state_t;

struct maybe_transaction {
    bool is_valid;
    bool unsafe;
    bool hard_reject;
    struct parsed_transaction v;
};

#define OUTPUT_FLAGS_KNOWN_LOCK     0x01
#define OUTPUT_FLAGS_IS_DAO         0x02
#define OUTPUT_FLAGS_IS_DAO_DEPOSIT 0x04

struct tx_output {
    uint64_t amount;
    uint8_t lock_arg[20];
    uint8_t flags;
};

typedef struct {
    uint8_t tx_hash[32];
    uint32_t index;
    blake2b_hash_state_t hash_state;
} input_state_t;

typedef struct {
    uint64_t capacity;
    bool active;
    bool is_dao;
    bool is_change;
    uint8_t dao_data_is_nonzero;
    uint32_t arg_chunk_ctr;
    uint32_t data_size;
    uint32_t lock_arg_index;
    uint8_t lock_arg_nonequal;
} cell_state_t;

typedef struct {
    bip32_path_t key;
    bip32_path_t temp_key;
    standard_lock_arg_t current_lock_arg;
    standard_lock_arg_t change_lock_arg;

    struct maybe_transaction maybe_transaction;

    blake2b_hash_state_t hash_state;

    uint32_t current_output_index;

    uint32_t input_count;

    input_state_t input_state;
    cell_state_t cell_state;

    uint32_t witness_idx;
    
    uint8_t transaction_hash[SIGN_HASH_SIZE];
    
    uint8_t final_hash[SIGN_HASH_SIZE];

    _Alignas(uint32_t) uint8_t witness_stack[64]; 
    _Alignas(uint32_t) uint8_t transaction_stack[512];
    
    uint64_t dao_bitmask;

    uint64_t total_inputs;
    uint64_t total_outputs;
    uint64_t dao_input_amount;
    uint64_t dao_output_amount;
    uint64_t plain_input_amount;
    uint64_t plain_output_amount;
    uint64_t change_amount;

    uint8_t *lock_arg_cmp;
    uint8_t lock_arg_tmp[20];
    buffer_t message_data_as_buffer;
    uint8_t packet_index; // 0-index is the initial setup packet, 1 is first packet to hash, etc.
    bool hash_only;
    bool first_witness_done;

} apdu_sign_state_t;

typedef struct {
    bip32_path_t key;
    cx_ecfp_public_key_t public_key;
    cx_blake2b_t hash_state;
} apdu_pubkey_state_t;

typedef struct {
    void *stack_root;
    apdu_handler handlers[INS_MAX + 1];

    struct {
        ui_callback_t ok_callback;
        ui_callback_t cxl_callback;

#ifndef TARGET_NANOX
        uint32_t ux_step;
        uint32_t ux_step_count;

        uint32_t timeout_cycle_count;
#endif

        struct {
            string_generation_callback callbacks[MAX_SCREEN_COUNT];
            const void *callback_data[MAX_SCREEN_COUNT];

#ifdef TARGET_NANOX
            struct {
                char prompt[PROMPT_WIDTH + 1];
                char value[VALUE_WIDTH + 1];
            } screen[MAX_SCREEN_COUNT];
#else
            char active_prompt[PROMPT_WIDTH + 1];
            char active_value[VALUE_WIDTH + 1];

            // This will and must always be static memory full of constants
            const char *const *prompts;
#endif
        } prompt;
    } ui;

    struct {
        union {
            apdu_pubkey_state_t pubkey;
            apdu_sign_state_t sign;
        } u;

        struct {
            struct priv_generate_key_pair generate_key_pair;
	    prefixed_public_key_hash_t prefixed_public_key_hash;
        } priv;
    } apdu;
    nvram_data new_data;
} globals_t;

extern globals_t global;

extern const uint8_t blake2b_personalization[17];

extern unsigned int app_stack_canary; // From SDK

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
