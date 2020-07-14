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

#define G global.apdu.u.pubkey
#define GPriv global.apdu.priv

static bool account_import_ok(void) {
    delayed_send(provide_ext_pubkey(G_io_apdu_buffer, &G.ext_public_key));
    return true;
}

__attribute__((noreturn)) static void prompt_account_import(ui_callback_t ok_cb, ui_callback_t cxl_cb) {
    static size_t const TYPE_INDEX = 0;

    static const char *const labels[] = {
        PROMPT("Import"),
        NULL,
    };
    REGISTER_STATIC_UI_VALUE(TYPE_INDEX, "Account");
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

    // m/44'/309'/<account_index>'
    bip32_path_t root_path = {3, {0x8000002C, 0x80000135, account_index}};
    G.key = root_path;
    generate_public_key(&G.ext_public_key, &G.key);

    ui_callback_t cb = account_import_ok;
    prompt_account_import(cb, delay_reject);
}
