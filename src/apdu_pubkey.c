#include "apdu_pubkey.h"

#include "apdu.h"
#include "cx.h"
#include "globals.h"
#include "keys.h"
#include "key_macros.h"
#include "protocol.h"
#include "to_string.h"
#include "ui.h"

#include <string.h>

#define G global.apdu.u.pubkey

static bool address_ok(void) {
    delayed_send(provide_address(G_io_apdu_buffer, &G.pkh));
    return true;
}

static bool ext_pubkey_ok(void) {
    delayed_send(provide_ext_pubkey(G_io_apdu_buffer, &G.ext_public_key));
    return true;
}

static void apdu_pubkey_state_to_string
   (char *out, size_t out_size,
    const apdu_pubkey_state_t *const payload) {
  pkh_to_string(out, out_size, payload->hrp, payload->hrp_len, &payload->pkh);
}

__attribute__((noreturn)) static void prompt_address() {
    static size_t const TYPE_INDEX = 0;
    static size_t const ADDRESS_INDEX = 1;
    static size_t const DRV_PATH_INDEX = 2;

    static const char *const pubkey_labels[] = {
        PROMPT("Provide"),
        PROMPT("Address"),
        PROMPT("Derivation Path"),
        NULL,
    };
    REGISTER_STATIC_UI_VALUE(TYPE_INDEX, "Public Key");
    register_ui_callback(ADDRESS_INDEX, apdu_pubkey_state_to_string, &G);
    register_ui_callback(DRV_PATH_INDEX, bip32_path_to_string, &G.bip32_path);
    ui_prompt(pubkey_labels, address_ok, delay_reject);
}

__attribute__((noreturn)) static void prompt_ext_pubkey() {
    static size_t const TYPE_INDEX = 0;
    static size_t const DRV_PATH_INDEX = 1;

    static const char *const pubkey_labels[] = {
        PROMPT("Provide"),
        PROMPT("Derivation Path"),
        NULL,
    };
    REGISTER_STATIC_UI_VALUE(TYPE_INDEX, "Extended Public Key");
    register_ui_callback(DRV_PATH_INDEX, bip32_path_to_string, &G.bip32_path);
    ui_prompt(pubkey_labels, ext_pubkey_ok, delay_reject);
}

__attribute__((noreturn)) size_t handle_apdu_get_public_key_impl(bool const prompt_ext) {
    const uint8_t *const buffer = G_io_apdu_buffer;

    const uint8_t p1 = buffer[OFFSET_P1];
    const uint8_t p2 = buffer[OFFSET_P2];
    const size_t cdata_size = buffer[OFFSET_LC];
    const uint8_t *const hrp = buffer + OFFSET_CDATA;

    if (p1 > ASCII_HRP_MAX_SIZE || p1 >= cdata_size) {
      THROW(EXC_WRONG_PARAM);
    }
    const uint8_t *const bip32_path = hrp + p1;

    if (p2 != 0) {
      THROW(EXC_WRONG_PARAM);
    }

    if (p1 == 0) {
      static const char default_hrp[] = "mainnet";
      G.hrp_len = sizeof(default_hrp) - 1;
      memcpy(G.hrp, default_hrp, G.hrp_len);
    } else {
      G.hrp_len = p1;
      memcpy(G.hrp, hrp, G.hrp_len);
    }

    read_bip32_path(&G.bip32_path, bip32_path, cdata_size);
    generate_extended_public_key(&G.ext_public_key, &G.bip32_path);
    generate_pkh_for_pubkey(&G.ext_public_key.public_key, &G.pkh);
    PRINTF("public key hash: %.*h\n", 20, G.pkh);

    if (prompt_ext) {
        prompt_ext_pubkey();
    } else {
        prompt_address();
    }
}


__attribute__((noreturn)) size_t handle_apdu_get_public_key() {
    handle_apdu_get_public_key_impl(false);
}

__attribute__((noreturn)) size_t handle_apdu_get_public_key_ext() {
    handle_apdu_get_public_key_impl(true);
}
