#include "os.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "apdu_sign.h"
#include "globals.h"

unsigned int volatile app_stack_canary;

void *pic(void *linked_address) {
    return linked_address;
}

void os_longjmp(unsigned int exception) {
    longjmp(try_context_get()->jmp_buf, exception);
}

try_context_t *current_context = NULL;

try_context_t *try_context_get(void) {
    return current_context;
}

try_context_t *try_context_set(try_context_t *ctx) {
    try_context_t *previous_ctx = current_context;
    current_context = ctx;
    return previous_ctx;
}

uint8_t G_io_apdu_buffer[IO_APDU_BUFFER_SIZE];

void handle_apdu_sign_no_throw() {
    BEGIN_TRY {
        TRY {
            handle_apdu_sign(OPERATION_TAG_PLAIN_TRANSFER);
        }
        CATCH_ALL {
            clear_apdu_globals();
        }
        FINALLY {}
    }
    END_TRY;
}

#define P1_FIRST       0x00
#define P1_NEXT        0x01
#define P1_NO_FALLBACK 0x40
#define P1_LAST_MARKER 0x80
#define P1_MASK        (~(P1_LAST_MARKER | P1_NO_FALLBACK))

static void init_signing_transaction() {
    const uint8_t preamble[19] = {0x80, 0x05, 0x00, 0x00, 0x0E, 0x03, 0x03, 0x80, 0x00, 0x00,
                                  0x2C, 0x80, 0x00, 0x23, 0x28, 0x80, 0x00, 0x00, 0x00};
    memcpy(G_io_apdu_buffer, preamble, sizeof(preamble));
    handle_apdu_sign_no_throw();
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    size_t remaining_size = Size;

    memset(&global, 0, sizeof(global));
    init_signing_transaction();

    while (remaining_size > 0) {
        size_t chunk_size = remaining_size > MAX_APDU_SIZE ? MAX_APDU_SIZE : remaining_size;
        remaining_size -= chunk_size;

        G_io_apdu_buffer[OFFSET_CLA] = 0x80;
        G_io_apdu_buffer[OFFSET_INS] = INS_SIGN_MESSAGE;
        if (remaining_size == 0) {
            G_io_apdu_buffer[OFFSET_P1] = P1_LAST_MARKER;
        } else {
            G_io_apdu_buffer[OFFSET_P1] = P1_NEXT;
        }
        G_io_apdu_buffer[OFFSET_LC] = chunk_size;
        memcpy(G_io_apdu_buffer + OFFSET_CDATA, Data, chunk_size);
        Data += chunk_size;

        handle_apdu_sign_no_throw();
    }
    return 0;
}
