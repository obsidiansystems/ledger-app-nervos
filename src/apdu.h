#pragma once

#include "exception.h"
#include "types.h"
#include "ui.h"
#include "apdu_sign.h"

#include "os.h"

#include <stdbool.h>
#include <stdint.h>

#include "apdu_pubkey.h"

#if CX_APILEVEL < 8
#error "May only compile with API level 8 or higher; requires newer firmware"
#endif

#define OFFSET_CLA   0
#define OFFSET_INS   1 // instruction code
#define OFFSET_P1    2 // user-defined 1-byte parameter
#define OFFSET_LC    4 // length of CDATA
#define OFFSET_CDATA 5 // payload

__attribute__((noreturn)) void main_loop(apdu_handler const *const handlers, size_t const handlers_size);

static inline size_t finalize_successful_send(size_t tx) {
    G_io_apdu_buffer[tx++] = 0x90;
    G_io_apdu_buffer[tx++] = 0x00;
    return tx;
}

// Send back response; do not restart the event loop
static inline void delayed_send(size_t tx) {
    io_exchange(CHANNEL_APDU | IO_RETURN_AFTER_TX, tx);
}

static inline bool delay_reject(void) {
    size_t tx = 0;
    G_io_apdu_buffer[tx++] = EXC_REJECT >> 8;
    G_io_apdu_buffer[tx++] = EXC_REJECT & 0xFF;
    delayed_send(tx);
    return true;
}

static inline void require_hid(void) {
    if (G_io_apdu_media != IO_APDU_MEDIA_USB_HID) {
        THROW(EXC_HID_REQUIRED);
    }
}

size_t provide_pubkey(uint8_t *const io_buffer, cx_ecfp_public_key_t const *const pubkey);
size_t provide_ext_pubkey(uint8_t *const io_buffer, extended_public_key_t const *const pubkey);

size_t handle_apdu_version(void);
size_t handle_apdu_git(void);
size_t handle_apdu_get_wallet_id(void);
size_t handle_apdu_sign_hash(void);

size_t handle_apdu_error(void);
