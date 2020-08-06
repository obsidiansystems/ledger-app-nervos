#include "to_string.h"

#include "apdu.h"
#include "keys.h"
#include "globals.h"
#include "base58.h"

#include <string.h>

#define NO_CONTRACT_STRING      "None"
#define NO_CONTRACT_NAME_STRING "Custom Delegate: please verify the address"

#define CB58_HASH_CHECKSUM_SIZE 4

static void compute_hash_checksum(uint8_t out[CB58_HASH_CHECKSUM_SIZE], void const *const data, size_t size) {
    uint8_t checksum[CX_SHA256_SIZE];
    cx_hash_sha256(data, size, checksum, sizeof(checksum));
    memcpy(out, checksum+CX_SHA256_SIZE-CB58_HASH_CHECKSUM_SIZE, CB58_HASH_CHECKSUM_SIZE);
}

void bip32_path_to_pkh_string(char *const out, size_t const out_size, bip32_path_t const *const bip32_path) {
    cx_ecfp_public_key_t pubkey;
    generate_public_key(&pubkey, bip32_path);
    public_key_hash_t pkh;
    generate_pkh_for_pubkey(&pubkey, &pkh);
    pkh_to_string(out, out_size, &pkh);
}

void pkh_to_string(char *out, size_t out_size,
                   const public_key_hash_t *const payload) {
  const size_t binary_cb58_size = sizeof(public_key_hash_t) + CB58_HASH_CHECKSUM_SIZE;
  uint8_t binary_cb58_buf[binary_cb58_size];
  memcpy(binary_cb58_buf, payload, sizeof(public_key_hash_t));
  compute_hash_checksum(binary_cb58_buf + sizeof(public_key_hash_t), payload, sizeof(public_key_hash_t));
  if (out_size < 2) {
    THROW(EXC_MEMORY_ERROR);
  }
  out[0] = 'X';
  out[1] = '-';
  out += 2;
  out_size -= 2;
  if (!b58enc(out, &out_size, binary_cb58_buf, binary_cb58_size)) {
    THROW(EXC_MEMORY_ERROR);
  }
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

#define DECIMAL_DIGITS 8

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
