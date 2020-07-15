#include "to_string.h"

#include "apdu.h"
#include "keys.h"
#include "globals.h"

#include <string.h>

#define NO_CONTRACT_STRING      "None"
#define NO_CONTRACT_NAME_STRING "Custom Delegate: please verify the address"

#define TEZOS_HASH_CHECKSUM_SIZE 4

void pkh_to_string(char *const buff, size_t const buff_size, uint8_t const hash[KEY_HASH_SIZE]);

// These functions output terminating null bytes, and return the ending offset.
static size_t frac_ckb_to_string(char *dest, uint64_t number);
/*
void public_key_hash_to_string(
    char *const buff,
    size_t const buff_size,
    public_key_hash_t const *const contract
) {
    // If hash_ptr exists, show it to us now. Otherwise, we unpack the
    // packed hash.
    if (contract->hash_ptr != NULL) {
        if (buff_size < HASH_SIZE_B58) THROW(EXC_WRONG_LENGTH);
        memcpy(buff, contract->hash_ptr, HASH_SIZE_B58);
    } else if (contract->originated == 0 && contract->signature_type == SIGNATURE_TYPE_UNSET) {
        if (buff_size < sizeof(NO_CONTRACT_STRING)) THROW(EXC_WRONG_LENGTH);
        strcpy(buff, NO_CONTRACT_STRING);
    } else {
        signature_type_t const signature_type =
            contract->originated != 0
                ? SIGNATURE_TYPE_UNSET
                : contract->signature_type;
        pkh_to_string(buff, buff_size, signature_type, contract->hash);
    }
}
*/


void compute_hash_checksum(uint8_t out[TEZOS_HASH_CHECKSUM_SIZE], void const *const data, size_t size) {
    uint8_t checksum[CX_SHA256_SIZE];
    cx_hash_sha256(data, size, checksum, sizeof(checksum));
    cx_hash_sha256(checksum, sizeof(checksum), checksum, sizeof(checksum));
    memcpy(out, checksum, TEZOS_HASH_CHECKSUM_SIZE);
}


#define STRCPY_OR_THROW(buff, size, x, exc)                                                                            \
    ({                                                                                                                 \
        if (size < sizeof(x))                                                                                          \
            THROW(exc);                                                                                                \
        strcpy(buff, x);                                                                                               \
    })

// These functions do not output terminating null bytes.

// This function fills digits, potentially with all leading zeroes, from the end of the buffer backwards
// This is intended to be used with a temporary buffer of length MAX_INT_DIGITS
// Returns offset of where it stopped filling in
static inline size_t convert_number(char dest[MAX_INT_DIGITS], uint64_t number, bool leading_zeroes) {
    check_null(dest);
    char *const end = dest + MAX_INT_DIGITS;
    for (char *ptr = end - 1; ptr >= dest; ptr--) {
        *ptr = '0' + number % 10;
        number /= 10;
        if (!leading_zeroes && number == 0) { // TODO: This is ugly
            return ptr - dest;
        }
    }
    return 0;
}

void number_to_string_indirect64(char *const dest, size_t const buff_size, uint64_t const *const number) {
    check_null(dest);
    check_null(number);
    if (buff_size < MAX_INT_DIGITS + 1)
        THROW(EXC_WRONG_LENGTH); // terminating null
    number_to_string(dest, *number);
}

void number_to_string_indirect32(char *const dest, size_t const buff_size, uint32_t const *const number) {
    check_null(dest);
    check_null(number);
    if (buff_size < MAX_INT_DIGITS + 1)
        THROW(EXC_WRONG_LENGTH); // terminating null
    number_to_string(dest, *number);
}

void frac_ckb_to_string_indirect(char *const dest, size_t const buff_size, uint64_t const *const number) {
    check_null(dest);
    check_null(number);
    if (buff_size < MAX_INT_DIGITS + 1)
        THROW(EXC_WRONG_LENGTH); // + terminating null + decimal point
    frac_ckb_to_string(dest, *number);
}

// (x, y) -> "x of y"
void frac_ckb_tuple_to_string_indirect(char *const dest, size_t const buff_size, uint64_tuple_t const *const tuple) {
    check_null(dest);
    check_null(tuple);
    char const of_str[] = " of ";
    if (buff_size < (2 * MAX_INT_DIGITS) + strlen(of_str) + 1)
        THROW(EXC_WRONG_LENGTH);
    size_t out_len = 0;
    out_len += frac_ckb_to_string(dest + out_len, tuple->fst);
    strcpy(dest + out_len, of_str);
    out_len += strlen(of_str);
    frac_ckb_to_string(dest + out_len, tuple->snd);
}


size_t number_to_string(char *const dest, uint64_t number) {
    check_null(dest);
    char tmp[MAX_INT_DIGITS];
    size_t off = convert_number(tmp, number, false);

    // Copy without leading 0s
    size_t length = sizeof(tmp) - off;
    memcpy(dest, tmp + off, length);
    dest[length] = '\0';
    return length;
}

// frac_ckb are in hundred-millionths
#define CKB_SCALE      100000000
#define DECIMAL_DIGITS 8

size_t frac_ckb_to_string(char *const dest, uint64_t number) {
    check_null(dest);
    uint64_t whole_tez = number / CKB_SCALE;
    uint64_t fractional = number % CKB_SCALE;
    size_t off = number_to_string(dest, whole_tez);
    if (fractional == 0) {
        return off;
    }
    dest[off++] = '.';

    char tmp[MAX_INT_DIGITS];
    convert_number(tmp, number, true);

    // Eliminate trailing 0s
    char *start = tmp + MAX_INT_DIGITS - DECIMAL_DIGITS;
    char *end;
    for (end = tmp + MAX_INT_DIGITS - 1; end >= start; end--) {
        if (*end != '0') {
            end++;
            break;
        }
    }

    size_t length = end - start;
    memcpy(dest + off, start, length);
    off += length;
    dest[off] = '\0';
    return off;
}

/* void lock_arg_to_string(char *const dest, size_t const buff_size, uint8_t const *const lockarg) { */
/*     bin_to_hex(dest, buff_size, lockarg, 20); */
/* } */

void copy_string(char *const dest, size_t const buff_size, char const *const src) {
    check_null(dest);
    check_null(src);
    char const *const src_in = (char const *)PIC(src);
    // I don't care that we will loop through the string twice, latency is not an issue
    if (strlen(src_in) >= buff_size)
        THROW(EXC_WRONG_LENGTH);
    strcpy(dest, src_in);
}

void bin_to_hex(char *const out, size_t const out_size, uint8_t const *const in, size_t const in_size) {
    check_null(out);
    check_null(in);

    size_t const out_len = in_size * 2;
    if (out_size < out_len + 1)
        THROW(EXC_MEMORY_ERROR);

    char const *const src = (char const *)PIC(in);
    for (size_t i = 0; i < in_size; i++) {
        out[i * 2] = "0123456789ABCDEF"[src[i] >> 4];
        out[i * 2 + 1] = "0123456789ABCDEF"[src[i] & 0x0F];
    }
    out[out_len] = '\0';
}

void buffer_to_hex(char *const out, size_t const out_size, buffer_t const *const in) {
    check_null(out);
    check_null(in);
    buffer_t const *const src = (buffer_t const *)PIC(in);
    bin_to_hex(out, out_size, src->bytes, src->length);
}

void lock_arg_to_sighash_address(char *const dest, size_t const buff_size, lock_arg_t const *const lock_arg) {
    global.apdu.priv.render_address_payload.s.address_format_type = ADDRESS_FORMAT_TYPE_SHORT;
    global.apdu.priv.render_address_payload.s.code_hash_index = ADDRESS_CODE_HASH_TYPE_SIGHASH;

    memcpy(&global.apdu.priv.render_address_payload.s.hash, lock_arg->hash, sizeof(global.apdu.priv.render_address_payload.s.hash));
    render_pkh(dest, buff_size, &global.apdu.priv.render_address_payload);
}

void lock_arg_to_multisig_address(char *const dest, size_t const buff_size, lock_arg_t const *const lock_arg) {
    bool has_timelock = false;
    for (int i = 0; i < 8; i++) {
        if (lock_arg->lock_period[i] != 0) {
            has_timelock = true;
            break;
        }
    }
    if (has_timelock) {
        global.apdu.priv.render_address_payload.f.address_format_type = ADDRESS_FORMAT_TYPE_FULL_TYPE;
        memcpy(&global.apdu.priv.render_address_payload.f.code_hash, multisigLockScript,
               sizeof(global.apdu.priv.render_address_payload.f.code_hash));
        memcpy(&global.apdu.priv.render_address_payload.f.lock_arg, lock_arg, sizeof(global.apdu.priv.render_address_payload.f.lock_arg));
    } else {
        global.apdu.priv.render_address_payload.s.address_format_type = ADDRESS_FORMAT_TYPE_SHORT;
        global.apdu.priv.render_address_payload.s.code_hash_index = ADDRESS_CODE_HASH_TYPE_MULTISIG;
        memcpy(&global.apdu.priv.render_address_payload.s.hash, lock_arg->hash,
               sizeof(global.apdu.priv.render_address_payload.s.hash));
    }

    render_pkh(dest, buff_size, &global.apdu.priv.render_address_payload);
}

// (x, h) -> "x of y"
void uint64_tuple_to_string(char *const dest, size_t const buff_size, uint64_tuple_t const *const tuple) {
    check_null(dest);
    check_null(tuple);
    size_t out_len = 0;
    out_len += number_to_string(dest + out_len, tuple->fst);
    char const of_str[] = " of ";
    strcpy(dest + out_len, of_str);
    out_len += strlen(of_str);
    number_to_string(dest + out_len, tuple->snd);
}
