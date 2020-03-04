#pragma once

#include "types.h"

#include "bolos_target.h"

// Zeros out all globals that can keep track of APDU instruction state.
// Notably this does *not* include UI state.
void clear_apdu_globals(void);

// Zeros out all application-specific globals and SDK-specific UI/exchange buffers.
void init_globals(void);

#define MAX_APDU_SIZE 230  // Maximum number of bytes in a single APDU

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
	bool parse_failed;
	struct parsed_transaction v;
};

#define OUTPUT_FLAGS_KNOWN_LOCK 0x01
#define OUTPUT_FLAGS_IS_DAO 0x02

struct tx_output {
  uint64_t amount;
  uint8_t lock_arg[20];
  uint8_t flags;
};

struct tx_context {
    uint8_t hash[32];
    uint8_t num_outputs;
    struct tx_output outputs[3];
};

#define MAX_TOSIGN_PARSED 512

typedef struct {
    bip32_path_t key;

    uint8_t packet_index; // 0-index is the initial setup packet, 1 is first packet to hash, etc.

    uint8_t to_parse[MAX_TOSIGN_PARSED];
    uint16_t to_parse_fill_idx;

    struct tx_context context_transactions[5];
    uint8_t context_transactions_fill_idx;

    struct maybe_transaction maybe_transaction;

    //uint8_t message_data[NERVOS_BUFSIZE];
    //uint32_t message_data_length;
    buffer_t message_data_as_buffer;

    blake2b_hash_state_t hash_state;
    uint8_t final_hash[SIGN_HASH_SIZE];

    uint8_t magic_byte;
    bool hash_only;
} apdu_sign_state_t;

typedef struct {
  void *stack_root;
  apdu_handler handlers[INS_MAX + 1];

  struct {
    ui_callback_t ok_callback;
    ui_callback_t cxl_callback;

#   ifndef TARGET_NANOX
    uint32_t ux_step;
    uint32_t ux_step_count;

    uint32_t timeout_cycle_count;
#   endif

    struct {
      string_generation_callback callbacks[MAX_SCREEN_COUNT];
      const void *callback_data[MAX_SCREEN_COUNT];

#     ifdef TARGET_NANOX
      struct {
        char prompt[PROMPT_WIDTH + 1];
        char value[VALUE_WIDTH + 1];
      } screen[MAX_SCREEN_COUNT];
#     else
      char active_prompt[PROMPT_WIDTH + 1];
      char active_value[VALUE_WIDTH + 1];

      // This will and must always be static memory full of constants
      const char *const *prompts;
#     endif
    } prompt;
  } ui;

  struct {
      union {
          struct {
              bip32_path_t key;
              cx_ecfp_public_key_t public_key;
          } pubkey;

          apdu_sign_state_t sign;
      } u;

      struct {
          struct priv_generate_key_pair generate_key_pair;

          struct {
              cx_ecfp_public_key_t compressed;
          } public_key_hash;
      } priv;
    } apdu;
} globals_t;

extern globals_t global;

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

// Properly updates NVRAM data to prevent any clobbering of data.
// 'out_param' defines the name of a pointer to the nvram_data struct
// that 'body' can change to apply updates.
#define UPDATE_NVRAM(out_name, body) ({ \
    nvram_data *const out_name = &global.apdu.baking_auth.new_data; \
    memcpy(&global.apdu.baking_auth.new_data, (nvram_data const *const)&N_data, sizeof(global.apdu.baking_auth.new_data)); \
    body; \
    nvm_write((void*)&N_data, &global.apdu.baking_auth.new_data, sizeof(N_data)); \
    update_baking_idle_screens(); \
})
