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

size_t handle_apdu_get_wallet_id(uint8_t __attribute__((unused)) instruction) {
    // blake2b hash of "nervos-ledger-id"
    static const uint8_t _U_ token[] = {0xc1, 0x30, 0xae, 0x5b, 0xf2, 0xfb, 0x61, 0xe3, 0x9e, 0x41, 0x9d, 0xc5, 0x8a,
                                        0x45, 0x4f, 0x4a, 0xb4, 0xb6, 0xe4, 0xb6, 0xdb, 0x0b, 0x4b, 0x34, 0x60, 0xc3,
                                        0xed, 0x12, 0x8e, 0xd5, 0x5f, 0xd2, 0x3d, 0x0a, 0x37, 0xc3, 0x75, 0x0b, 0xb2,
                                        0xb4, 0xd8, 0x0a, 0x74, 0x11, 0xe3, 0x68, 0x3b, 0x91, 0x80, 0x62, 0xab, 0x98,
                                        0xfd, 0x4d, 0x2c, 0x6d, 0x05, 0x95, 0xbb, 0x03, 0x30, 0x81, 0x78, 0xb6};
    // uint32_t id_path[] = {0x8000002C, 0x80000135};
    bip32_path_t id_path = {2, {0x8000002C, 0x80000135}};

    int rv = 0;
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
                      rv = cx_hash((cx_hash_t *)&hashState, CX_LAST, (uint8_t *)key_pair->public_key.W,
                                   key_pair->public_key.W_len, G_io_apdu_buffer, sizeof(G_io_apdu_buffer));
                  }));
    return finalize_successful_send(rv);
}

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

                uint8_t const instruction = G_io_apdu_buffer[OFFSET_INS];

                apdu_handler const cb = instruction >= handlers_size ? handle_apdu_error : handlers[instruction];

                size_t const tx = cb(instruction);

                if(app_stack_canary != 0xdeadbeef) {
                    PRINTF("STACK ERROR: canary has value 0x%.*h rather than 0xdeadbeef\n", 4, &app_stack_canary);
                    THROW(EXC_STACK_ERROR);
                }

                rx = io_exchange(CHANNEL_APDU, tx);
            }
            CATCH(ASYNC_EXCEPTION) {
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
