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

#define _U_ __attribute__((unused))

// Return number of bytes to transmit (tx)
typedef size_t (*apdu_handler)(uint8_t instruction);

#define MAX_INT_DIGITS 20

typedef struct {
    size_t length;
    size_t size;
    uint8_t *bytes;
} buffer_t;

// UI
typedef bool (*ui_callback_t)(void); // return true to go back to idle screen

// Uses K&R style declaration to avoid being stuck on const void *, to avoid having to cast the
// function pointers.
typedef void (*string_generation_callback)(/* char *buffer, size_t buffer_size, const void *data */);

// Keys
typedef struct {
    cx_ecfp_public_key_t public_key;
    cx_ecfp_private_key_t private_key;
} key_pair_t;

#define MAX_BIP32_PATH 10

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
#define PROMPT_WIDTH     16
#define VALUE_WIDTH      64 // Needs to hold a 32 bytes of hash in hex.

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

// TODO: Rename to KEY_HASH_SIZE
#define KEY_HASH_SIZE 20

// HASH_SIZE encoded in base-58 ASCII
#define HASH_SIZE_B58 36

enum operation_tag {
    OPERATION_TAG_NONE = -1, // Sentinal value, as 0 is possibly used for something
    OPERATION_TAG_NOT_SET = 0, // Used for "value not yet determined" during parsing.
    OPERATION_TAG_PLAIN_TRANSFER = 1,
    OPERATION_TAG_DAO_DEPOSIT,
    OPERATION_TAG_DAO_PREPARE,
    OPERATION_TAG_DAO_WITHDRAW
};

typedef uint8_t standard_lock_arg_t[20];

typedef struct public_key_hash {
    uint8_t hash[KEY_HASH_SIZE];
    char *hash_ptr; // caching.
} public_key_hash_t;

typedef union {
    uint8_t entire[2 + sizeof(standard_lock_arg_t)];
    struct {
        uint8_t address_type_is_short;
        uint8_t key_hash_type_is_sighash;
        standard_lock_arg_t hash;
    };
} prefixed_public_key_hash_t;


#define HAS_DESTINATION_ADDRESS 0x01
#define HAS_CHANGE_ADDRESS      0x02

struct parsed_transaction {
    enum operation_tag tag;
    uint64_t total_fee;
    uint64_t amount; // 0 where inappropriate
    uint8_t flags;   // Interpretation depends on operation type
    uint32_t source_acct;
    uint8_t destination[20];
    uint64_t dao_amount;
    uint64_t dao_output_amount;
    uint8_t group_input_count;
};

// Maximum number of APDU instructions
#define INS_MAX 0x09

#define APDU_INS(x)                                                                                                    \
    ({                                                                                                                 \
        _Static_assert(x <= INS_MAX, "APDU instruction is out of bounds");                                             \
        x;                                                                                                             \
    })

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

typedef struct {
	bool initialized;
	address_type_t address_type;
	char network_prompt[10];
} nvram_data;
