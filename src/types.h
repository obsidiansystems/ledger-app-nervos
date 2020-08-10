#pragma once

#include "exception.h"
#include "os.h"
#include "os_io_seproxyhal.h"

#include <stdbool.h>
#include <string.h>

// Type-safe versions of true/false
#undef true
#define true ((bool)1)
#undef false
#define false ((bool)0)

// Return number of bytes to transmit (tx)
typedef size_t (*apdu_handler)(void);

#define MAX_INT_DIGITS 20

#define ROOT_PATH_0 0x8000002C
#define ROOT_PATH_1 0x80002328

typedef struct {
    size_t length;
    size_t size;
    uint8_t *bytes;
} buffer_t;

typedef struct {
    uint64_t fst;
    uint64_t snd;
} uint64_tuple_t;

typedef struct {
    uint32_t fst;
    uint32_t snd;
} uint32_tuple_t;

// UI
typedef bool (*ui_callback_t)(void); // return true to go back to idle screen

// Uses K&R style declaration to avoid being stuck on const void *, to avoid having to cast the
// function pointers.
typedef void (*string_generation_callback)(/* char *out, size_t out_size, void const *const in */);

// Keys
typedef struct {
    cx_ecfp_public_key_t public_key;
    cx_ecfp_private_key_t private_key;
} key_pair_t;

#define CHAIN_CODE_DATA_SIZE 32

typedef struct {
    cx_ecfp_public_key_t public_key;
    uint8_t chain_code[CHAIN_CODE_DATA_SIZE];
} extended_public_key_t;

typedef struct {
    key_pair_t key_pair;
    uint8_t chain_code[CHAIN_CODE_DATA_SIZE];
} extended_key_pair_t;

#define MAX_BIP32_PATH 5

typedef struct {
    uint8_t length;
    uint32_t components[MAX_BIP32_PATH];
} bip32_path_t;

static inline void copy_bip32_path(bip32_path_t *const out, bip32_path_t volatile const *const in) {
    check_null(out);
    check_null(in);
    memcpy(out->components, (void *)in->components, in->length * sizeof(*in->components));
    out->length = in->length;
}

static inline bool bip32_paths_eq(bip32_path_t volatile const *const a, bip32_path_t volatile const *const b) {
    return a == b ||
           (a != NULL && b != NULL && a->length == b->length &&
            memcmp((void const *)a->components, (void const *)b->components, a->length * sizeof(*a->components)) == 0);
}

#define SIGN_HASH_SIZE 32 // TODO: Rename or use a different constant.

#define PKH_STRING_SIZE                  40 // includes null byte // TODO: use sizeof for this.
#define PROTOCOL_HASH_BASE58_STRING_SIZE sizeof("ProtoBetaBetaBetaBetaBetaBetaBetaBetaBet11111a5ug96")

#define MAX_SCREEN_COUNT 7 // Current maximum usage
#define PROMPT_WIDTH     17
#define VALUE_WIDTH      128 // Needs to hold a 32 bytes of hash in hex.

// Macros to wrap a static prompt and value strings and ensure they aren't too long.
#define PROMPT(str)                                                                                                    \
    ({                                                                                                                 \
        _Static_assert(sizeof(str) <= PROMPT_WIDTH + 1 /*null byte*/, str " won't fit in the UI prompt.");             \
        str;                                                                                                           \
    })

#define STATIC_UI_VALUE(str)                                                                                           \
    ({                                                                                                                 \
        _Static_assert(sizeof(str) <= VALUE_WIDTH + 1 /*null byte*/, str " won't fit in the UI.");                     \
        str;                                                                                                           \
    })


// Operations
#define PROTOCOL_HASH_SIZE 32

#define KEY_HASH_SIZE 20

// HASH_SIZE encoded in base-58 ASCII
#define HASH_SIZE_B58 36

enum operation_tag {
    OPERATION_TAG_NONE = -1, // Sentinal value, as 0 is possibly used for something
    OPERATION_TAG_NOT_SET = 0, // Used for "value not yet determined" during parsing.
    OPERATION_TAG_PLAIN_TRANSFER = 1,
    OPERATION_TAG_MULTI_OUTPUT_TRANSFER,
    OPERATION_TAG_SELF_TRANSFER,
    OPERATION_TAG_MULTI_INPUT_TRANSFER,
    OPERATION_TAG_DAO_DEPOSIT,
    OPERATION_TAG_DAO_PREPARE,
    OPERATION_TAG_DAO_WITHDRAW
};

typedef uint8_t public_key_hash_t[CX_RIPEMD160_SIZE];

#define ADDRESS_FORMAT_TYPE_SHORT 0x01
#define ADDRESS_FORMAT_TYPE_FULL_DATA 0x02
#define ADDRESS_FORMAT_TYPE_FULL_TYPE 0x04

#define ADDRESS_CODE_HASH_TYPE_SIGHASH 0x00
#define ADDRESS_CODE_HASH_TYPE_MULTISIG 0x01

#define HAS_DESTINATION_ADDRESS 0x01
#define HAS_CHANGE_ADDRESS      0x02

#define MAX_OUTPUTS 3

#define STRCPY(buff, x)                                                                                                \
    ({                                                                                                                 \
        _Static_assert(sizeof(buff) >= sizeof(x) && sizeof(*x) == sizeof(char), "String won't fit in buffer");         \
        strcpy(buff, x);                                                                                               \
    })

#undef MAX
#define MAX(a, b)                                                                                                      \
    ({                                                                                                                 \
        __typeof__(a) ____a_ = (a);                                                                                    \
        __typeof__(b) ____b_ = (b);                                                                                    \
        ____a_ > ____b_ ? ____a_ : ____b_;                                                                             \
    })

#undef MIN
#define MIN(a, b)                                                                                                      \
    ({                                                                                                                 \
        __typeof__(a) ____a_ = (a);                                                                                    \
        __typeof__(b) ____b_ = (b);                                                                                    \
        ____a_ < ____b_ ? ____a_ : ____b_;                                                                             \
    })

#define ADDRESS_TYPE_MASK 1
typedef enum {
	ADDRESS_MAINNET=0,
	ADDRESS_TESTNET
} address_type_t;

typedef enum {
  SIGN_HASH_OFF=0,
  SIGN_HASH_ON
} sign_hash_type_t;

typedef struct {
	bool initialized;
	address_type_t address_type;
	char network_prompt[10];
  sign_hash_type_t sign_hash_type;
	char sign_hash_prompt[10];
} nvram_data;
