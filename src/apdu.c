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

size_t handle_apdu_error(uint8_t __attribute__((unused)) instruction) {
    THROW(EXC_INVALID_INS);
}

size_t handle_apdu_version(uint8_t __attribute__((unused)) instruction) {
    memcpy(G_io_apdu_buffer, &version, sizeof(version_t));
    size_t tx = sizeof(version_t);
    return finalize_successful_send(tx);
}

size_t handle_apdu_git(uint8_t __attribute__((unused)) instruction) {
    static const char commit[] = COMMIT;
    memcpy(G_io_apdu_buffer, commit, sizeof(commit));
    size_t tx = sizeof(commit);
    return finalize_successful_send(tx);
}

#define WALLET_ID_LENGTH 6

size_t handle_apdu_get_wallet_id(uint8_t __attribute__((unused)) instruction) {
    // We are hashing the full uncompressed public key of m/44'/9000' as the wallet id
    // The hash function is hmac-sha256 with a well-known key, in order to "personalize" the hash
    // function for this specific purpose.  We truncate the hmac to ensure that the public key
    // is not recoverable from the wallet id, even if sha256 is someday broken.
    uint8_t wallet_id[CX_SHA256_SIZE];

    bip32_path_t id_path = {2, {0x8000002C, 0x80002328}};
    const char hmac_key[] = "wallet-id";

    cx_hmac_sha256_t hmac_state;
    cx_hmac_sha256_init(&hmac_state, (unsigned char*)&hmac_key, sizeof(hmac_key)-1);


    WITH_KEY_PAIR(id_path, key_pair, size_t, ({
                      PRINTF("\nPublic Key: %.*h\n", key_pair->public_key.W_len, key_pair->public_key.W);
                      cx_hmac((cx_hmac_t *)&hmac_state, CX_LAST, (uint8_t *)key_pair->public_key.W,
                              key_pair->public_key.W_len, wallet_id, sizeof(wallet_id));
                  }));

    memcpy(G_io_apdu_buffer, wallet_id, WALLET_ID_LENGTH);

    return finalize_successful_send(WALLET_ID_LENGTH);
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

                apdu_handler const cb = instruction >= handlers_size ? handle_apdu_error : handlers[instruction];

                PRINTF("SIZOF1: %d SIZEOF2: %d\n", sizeof(G_ux), sizeof(G_ux_params));
                PRINTF("Calling handler\n");
                size_t const tx = cb(instruction);
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
