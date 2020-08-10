#include "apdu_pubkey.h"

#include "apdu.h"
#include "cx.h"
#include "globals.h"
#include "keys.h"
#include "protocol.h"
#include "to_string.h"
#include "ui.h"
#include "segwit_addr.h"

#include <string.h>

#define G global.apdu.u.pubkey

static bool pubkey_ok(void) {
    delayed_send(provide_pubkey(G_io_apdu_buffer, &G.ext_public_key.public_key));
    return true;
}

static bool ext_pubkey_ok(void) {
    delayed_send(provide_ext_pubkey(G_io_apdu_buffer, &G.ext_public_key));
    return true;
}

__attribute__((noreturn)) static void prompt_path() {
    static size_t const TYPE_INDEX = 0;
    static size_t const ADDRESS_INDEX = 1;

    static const char *const pubkey_labels[] = {
        PROMPT("Provide"),
        PROMPT("Address"),
        NULL,
    };
    REGISTER_STATIC_UI_VALUE(TYPE_INDEX, "Public Key");
    register_ui_callback(ADDRESS_INDEX, pkh_to_string, &G.pkh);
    ui_prompt(pubkey_labels, pubkey_ok, delay_reject);
}

__attribute__((noreturn)) static void prompt_ext_path() {
    static size_t const TYPE_INDEX = 0;
    static size_t const DRV_PATH_INDEX = 1;
    static size_t const ADDRESS_INDEX = 2;

    static const char *const pubkey_labels[] = {
        PROMPT("Provide"),
        PROMPT("Derivation Path"),
        PROMPT("Address"),
        NULL,
    };
    REGISTER_STATIC_UI_VALUE(TYPE_INDEX, "Extended Public Key");
    register_ui_callback(DRV_PATH_INDEX, bip32_path_to_string, &G.bip32_path);
    register_ui_callback(ADDRESS_INDEX, pkh_to_string, &G.pkh);
    ui_prompt(pubkey_labels, ext_pubkey_ok, delay_reject);
}

__attribute__((noreturn)) size_t handle_apdu_get_public_key_impl(bool const prompt_ext) {
    const uint8_t *const buff = G_io_apdu_buffer + OFFSET_CDATA;

    if (READ_UNALIGNED_BIG_ENDIAN(uint8_t, &G_io_apdu_buffer[OFFSET_P1]) != 0)
        THROW(EXC_WRONG_PARAM);

    size_t const cdata_size = READ_UNALIGNED_BIG_ENDIAN(uint8_t, &G_io_apdu_buffer[OFFSET_LC]);

    read_bip32_path(&G.bip32_path, buff, cdata_size);
    generate_extended_public_key(&G.ext_public_key, &G.bip32_path);
    generate_pkh_for_pubkey(&G.ext_public_key.public_key, &G.pkh);

    if (prompt_ext) {
        prompt_ext_path();
    } else {
        prompt_path();
    }
}

__attribute__((noreturn)) size_t handle_apdu_get_public_key() {
    handle_apdu_get_public_key_impl(false);
}

__attribute__((noreturn)) size_t handle_apdu_get_public_key_ext() {
    handle_apdu_get_public_key_impl(true);
}
