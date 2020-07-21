#include "globals.h"

#include "exception.h"
#include "to_string.h"

#ifdef TARGET_NANOX
#include "ux.h"
#endif

#include <string.h>


// WARNING: ***************************************************
// Non-const globals MUST NOT HAVE AN INITIALIZER.
//
// Providing an initializer will cause the application to crash
// if you write to it.
// ************************************************************


globals_t global;

const uint8_t blake2b_personalization[] = "ckb-default-hash";

// These are strange variables that the SDK relies on us to define but uses directly itself.
ux_state_t G_ux;
bolos_ux_params_t G_ux_params;

unsigned char G_io_seproxyhal_spi_buffer[IO_SEPROXYHAL_BUFFER_SIZE_B];

void clear_apdu_globals(void) {
    memset(&global.apdu, 0, sizeof(global.apdu));
}

void init_globals(void) {
    memset(&global, 0, sizeof(global));

    memset(&G_ux, 0, sizeof(G_ux));
    memset(&G_ux_params, 0, sizeof(G_ux_params));

    memset(G_io_seproxyhal_spi_buffer, 0, sizeof(G_io_seproxyhal_spi_buffer));
}

// DO NOT TRY TO INIT THIS. This can only be written via an system call.
// The "N_" is *significant*. It tells the linker to put this in NVRAM.
#ifdef TARGET_NANOX
nvram_data const N_data_real;
#else
nvram_data N_data_real;
#endif

static const char sign_hash_prompt_on[] = "On";
static const char sign_hash_prompt_off[] = "Off";
static const char mainnet_prompt[] = "mainnet";
static const char testnet_prompt[] = "testnet";

void switch_network() {
    nvram_data data;
    memcpy(&data, &N_data, sizeof(nvram_data));
    const bool isMain = data.address_type == ADDRESS_MAINNET; 
    data.address_type = isMain ? ADDRESS_TESTNET : ADDRESS_MAINNET;
    if(isMain)
      strcpy(data.network_prompt, testnet_prompt);
    else
      strcpy(data.network_prompt, mainnet_prompt);

    nvm_write((void*)&N_data, (void*)&data, sizeof(N_data));
}
void switch_sign_hash() {
    nvram_data data;
    memcpy(&data, &N_data, sizeof(nvram_data));
    const bool isOn = data.sign_hash_type == SIGN_HASH_ON; 
    data.sign_hash_type = isOn ? SIGN_HASH_OFF : SIGN_HASH_ON;
    if(isOn)
      strcpy(data.sign_hash_prompt, sign_hash_prompt_off);
    else
      strcpy(data.sign_hash_prompt, sign_hash_prompt_on);
    nvm_write((void*)&N_data, (void*)&data, sizeof(N_data));
}

#ifndef TARGET_NANOX
_Static_assert(sizeof global <= 2120, "Size of globals_t exceeds the tested working limit");
#endif
