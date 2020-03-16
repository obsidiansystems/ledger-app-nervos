#include "apdu_pubkey.h"

#include "apdu.h"
#include "cx.h"
#include "globals.h"
#include "keys.h"
#include "protocol.h"
#include "to_string.h"
#include "ui.h"
#include "segwit_addr.h"
#ifdef BAKING_APP
#include "baking_auth.h"
#endif // BAKING_APP

#include <string.h>

#define G global.apdu.u.pubkey

static bool pubkey_ok(void) {
    delayed_send(provide_pubkey(G_io_apdu_buffer, &G.public_key));
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
        bound_check_buffer(out_current_offset, out_size);
        out[out_current_offset++] = '/';
        bound_check_buffer(out_current_offset, out_size);
        out[out_current_offset] = '\0';
    }
}

static void render_pkh(const char *const hrb, char *const out, size_t const out_size,
                       apdu_pubkey_state_t const *const pubkey) {
    const size_t base32_max = 256;
    uint8_t base32_buf[base32_max];
    size_t base32_len = 0;
    if (!convert_bits(base32_buf, base32_max, &base32_len, 5, pubkey->public_key_hash, SIGN_HASH_SIZE, 8, 1)) {
        THROW(EXC_MEMORY_ERROR);
    }
    if (!bech32_encode(out, out_size, hrb, base32_buf, base32_len)) {
        THROW(EXC_MEMORY_ERROR);
    }
}

static void render_pkh_mainnet(char *const out, size_t const out_size, apdu_pubkey_state_t const *const pubkey) {
    render_pkh("ckb", out, out_size, pubkey);
}

static void render_pkh_testnet(char *const out, size_t const out_size, apdu_pubkey_state_t const *const pubkey) {
    render_pkh("ckt", out, out_size, pubkey);
}

__attribute__((noreturn)) static void prompt_path(ui_callback_t ok_cb, ui_callback_t cxl_cb) {
    static size_t const TYPE_INDEX = 0;
    static size_t const DRV_PATH_INDEX = 1;
    static size_t const MAIN_NET_INDEX = 2;
    static size_t const TEST_NET_INDEX = 3;

    static const char *const pubkey_labels[] = {
        PROMPT("Get Public Key"), PROMPT("Derivation Path"), PROMPT("Mainnet Address"), PROMPT("Testnet Address"), NULL,
    };
    REGISTER_STATIC_UI_VALUE(TYPE_INDEX, "Public Key");
    register_ui_callback(DRV_PATH_INDEX, bip32_path_to_string, &G);
    register_ui_callback(MAIN_NET_INDEX, render_pkh_mainnet, &G);
    register_ui_callback(TEST_NET_INDEX, render_pkh_testnet, &G);
    ui_prompt(pubkey_labels, ok_cb, cxl_cb);
}

size_t handle_apdu_get_public_key(uint8_t _U_ instruction) {
    uint8_t *dataBuffer = G_io_apdu_buffer + OFFSET_CDATA;

    if (READ_UNALIGNED_BIG_ENDIAN(uint8_t, &G_io_apdu_buffer[OFFSET_P1]) != 0)
        THROW(EXC_WRONG_PARAM);

    size_t const cdata_size = READ_UNALIGNED_BIG_ENDIAN(uint8_t, &G_io_apdu_buffer[OFFSET_LC]);

    read_bip32_path(&G.key, dataBuffer, cdata_size);
    generate_public_key(&G.public_key, &G.key);

    cx_blake2b_init2(&G.hash_state, SIGN_HASH_SIZE * 8, NULL, 0, (uint8_t *)blake2b_personalization,
                     sizeof(blake2b_personalization) - 1);

    cx_hash((cx_hash_t *)&G.hash_state, 0, (uint8_t *const) & G.public_key, sizeof(G.public_key), NULL, 0);
    cx_hash((cx_hash_t *)&G.hash_state, CX_LAST, NULL, 0, (uint8_t *const) & G.public_key_hash,
            sizeof(G.public_key_hash));

    // instruction == INS_PROMPT_PUBLIC_KEY || instruction == INS_AUTHORIZE_BAKING
    // INS_PROMPT_PUBLIC_KEY
    ui_callback_t cb = pubkey_ok;
    prompt_path(cb, delay_reject);
}
