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
#define INTERMEDIATE_BUFF_SIZE (BLAKE2B_BLOCKBYTES + MAX_APDU_SIZE)

#define PRIVATE_KEY_DATA_SIZE 32

#define MAX_SIGNATURE_SIZE 100

struct priv_generate_key_pair {
    uint8_t private_key_data[PRIVATE_KEY_DATA_SIZE];
    key_pair_t res;
};

#define OUTPUT_FLAGS_KNOWN_LOCK     0x01
#define OUTPUT_FLAGS_IS_DAO         0x02
#define OUTPUT_FLAGS_IS_DAO_DEPOSIT 0x04

typedef struct {
    bip32_path_t bip32_path_prefix;
    uint8_t final_hash[SIGN_HASH_SIZE];
    buffer_t final_hash_as_buffer;
    uint8_t num_signatures_left;
} apdu_sign_state_t;

typedef struct {
    bip32_path_t bip32_path;
    extended_public_key_t ext_public_key;
} apdu_pubkey_state_t;

typedef enum {
    APP_MODE_DEFAULT = 0,
    APP_MODE_SIGNING_KNOWN_HASH = 1,
} app_mode_t;

typedef struct {
    void *stack_root;

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
        } u;

        struct {
            struct priv_generate_key_pair generate_key_pair;
        } priv;
    } apdu;

    uint8_t latest_apdu_instruction; // For detecting when a sequence of requests to the same APDU ends
    app_mode_t current_app_mode;
    nvram_data new_data;
} globals_t;

extern globals_t global;

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
