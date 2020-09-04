#pragma once

#include "globals.h"
#include "keys.h"

// Yes you need this oddness if you want to use __LINE__
#define CONCAT_(a, b)  a##b
#define CONCAT(a, b)   CONCAT_(a, b)
#define MACROVAR(a, b) CONCAT(____##_##a##_##b, __LINE__)

#define WITH_EXTENDED_KEY_PAIR(bip32_path, vname, result_type, body)                                                   \
    ({                                                                                                                 \
        bip32_path_t const *const MACROVAR(vname, key) = &(bip32_path);                                                \
        extended_key_pair_t volatile MACROVAR(vname, generated_pair);                                                  \
        explicit_bzero(                                                                                                \
            (extended_key_pair_t *const)&MACROVAR(vname, generated_pair),                                              \
            sizeof(MACROVAR(vname, generated_pair)));                                                                  \
        result_type volatile MACROVAR(vname, retval);                                                                  \
        BEGIN_TRY {                                                                                                    \
            TRY {                                                                                                      \
                generate_extended_key_pair(                                                                            \
                    (extended_key_pair_t /*volatile*/ *const)&MACROVAR(vname, generated_pair),                         \
                    MACROVAR(vname, key));                                                                             \
                extended_key_pair_t const *const vname =                                                               \
                    (extended_key_pair_t /*volatile*/ const *const)&MACROVAR(vname, generated_pair);                   \
                MACROVAR(vname, retval) = body;                                                                        \
            }                                                                                                          \
            CATCH_OTHER(e) {                                                                                           \
                THROW(e);                                                                                              \
            }                                                                                                          \
            FINALLY {                                                                                                  \
                explicit_bzero(                                                                                        \
                    (extended_key_pair_t *const)&MACROVAR(vname, generated_pair),                                      \
                    sizeof(MACROVAR(vname, generated_pair)));                                                          \
            }                                                                                                          \
        }                                                                                                              \
        END_TRY;                                                                                                       \
        MACROVAR(vname, retval);                                                                                       \
    })

static inline void generate_extended_public_key(extended_public_key_t *const out, bip32_path_t const *const bip32_path) {
    check_null(out);
    check_null(bip32_path);
    WITH_EXTENDED_KEY_PAIR(*bip32_path, it, void*, ({
        memcpy(&out->public_key, &it->key_pair.public_key, sizeof(out->public_key));
        memcpy(&out->chain_code, &it->chain_code, sizeof(out->chain_code));
    }));
}

static inline void generate_public_key(cx_ecfp_public_key_t *const out, bip32_path_t const *const bip32_path) {
    check_null(out);
    check_null(bip32_path);
    WITH_EXTENDED_KEY_PAIR(*bip32_path, it, void*, ({
        memcpy(out, &it->key_pair.public_key, sizeof(*out));
    }));
}
