#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include "os.h"
#include "cx.h"
#include "types.h"

#define READ_UNALIGNED_BIG_ENDIAN(type, in)                                                                            \
    ({                                                                                                                 \
        uint8_t const *bytes = (uint8_t const *)in;                                                                    \
        uint8_t out_bytes[sizeof(type)];                                                                               \
        type res;                                                                                                      \
                                                                                                                       \
        for (size_t i = 0; i < sizeof(type); i++) {                                                                    \
            out_bytes[i] = bytes[sizeof(type) - i - 1];                                                                \
        }                                                                                                              \
        memcpy(&res, out_bytes, sizeof(type));                                                                         \
                                                                                                                       \
        res;                                                                                                           \
    })

// Same as READ_UNALIGNED_BIG_ENDIAN but helps keep track of how many bytes
// have been read by adding sizeof(type) to the given counter.
#define CONSUME_UNALIGNED_BIG_ENDIAN(counter, type, addr)                                                              \
    ({                                                                                                                 \
        type consume_res = READ_UNALIGNED_BIG_ENDIAN(type, addr);                                                      \
        counter += sizeof(type);                                                                                       \
        consume_res;                                                                                                   \
    })
