#include "cx.h"
#include "ux.h"
#include <stdlib.h>

#ifdef HAVE_SHA256
cx_err_t cx_sha256_init_no_throw(cx_sha256_t *hash) {
    return CX_OK;
}

size_t cx_hash_sha256(const uint8_t *in, size_t in_len, uint8_t *out, size_t out_len) {
    return CX_SHA256_SIZE;
}
#endif

size_t cx_hash_get_size(const cx_hash_t *ctx) {
    return 32;
}

#ifdef HAVE_RIPEMD
cx_err_t cx_ripemd160_init_no_throw(cx_ripemd160_t *hash) {
    return CX_OK;
}
#endif

#ifdef HAVE_SHA3
cx_err_t cx_keccak_init_no_throw(cx_sha3_t *hash, size_t size) {
    return CX_OK;
}
#endif

#ifdef HAVE_BLAKE2
cx_err_t cx_blake2b_init2_no_throw(cx_blake2b_t *hash, size_t size, uint8_t *salt, size_t salt_len, uint8_t *perso,
                                   size_t perso_len) {
    return CX_OK;
}

cx_err_t cx_blake2b_init_no_throw(cx_blake2b_t *hash, size_t size) {
    return cx_blake2b_init2_no_throw(hash, size, NULL, 0, NULL, 0);
}
#endif

cx_err_t cx_hash_no_throw(cx_hash_t *hash, uint32_t mode, const uint8_t *in, size_t len, uint8_t *out, size_t out_len) {
    return CX_OK;
}

cx_err_t cx_ecfp_generate_pair_no_throw(cx_curve_t curve, cx_ecfp_public_key_t *pubkey, cx_ecfp_private_key_t *privkey,
                                        bool keepprivate) {
    return CX_OK;
}

cx_err_t cx_ecfp_init_private_key_no_throw(cx_curve_t curve, const uint8_t *rawkey, size_t key_len,
                                           cx_ecfp_private_key_t *pvkey) {
    return CX_OK;
}

cx_err_t cx_edwards_compress_point_no_throw(cx_curve_t curve, uint8_t *p, size_t p_len) {
    return CX_OK;
}

cx_err_t cx_ecdsa_sign_no_throw(const cx_ecfp_private_key_t *pvkey, uint32_t mode, cx_md_t hashID, const uint8_t *hash,
                                size_t hash_len, uint8_t *sig, size_t *sig_len, uint32_t *info) {
    return CX_OK;
}

unsigned short io_exchange(unsigned char channel_and_flags, unsigned short tx_len) {
    return 0;
}

cx_err_t cx_math_mult_no_throw(uint8_t *r, const uint8_t *a, const uint8_t *b, size_t len) {
    return CX_OK;
}

void os_perso_derive_node_bip32(cx_curve_t curve, const unsigned int *path, unsigned int pathLength,
                                unsigned char *privateKey, unsigned char *chain) {}

void io_seproxyhal_init_ux() {}

void io_seproxyhal_display_default(const bagl_element_t *element) {}

bolos_bool_t os_global_pin_is_validated(void) {
    return BOLOS_TRUE;
}

void os_sched_exit(bolos_task_status_t exit_code) {
    exit(exit_code);
}

void nvm_write(void *dst_adr, void *src_adr, unsigned int src_len) {}

unsigned int io_seph_is_status_sent(void) {
    return 1;
}

bolos_task_status_t os_sched_last_status(unsigned int task_idx) {
    return 0;
}

bolos_bool_t os_perso_isonboarded(void) {
    return BOLOS_TRUE;
}

void io_seph_send(const unsigned char *buffer, unsigned short length) {}

unsigned short io_seph_recv(unsigned char *buffer, unsigned short maxlength, unsigned int flags) {
    return 0;
}

unsigned int os_ux_blocking(bolos_ux_params_t *params) {
    return BOLOS_UX_OK;
}

void halt(void) {
    exit(0);
}
