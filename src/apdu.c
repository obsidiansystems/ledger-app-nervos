#include "apdu.h"
#include "globals.h"
#include "to_string.h"
#include "version.h"
#include "key_macros.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

size_t provide_pubkey(uint8_t *const io_buffer, cx_ecfp_public_key_t const *const pubkey) {
    check_null(io_buffer);
    check_null(pubkey);
    size_t tx = 0;
    io_buffer[tx++] = pubkey->W_len;
    memmove(io_buffer + tx, pubkey->W, pubkey->W_len);
    tx += pubkey->W_len;
    return finalize_successful_send(tx);
}

size_t provide_ext_pubkey(uint8_t *const io_buffer, extended_public_key_t const *const ext_pubkey) {
    check_null(io_buffer);
    check_null(ext_pubkey);
    size_t tx = 0;
    size_t keySize = ext_pubkey->public_key.W_len;
    io_buffer[tx++] = keySize;
    memmove(io_buffer + tx, ext_pubkey->public_key.W, keySize);
    tx += keySize;
    io_buffer[tx++] = CHAIN_CODE_DATA_SIZE;
    memmove(io_buffer + tx, ext_pubkey->chain_code, CHAIN_CODE_DATA_SIZE);
    tx += CHAIN_CODE_DATA_SIZE;
    return finalize_successful_send(tx);
}

size_t handle_apdu_error(void) {
    THROW(EXC_INVALID_INS);
}

size_t handle_apdu_version(void) {
    memcpy(G_io_apdu_buffer, &version, sizeof(version_t));
    size_t tx = sizeof(version_t);
    return finalize_successful_send(tx);
}

size_t handle_apdu_git(void) {
    PRINTF("Handling apdu_git");
    static const char commit[] = COMMIT;
    memcpy(G_io_apdu_buffer, commit, sizeof(commit));
    size_t tx = sizeof(commit);
    return finalize_successful_send(tx);
}

size_t handle_apdu_get_wallet_id(void) {
    bip32_path_t id_path = {2, {ROOT_PATH_0, ROOT_PATH_1}};

    size_t rv = 0;
    cx_blake2b_t hashState;
    cx_blake2b_init(&hashState, 512);

    WITH_KEY_PAIR(id_path, key_pair, size_t, ({
        PRINTF("\nPublic Key: %.*h\n", key_pair->public_key.W_len, key_pair->public_key.W);
        // unsigned int _U_ info;
        // This isn't working properly deterministically, so stubbing it to unblock development.
        // cx_ecdsa_sign(&key, CX_LAST | CX_RND_RFC6979, CX_BLAKE2B, token, sizeof(token), signedToken,
        // 100, &info);
        // Stubbed until we have the sign step working.
        // rv = cx_hash((cx_hash_t*) &hashState, CX_LAST, signedToken, sizeof(signedToken),
        // G_io_apdu_buffer, sizeof(G_io_apdu_buffer));
        rv += cx_hash((cx_hash_t *)&hashState, CX_LAST, (uint8_t *)key_pair->public_key.W,
                      key_pair->public_key.W_len, G_io_apdu_buffer, sizeof(G_io_apdu_buffer));
    }));
    return finalize_successful_send(rv);
}

#ifdef STACK_MEASURE
__attribute__((noinline)) void stack_sentry_fill() {
  uint32_t* p;
  volatile int top;
  top=5;
  memset((void*)(&app_stack_canary+1), 42, ((uint8_t*)(&top-10))-((uint8_t*)&app_stack_canary));
}

void measure_stack_max() {
  uint32_t* p;
  volatile int top;
  for(p=&app_stack_canary+1; p<((&top)-10); p++)
    if(*p != 0x2a2a2a2a) {
        PRINTF("Free space between globals and maximum stack: %d\n", 4*(p-&app_stack_canary));
        return;
    }
}
#endif

#define CLA 0x80

__attribute__((noreturn)) void main_loop(apdu_handler const *const handlers, size_t const handlers_size) {
    volatile size_t rx = io_exchange(CHANNEL_APDU, 0);
    while (true) {
        BEGIN_TRY {
            TRY {
                app_stack_canary=0xdeadbeef;
                // Process APDU of size rx

                if (rx == 0) {
                    // no apdu received, well, reset the session, and reset the
                    // bootloader configuration
                    THROW(EXC_SECURITY);
                }

                if (G_io_apdu_buffer[OFFSET_CLA] != CLA) {
                    THROW(EXC_CLASS);
                }

                // The amount of bytes we get in our APDU must match what the APDU declares
                // its own content length is. All these values are unsigned, so this implies
                // that if rx < OFFSET_CDATA it also throws.
                if (rx != G_io_apdu_buffer[OFFSET_LC] + OFFSET_CDATA) {
                    THROW(EXC_WRONG_LENGTH);
                }

#ifdef STACK_MEASURE
                stack_sentry_fill();
#endif

                uint8_t const instruction = G_io_apdu_buffer[OFFSET_INS];

                PRINTF("Handling instruction %d when number of handlers is %d\n", instruction, handlers_size);
                apdu_handler const cb = instruction >= handlers_size
                    ? handle_apdu_error
                    : (apdu_handler)PIC(handlers[instruction]);

                PRINTF("Calling handler\n");
                size_t const tx = cb();
                PRINTF("Normal return\n");

                if(0xdeadbeef != app_stack_canary) {
                    THROW(EXC_STACK_ERROR);
                }
#ifdef STACK_MEASURE
                measure_stack_max();
#endif

                rx = io_exchange(CHANNEL_APDU, tx);
            }
            CATCH(ASYNC_EXCEPTION) {
#ifdef STACK_MEASURE
                measure_stack_max();
#endif
                rx = io_exchange(CHANNEL_APDU | IO_ASYNCH_REPLY, 0);
            }
            CATCH(EXCEPTION_IO_RESET) {
                THROW(EXCEPTION_IO_RESET);
            }
            CATCH_OTHER(e) {
                clear_apdu_globals(); // IMPORTANT: Application state must not persist through errors

                uint16_t sw = e;
                PRINTF("Error caught at top level, number: %x\n", sw);
                switch (sw) {
                default:
                    sw = 0x6800 | (e & 0x7FF);
                    // FALL THROUGH
                case 0x6000 ... 0x6FFF:
                case 0x9000 ... 0x9FFF: {
                    PRINTF("Line number: %d", sw & 0x0FFF);
                    size_t tx = 0;
                    G_io_apdu_buffer[tx++] = sw >> 8;
                    G_io_apdu_buffer[tx++] = sw;
                    rx = io_exchange(CHANNEL_APDU, tx);
                    break;
                }
                case 0xA000 ... 0xAFFF: {
                    PRINTF("Other error: %x\n", sw);
                    size_t tx = 0;
                    G_io_apdu_buffer[tx++] = sw >> 8;
                    G_io_apdu_buffer[tx++] = sw;
                    rx = io_exchange(CHANNEL_APDU, tx);
                    break;
                }
                }
            }
            FINALLY {}
        }
        END_TRY;
    }
}

// I have no idea what this function does, but it is called by the OS
unsigned short io_exchange_al(unsigned char channel, unsigned short tx_len) {
    switch (channel & ~(IO_FLAGS)) {
    case CHANNEL_KEYBOARD:
        break;

    // multiplexed io exchange over a SPI channel and TLV encapsulated protocol
    case CHANNEL_SPI:
        if (tx_len) {
            io_seproxyhal_spi_send(G_io_apdu_buffer, tx_len);

            if (channel & IO_RESET_AFTER_REPLIED) {
                reset();
            }
            return 0; // nothing received from the master so far (it's a tx
                      // transaction)
        } else {
            return io_seproxyhal_spi_recv(G_io_apdu_buffer, sizeof(G_io_apdu_buffer), 0);
        }

    default:
        THROW(INVALID_PARAMETER);
    }
    return 0;
}
