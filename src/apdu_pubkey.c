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
#define GPriv global.apdu.priv

static bool pubkey_ok(void) {
    delayed_send(provide_pubkey(G_io_apdu_buffer, &G.ext_public_key.public_key));
    return true;
}

static bool ext_pubkey_ok(void) {
    delayed_send(provide_ext_pubkey(G_io_apdu_buffer, &G.ext_public_key));
    return true;
}

#define BIP32_HARDENED_PATH_BIT 0x80000000

static inline void bound_check_buffer(size_t counter, size_t size) {
    if (counter >= size) {
        THROW(EXC_MEMORY_ERROR);
    }
}

static void bip32_path_to_string(char *const out, size_t const out_size, apdu_pubkey_state_t const *const pubkey) {
    size_t out_current_offset = 0;
    for (int i = 0; i < MAX_BIP32_PATH && i < pubkey->key.length; i++) {
        bool is_hardened = pubkey->key.components[i] & BIP32_HARDENED_PATH_BIT;
        uint32_t component = pubkey->key.components[i] & ~BIP32_HARDENED_PATH_BIT;
        number_to_string_indirect32(out + out_current_offset, out_size - out_current_offset, &component);
        out_current_offset = strlen(out);
        if (is_hardened) {
            bound_check_buffer(out_current_offset, out_size);
            out[out_current_offset++] = '\'';
        }
        if (i < pubkey->key.length - 1) {
            bound_check_buffer(out_current_offset, out_size);
            out[out_current_offset++] = '/';
        }
        bound_check_buffer(out_current_offset, out_size);
        out[out_current_offset] = '\0';
    }
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
    register_ui_callback(ADDRESS_INDEX, lock_arg_to_sighash_address, &G.render_address_lock_arg);
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
    register_ui_callback(DRV_PATH_INDEX, bip32_path_to_string, &G);
    register_ui_callback(ADDRESS_INDEX, lock_arg_to_sighash_address, &G.render_address_lock_arg);
    ui_prompt(pubkey_labels, ext_pubkey_ok, delay_reject);
}

size_t handle_apdu_get_public_key(uint8_t _U_ instruction) {
    const uint8_t *const dataBuffer = G_io_apdu_buffer + OFFSET_CDATA;

    if (READ_UNALIGNED_BIG_ENDIAN(uint8_t, &G_io_apdu_buffer[OFFSET_P1]) != 0)
        THROW(EXC_WRONG_PARAM);

    size_t const cdata_size = READ_UNALIGNED_BIG_ENDIAN(uint8_t, &G_io_apdu_buffer[OFFSET_LC]);

    read_bip32_path(&G.key, dataBuffer, cdata_size);

    generate_extended_public_key(&G.ext_public_key, &G.key);

    generate_pkh_for_pubkey(&G.ext_public_key.public_key, &G.render_address_lock_arg);

    if (instruction == INS_PROMPT_EXT_PUBLIC_KEY) {
      prompt_ext_path();
    } else {
      prompt_path();
    }
}
