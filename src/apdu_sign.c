#include "apdu_sign.h"

#include "apdu.h"
#include "globals.h"
#include "key_macros.h"
#include "keys.h"
#include "memory.h"
#include "to_string.h"
#include "protocol.h"
#include "ui.h"
#include "cx.h"

#include <string.h>

#define G global.apdu.u.sign

static size_t perform_signature(void);

static inline void clear_data(void) {
    memset(&G, 0, sizeof(G));
}

static bool sign_ok(void) {
    delayed_send(perform_signature());
    return true;
}

static bool sign_reject(void) {
    clear_data();
    delay_reject();
    return true; // Return to idle
}

static size_t sign_complete(void) {
    static uint32_t const TYPE_INDEX = 0;
    static uint32_t const DRV_PATH_INDEX = 1;
    static uint32_t const HASH_INDEX = 2;
    static char const *const transaction_prompts[] = {
        PROMPT("Sign"),
        PROMPT("Derivation Path"),
        PROMPT("Hash"),
        NULL
    };
    REGISTER_STATIC_UI_VALUE(TYPE_INDEX, "Hash");

    register_ui_callback(DRV_PATH_INDEX, bip32_path_to_string, &G.bip32_path);

    G.final_hash_as_buffer.bytes = &G.final_hash[0];
    G.final_hash_as_buffer.length = sizeof(G.final_hash);
    G.final_hash_as_buffer.size = sizeof(G.final_hash);
    register_ui_callback(HASH_INDEX, buffer_to_hex, &G.final_hash_as_buffer);

    ui_prompt(transaction_prompts, sign_ok, sign_reject);
}

static size_t sign_hash_impl(uint8_t const *const in, uint8_t const in_size, bool const isLastMessage) {
    PRINTF("Entering %s\n", __FUNCTION__);
    if (!isLastMessage) {
        PRINTF("Expected single message\n");
        THROW(EXC_SECURITY);
    }
    if (in_size != sizeof(G.final_hash)) {
        PRINTF("Wrong length: Got %d but expected %d\n", in_size, sizeof(G.final_hash));
        THROW(EXC_WRONG_LENGTH);
    }
    memmove(G.final_hash, in, sizeof(G.final_hash));
    return sign_complete();
}

#define P1_NEXT       0x01
#define P1_LAST       0x80

size_t handle_apdu_sign_hash(void) {
    uint8_t const *const buff = &G_io_apdu_buffer[OFFSET_CDATA];
    uint8_t const buff_size = READ_UNALIGNED_BIG_ENDIAN(uint8_t, &G_io_apdu_buffer[OFFSET_LC]);
    if (buff_size > MAX_APDU_SIZE)
        THROW(EXC_WRONG_LENGTH_FOR_INS);
    uint8_t const p1 = READ_UNALIGNED_BIG_ENDIAN(uint8_t, &G_io_apdu_buffer[OFFSET_P1]);

    bool const isFirstMessage = (p1 & P1_NEXT) == 0;
    bool const isLastMessage = (p1 & P1_LAST) != 0;

    if (isFirstMessage) {
        clear_data();
        read_bip32_path(&G.bip32_path, buff, buff_size);
        return finalize_successful_send(0);
    }
    return sign_hash_impl(buff, buff_size, isLastMessage);
}

static size_t perform_signature(void) {
    size_t tx = 0;
    memcpy(&G_io_apdu_buffer[tx], G.final_hash, sizeof(G.final_hash));
    tx += sizeof(G.final_hash);

    tx += WITH_KEY_PAIR(G.bip32_path, key_pair, size_t, ({
        sign(&G_io_apdu_buffer[tx], MAX_SIGNATURE_SIZE, key_pair, G.final_hash, sizeof(G.final_hash));
    }));

    clear_data();
    return finalize_successful_send(tx);
}
