/*******************************************************************************
 *   Ledger Blue
 *   (c) 2016 Ledger
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 ********************************************************************************/

#include "keys.h"

#include "apdu.h"
#include "globals.h"
#include "memory.h"
#include "protocol.h"
#include "types.h"

#include <stdbool.h>
#include <string.h>

#define PRIVATE_KEY_DATA_SIZE 32


size_t read_bip32_path(bip32_path_t *const out, uint8_t const *const in, size_t const in_size) {
    struct bip32_path_wire const *const buf_as_bip32 = (struct bip32_path_wire const *)in;

    if (in_size < sizeof(buf_as_bip32->length))
        THROW(EXC_WRONG_LENGTH_FOR_INS);

    size_t ix = 0;
    out->length = CONSUME_UNALIGNED_BIG_ENDIAN(ix, uint8_t, &buf_as_bip32->length);

    if (in_size - ix < out->length * sizeof(*buf_as_bip32->components))
        THROW(EXC_WRONG_LENGTH_FOR_INS);
    if (out->length == 0 || out->length > NUM_ELEMENTS(out->components))
        THROW(EXC_WRONG_VALUES);

    for (size_t i = 0; i < out->length; i++) {
        out->components[i] = CONSUME_UNALIGNED_BIG_ENDIAN(ix, uint32_t, &buf_as_bip32->components[i]);
    }

    return ix;
}

void concat_bip32_path(bip32_path_t *const out, bip32_path_t const *const in) {
    size_t const new_length = out->length + in->length;
    if (new_length > NUM_ELEMENTS(out->components))
        THROW(EXC_MEMORY_ERROR);
    for (size_t i = 0; i < in->length; i++) {
        out->components[out->length + i] = in->components[i];
    }
    out->length = new_length;
}

void generate_extended_key_pair(extended_key_pair_t *const out, bip32_path_t const *const bip32_path) {
    check_null(out);
    check_null(bip32_path);

    cx_curve_t const cx_curve = CX_CURVE_SECP256K1;

    unsigned char volatile private_key_data[PRIVATE_KEY_DATA_SIZE];
    explicit_bzero((unsigned char /*volatile*/*const)&private_key_data, sizeof(private_key_data));

    BEGIN_TRY {
        TRY {
            os_perso_derive_node_bip32(
                cx_curve,
                bip32_path->components, bip32_path->length,
                (unsigned char /*volatile*/*const)private_key_data,
                out->chain_code);

            cx_ecfp_init_private_key(
                cx_curve,
                (unsigned char /*volatile*/*const)private_key_data, sizeof(private_key_data),
                &out->key_pair.private_key);
            cx_ecfp_generate_pair(
                cx_curve,
                &out->key_pair.public_key,
                &out->key_pair.private_key,
                1);

            if (cx_curve == CX_CURVE_Ed25519) {
                cx_edward_compress_point(
                    CX_CURVE_Ed25519,
                    out->key_pair.public_key.W,
                    out->key_pair.public_key.W_len);
                out->key_pair.public_key.W_len = 33;
            }
        }
        CATCH_OTHER(e) {
            THROW(e);
        }
        FINALLY {
            explicit_bzero((unsigned char /*volatile*/*const)private_key_data, sizeof(private_key_data));
        }
    }
    END_TRY;
}

size_t sign(uint8_t *const out, size_t const out_size, key_pair_t const *const pair, uint8_t const *const in, size_t const in_size) {
    check_null(out);
    check_null(pair);
    check_null(in);

    static size_t const OUT_SIZE = 65;

    if (out_size < OUT_SIZE)
        THROW(EXC_WRONG_LENGTH);

    explicit_bzero(out, OUT_SIZE);

    static size_t const SIG_SIZE = 100;
    uint8_t sig[SIG_SIZE];
    explicit_bzero(sig, sizeof(sig));

    unsigned int info = 0;

    cx_ecdsa_sign(&pair->private_key, CX_LAST | CX_RND_RFC6979,
                  CX_SHA256, // historical reasons...semantically CX_NONE
                  (uint8_t const *const)PIC(in), in_size, sig, SIG_SIZE, &info);

    // Converting to compressed format
    int const r_size = sig[3];
    int const r_size_sat = (r_size > 32) ? 32 : r_size;
    uint8_t const *const r = sig + 4 + (r_size > 32 ? 1 : 0);
    int const s_size = sig[3 + r_size + 2];
    int const s_size_sat = (s_size > 32) ? 32 : s_size;
    uint8_t const *const s = sig + 4 + r_size + 2 + (s_size > 32 ? 1 : 0);

    os_memmove(out + 32 - r_size_sat, r, r_size_sat);
    os_memmove(out + 64 - s_size_sat, s, s_size_sat);

    out[64] = 0;
    if (info & CX_ECCINFO_PARITY_ODD) {
        out[64] |= 0x01;
    }
    if (info & CX_ECCINFO_xGTn) {
        out[64] |= 0x02;
    }

    return OUT_SIZE;
}

void generate_pkh_for_pubkey(const cx_ecfp_public_key_t *const key, public_key_hash_t *const dest) {
    uint8_t temp_sha256_hash[CX_SHA256_SIZE];

    union {
      cx_sha256_t sha256;
      cx_ripemd160_t ripemd160;
    } hash_state;

    // AVA uses bitcoin's Elliptic Curve point compression when generating addresses
    // Full uncompressed keys are encoded with the tag 0x04, followed by two 32-byte numbers,  X and Y
    // Given X, there's only two possibilities for Y, so the public key can encoded in 32 bytes + one bit.
    // Bitcoin tags the public key as follows:
    //     0x04  uncompressed public keys, (i.e. an point on the curve with the full X and Y coordinates)
    //     0x02  compressed public key, with LSB bit of Y = 0
    //     0x03  compressed public key, with LSB bit of Y = 1
    uint8_t tag_byte=(key->W[64]&1) ? 0x03 : 0x02;

    cx_sha256_init(&hash_state.sha256);
    cx_hash((cx_hash_t *)&hash_state.sha256, 0, &tag_byte, 1, NULL, 0);
    cx_hash((cx_hash_t *)&hash_state.sha256, CX_LAST, key->W+1, 32, temp_sha256_hash, CX_SHA256_SIZE);
    cx_ripemd160_init(&hash_state.ripemd160);
    cx_hash((cx_hash_t *)&hash_state.ripemd160, CX_LAST, temp_sha256_hash, CX_SHA256_SIZE, (uint8_t *)dest, sizeof(public_key_hash_t));
}
