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

key_pair_t *generate_extended_key_pair_return_global(bip32_path_t const *const bip32_path, uint8_t *const chain_code /* optional */) {
    check_null(bip32_path);
    struct priv_generate_key_pair *const priv = &global.apdu.priv.generate_key_pair;

    cx_curve_t const cx_curve = CX_CURVE_SECP256K1;

    os_perso_derive_node_bip32(cx_curve, bip32_path->components, bip32_path->length, priv->private_key_data, chain_code);

    BEGIN_TRY {
        TRY {
            cx_ecfp_init_private_key(cx_curve, priv->private_key_data, sizeof(priv->private_key_data),
                                     &priv->res.private_key);
            cx_ecfp_generate_pair(cx_curve, &priv->res.public_key, &priv->res.private_key, 1);

            if (cx_curve == CX_CURVE_Ed25519) {
                cx_edward_compress_point(CX_CURVE_Ed25519, priv->res.public_key.W, priv->res.public_key.W_len);
                priv->res.public_key.W_len = 33;
            }
        }
        FINALLY {
            explicit_bzero(priv->private_key_data, sizeof(priv->private_key_data));
        }
    }
    END_TRY;

    return &priv->res;
}

key_pair_t *generate_key_pair_return_global(bip32_path_t const *const bip32_path) {
    key_pair_t *const result = generate_extended_key_pair_return_global(bip32_path, NULL);
    return result;
}

size_t sign(uint8_t *const out, size_t const out_size, key_pair_t const *const pair, uint8_t const *const in,
            size_t const in_size) {
    check_null(out);
    check_null(pair);
    check_null(in);

    static size_t const SIG_SIZE = 100;
    uint8_t sig[SIG_SIZE];

    if (out_size < 65)
        THROW(EXC_WRONG_LENGTH);
    unsigned int info;

    cx_ecdsa_sign(&pair->private_key, CX_LAST | CX_RND_RFC6979,
                  CX_SHA256, // historical reasons...semantically CX_NONE
                  (uint8_t const *)PIC(in), in_size, sig, SIG_SIZE, &info);

    // Converting to compressed format
    int r_size = sig[3];
    int r_size_sat = (r_size > 32) ? 32 : r_size;
    uint8_t *r = sig + 4 + (r_size > 32 ? 1 : 0);
    int s_size = sig[3 + r_size + 2];
    int s_size_sat = (s_size > 32) ? 32 : s_size;
    uint8_t *s = sig + 4 + r_size + 2 + (s_size > 32 ? 1 : 0);

    os_memmove(out + 32 - r_size_sat, r, r_size_sat);
    os_memmove(out + 64 - s_size_sat, s, s_size_sat);

    out[64] = 0;
    if (info & CX_ECCINFO_PARITY_ODD) {
        out[64] |= 0x01;
    }
    if (info & CX_ECCINFO_xGTn) {
        out[64] |= 0x02;
    }

    return 65;
}

void generate_pkh_for_pubkey(const cx_ecfp_public_key_t *const key, public_key_hash_t *const dest) {
    uint8_t temp_sha256_hash[CX_SHA256_SIZE];

    union {
      cx_sha256_t sha256;
      cx_ripemd160_t ripemd160;
    } hash_state;

    uint8_t tag_byte=(key->W[64]&1) ? 0x03 : 0x02;

    cx_sha256_init(&hash_state.sha256);
    cx_hash((cx_hash_t *)&hash_state.sha256, 0, &tag_byte, 1, NULL, 0);
    cx_hash((cx_hash_t *)&hash_state.sha256, CX_LAST, key->W+1, 32, temp_sha256_hash, CX_SHA256_SIZE);
    cx_ripemd160_init(&hash_state.ripemd160);
    cx_hash((cx_hash_t *)&hash_state.ripemd160, CX_LAST, temp_sha256_hash, CX_SHA256_SIZE, (uint8_t *)dest, sizeof(public_key_hash_t));
}
