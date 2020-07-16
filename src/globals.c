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
#ifdef TARGET_NANOX
ux_state_t G_ux;
bolos_ux_params_t G_ux_params;
#else
ux_state_t ux;
#endif

unsigned char G_io_seproxyhal_spi_buffer[IO_SEPROXYHAL_BUFFER_SIZE_B];

void clear_apdu_globals(void) {
    memset(&global.apdu, 0, sizeof(global.apdu));
}

void init_globals(void) {
    memset(&global, 0, sizeof(global));

#ifdef TARGET_NANOX
    memset(&G_ux, 0, sizeof(G_ux));
    memset(&G_ux_params, 0, sizeof(G_ux_params));
#else
    memset(&ux, 0, sizeof(ux));
#endif

    memset(G_io_seproxyhal_spi_buffer, 0, sizeof(G_io_seproxyhal_spi_buffer));
}

// DO NOT TRY TO INIT THIS. This can only be written via an system call.
// The "N_" is *significant*. It tells the linker to put this in NVRAM.
#ifdef TARGET_NANOX
nvram_data const N_data_real;
#else
nvram_data N_data_real;
#endif

void switch_network() {
    static const nvram_data data[]={
      {
        true,
        ADDRESS_TESTNET,
        "testnet"
      }, {
        true,
        ADDRESS_MAINNET,
        "mainnet"
      }};
    nvm_write((void*)&N_data, (void*)&data[N_data.address_type&1], sizeof(N_data));
}

#ifndef TARGET_NANOX
_Static_assert(sizeof global <= 2120, "Size of globals_t exceeds the tested working limit");
#endif
