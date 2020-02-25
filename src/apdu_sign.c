#include "apdu_sign.h"

#include "apdu.h"
#include "globals.h"
#include "key_macros.h"
#include "keys.h"
#include "memory.h"
#include "to_string.h"
#include "protocol.h"
#include "ui.h"

#include "cx.h"
#include "blockchain.h"

#include <string.h>

#define G global.apdu.u.sign

#define PARSE_ERROR() THROW(EXC_PARSE_ERROR)

#define B2B_BLOCKBYTES 128

static inline void conditional_init_hash_state(blake2b_hash_state_t *const state) {
    check_null(state);
    if (!state->initialized) {
        cx_blake2b_init(&state->state, SIGN_HASH_SIZE*8); // cx_blake2b_init takes size in bits.
        state->initialized = true;
    }
}

static void blake2b_incremental_hash(
    /*in/out*/ uint8_t *const out, size_t const out_size,
    /*in/out*/ size_t *const out_length,
    /*in/out*/ blake2b_hash_state_t *const state
) {
    check_null(out);
    check_null(out_length);
    check_null(state);

    uint8_t *current = out;
    while (*out_length > B2B_BLOCKBYTES) {
        if (current - out > (int)out_size) THROW(EXC_MEMORY_ERROR);
        conditional_init_hash_state(state);
        cx_hash((cx_hash_t *) &state->state, 0, current, B2B_BLOCKBYTES, NULL, 0);
        *out_length -= B2B_BLOCKBYTES;
        current += B2B_BLOCKBYTES;
    }
    // TODO use circular buffer at some point
    memmove(out, current, *out_length);
}

static void blake2b_finish_hash(
    /*out*/ uint8_t *const out, size_t const out_size,
    /*in/out*/ uint8_t *const buff, size_t const buff_size,
    /*in/out*/ size_t *const buff_length,
    /*in/out*/ blake2b_hash_state_t *const state
) {
    check_null(out);
    check_null(buff);
    check_null(buff_length);
    check_null(state);

    conditional_init_hash_state(state);
    blake2b_incremental_hash(buff, buff_size, buff_length, state);
    cx_hash((cx_hash_t *) &state->state, CX_LAST, buff, *buff_length, out, out_size);
}

static int perform_signature(bool const on_hash, bool const send_hash);

static inline void clear_data(void) {
    memset(&G, 0, sizeof(G));
}

static bool sign_without_hash_ok(void) {
    delayed_send(perform_signature(true, false));
    return true;
}

static bool sign_with_hash_ok(void) {
    delayed_send(perform_signature(true, true));
    return true;
}

static bool sign_reject(void) {
    clear_data();
    delay_reject();
    return true; // Return to idle
}

#define MAX_NUMBER_CHARS (MAX_INT_DIGITS + 2) // include decimal point and terminating null

static size_t sign_complete(uint8_t instruction) {
    static size_t const TYPE_INDEX = 0;
    static size_t const HASH_INDEX = 1;

    static const char *const parse_fail_prompts[] = {
        PROMPT("Unrecognized"),
        PROMPT("Sign Hash"),
        NULL,
    };

    REGISTER_STATIC_UI_VALUE(TYPE_INDEX, "Operation");

    ui_callback_t const ok_c = instruction == INS_SIGN_WITH_HASH ? sign_with_hash_ok : sign_without_hash_ok;

//unsafe:
    G.message_data_as_buffer.bytes = (uint8_t *)&G.final_hash;
    G.message_data_as_buffer.size = sizeof(G.final_hash);
    G.message_data_as_buffer.length = sizeof(G.final_hash);
    // Base58 encoding of 32-byte hash is 43 bytes long.
    register_ui_callback(HASH_INDEX, buffer_to_hex, &G.message_data_as_buffer);
    ui_prompt(parse_fail_prompts, ok_c, sign_reject);
}

void parse_operation(struct maybe_transaction* _U_ dest, bip32_path_t* _U_ key_derivation, uint8_t *const _U_ buff, uint8_t const _U_ buff_size) {

}

#define P1_FIRST 0x00
#define P1_NEXT 0x01
#define P1_HASH_ONLY_NEXT 0x03 // You only need it once
#define P1_LAST_MARKER 0x80

static size_t handle_apdu(bool const enable_hashing, bool const enable_parsing, uint8_t const instruction) {
    uint8_t *const buff = &G_io_apdu_buffer[OFFSET_CDATA];
    uint8_t const p1 = READ_UNALIGNED_BIG_ENDIAN(uint8_t, &G_io_apdu_buffer[OFFSET_P1]);
    uint8_t const buff_size = READ_UNALIGNED_BIG_ENDIAN(uint8_t, &G_io_apdu_buffer[OFFSET_LC]);
    if (buff_size > MAX_APDU_SIZE) THROW(EXC_WRONG_LENGTH_FOR_INS);

    bool last = (p1 & P1_LAST_MARKER) != 0;
    switch (p1 & ~P1_LAST_MARKER) {
    case P1_FIRST:
        clear_data();
        read_bip32_path(&G.key, buff, buff_size);
        return finalize_successful_send(0);

    case P1_NEXT:
        if (G.key.length == 0) THROW(EXC_WRONG_LENGTH_FOR_INS);

        // Guard against overflow
        if (G.packet_index >= 0xFF) PARSE_ERROR();
        G.packet_index++;

        break;
    default:
        THROW(EXC_WRONG_PARAM);
    }

    if (enable_parsing) {
	    if (G.packet_index == 1) {
	        G.maybe_transaction.is_valid = false;
		parse_operation(&G.maybe_transaction, &G.key, buff, buff_size);
	    }
    }

    if (enable_hashing) {
        // Hash contents of *previous* message (which may be empty).
        blake2b_incremental_hash(
            G.message_data, sizeof(G.message_data),
            &G.message_data_length,
            &G.hash_state);
    }

    if (G.message_data_length + buff_size > sizeof(G.message_data)) PARSE_ERROR();

    memmove(G.message_data + G.message_data_length, buff, buff_size);
    G.message_data_length += buff_size;

    if (last) {
        if (enable_hashing) {
            // Hash contents of *this* message and then get the final hash value.
            blake2b_incremental_hash(
                G.message_data, sizeof(G.message_data),
                &G.message_data_length,
                &G.hash_state);
            blake2b_finish_hash(
                G.final_hash, sizeof(G.final_hash),
                G.message_data, sizeof(G.message_data),
                &G.message_data_length,
                &G.hash_state);
        }

        return
		sign_complete(instruction);
    } else {
        return finalize_successful_send(0);
    }
}

size_t handle_apdu_sign(uint8_t instruction) {
    bool const enable_hashing = instruction != INS_SIGN_UNSAFE;
    bool const enable_parsing = enable_hashing;
    return handle_apdu(enable_hashing, enable_parsing, instruction);
}

size_t handle_apdu_sign_with_hash(uint8_t instruction) {
    bool const enable_hashing = true;
    bool const enable_parsing = true;
    return handle_apdu(enable_hashing, enable_parsing, instruction);
}

static int perform_signature(bool const on_hash, bool const send_hash) {
    if (on_hash && G.hash_only) {
        memcpy(G_io_apdu_buffer, G.final_hash, sizeof(G.final_hash));
        clear_data();
        return finalize_successful_send(sizeof(G.final_hash));
    }


    size_t tx = 0;
    if (send_hash && on_hash) {
        memcpy(&G_io_apdu_buffer[tx], G.final_hash, sizeof(G.final_hash));
        tx += sizeof(G.final_hash);
    }

    uint8_t const *const data = on_hash ? G.final_hash : G.message_data;
    size_t const data_length = on_hash ? sizeof(G.final_hash) : G.message_data_length;
    tx += WITH_KEY_PAIR(G.key, key_pair, size_t, ({
        sign(&G_io_apdu_buffer[tx], MAX_SIGNATURE_SIZE, key_pair, data, data_length);
    }));


    clear_data();
    return finalize_successful_send(tx);
}
