#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include "exception.h"
#include "memory.h"
#include "os_cx.h"
#include "types.h"

#if CX_APILEVEL <= 8
#error "CX_APILEVEL 8 and below is not supported"
#endif

struct bip32_path_wire {
    uint8_t length;
    uint32_t components[0];
} __attribute__((packed));

// throws
size_t read_bip32_path(bip32_path_t *const out, uint8_t const *const in, size_t const in_size);

// Non-reentrant
key_pair_t *generate_extended_key_pair_return_global(bip32_path_t const *const bip32_path, uint8_t *const chain_code);

// Non-reentrant
key_pair_t *generate_key_pair_return_global(bip32_path_t const *const bip32_path);

// Non-reentrant
static inline void generate_key_pair(key_pair_t *const out, bip32_path_t const *const bip32_path) {
    check_null(out);
    key_pair_t *const result = generate_key_pair_return_global(bip32_path);
    memcpy(out, result, sizeof(*out));
    explicit_bzero(result, sizeof(*result));
}

// Non-reentrant
static inline void generate_extended_public_key(extended_public_key_t *const out, bip32_path_t const *const bip32_path) {
    check_null(out);
    key_pair_t *const result = generate_extended_key_pair_return_global(bip32_path, out->chain_code);
    memcpy(&out->public_key, &result->public_key, sizeof(out->public_key));
    explicit_bzero(result, sizeof(*result));
}

static inline void generate_public_key(cx_ecfp_public_key_t *const out, bip32_path_t const *const bip32_path) {
    check_null(out);
    key_pair_t *const result = generate_extended_key_pair_return_global(bip32_path, NULL);
    memcpy(out, &result->public_key, sizeof(*out));
    explicit_bzero(result, sizeof(*result));
}

// Non-reentrant
cx_ecfp_public_key_t const *public_key_hash_return_global(uint8_t *const out, size_t const out_size,
                                                          cx_ecfp_public_key_t const *const restrict public_key);

// Non-reentrant
static inline void public_key_hash(uint8_t *const hash_out, size_t const hash_out_size,
                                   cx_ecfp_public_key_t *const pubkey_out, // pass NULL if this value is not desired
                                   cx_ecfp_public_key_t const *const restrict public_key) {
    cx_ecfp_public_key_t const *const pubkey = public_key_hash_return_global(hash_out, hash_out_size, public_key);
    if (pubkey_out != NULL) {
        memcpy(pubkey_out, pubkey, sizeof(*pubkey_out));
    }
}

size_t sign(uint8_t *const out, size_t const out_size, key_pair_t const *const key, uint8_t const *const in,
            size_t const in_size);

void generate_pkh_for_pubkey(const cx_ecfp_public_key_t *key, public_key_hash_t *dest);
