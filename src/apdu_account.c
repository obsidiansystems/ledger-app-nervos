#include "apdu_account.h"

#include "apdu.h"
#include "cx.h"
#include "globals.h"
#include "keys.h"
#include "protocol.h"
#include "to_string.h"
#include "ui.h"
#include "segwit_addr.h"

#include <string.h>

#define G global.apdu.u.account_import
#define GPriv global.apdu.priv

static bool account_import_ok(void) {
    delayed_send(provide_account_import(G_io_apdu_buffer, &G.root_public_key.public_key, &G.external_public_key, &G.change_public_key));
    return true;
}

static void account_id_to_string(char *const out, size_t const out_size, uint32_t const* const account_index) {
    uint32_t account_to_show_to_user = (*account_index) + 1;
    number_to_string_indirect32(out, out_size, &account_to_show_to_user);
}

__attribute__((noreturn)) static void prompt_account_import(ui_callback_t ok_cb, ui_callback_t cxl_cb, uint32_t account_index) {
    static size_t const TYPE_INDEX = 0;
    static size_t const ACCOUNT_INDEX = 1;

    static const char *const labels[] = {
        PROMPT("Import"),
        PROMPT("Account"),
        NULL,
    };
    REGISTER_STATIC_UI_VALUE(TYPE_INDEX, "Account");
    register_ui_callback(ACCOUNT_INDEX, account_id_to_string, &account_index);
    ui_prompt(labels, ok_cb, cxl_cb);
}

size_t handle_apdu_account_import(uint8_t _U_ instruction) {
    if (READ_UNALIGNED_BIG_ENDIAN(uint8_t, &G_io_apdu_buffer[OFFSET_P1]) != 0)
        THROW(EXC_WRONG_PARAM);

    size_t const cdata_size = READ_UNALIGNED_BIG_ENDIAN(uint8_t, &G_io_apdu_buffer[OFFSET_LC]);

    // Need the Account index (4 bytes)
    if (cdata_size != 4)
       THROW(EXC_WRONG_PARAM);

    uint32_t const account_index_raw = READ_UNALIGNED_BIG_ENDIAN(uint32_t, &G_io_apdu_buffer[OFFSET_CDATA]);

    if (account_index_raw >= 0x80000000)
      THROW(EXC_WRONG_PARAM);

    // Use hardened value
    uint32_t const account_index = 0x80000000 + account_index_raw;

    // Derive Extended Public key, but only public key will be provided back
    // m/44'/309'/<account_index>'
    bip32_path_t root_path = {3, {0x8000002C, 0x80000135, account_index}};
    G.path = root_path;
    generate_public_key(&G.root_public_key, &G.path);

    // Derive Extended Public key
    // m/44'/309'/<account_index>'/0
    bip32_path_t external_path = {4, {0x8000002C, 0x80000135, account_index, 0x00000000}};
    G.path = external_path;
    generate_public_key(&G.external_public_key, &G.path);

    // Derive Extended Public key
    // m/44'/309'/<account_index>'/1
    bip32_path_t change_path = {4, {0x8000002C, 0x80000135, account_index, 0x00000001}};
    G.path = change_path;
    generate_public_key(&G.change_public_key, &G.path);

    ui_callback_t cb = account_import_ok;
    prompt_account_import(cb, delay_reject, account_index_raw);
}
