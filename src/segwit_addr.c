/* Copyright (c) 2017 Pieter Wuille
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "segwit_addr.h"

uint32_t bech32_polymod_step(uint32_t pre) {
    uint8_t b = pre >> 25;
    return ((pre & 0x1FFFFFF) << 5) ^ (-((b >> 0) & 1) & 0x3b6a57b2UL) ^ (-((b >> 1) & 1) & 0x26508e6dUL) ^
           (-((b >> 2) & 1) & 0x1ea119faUL) ^ (-((b >> 3) & 1) & 0x3d4233ddUL) ^ (-((b >> 4) & 1) & 0x2a1462b3UL);
}

static const char *charset = "qpzry9x8gf2tvdw0s3jn54khce6mua7l";

static const int8_t charset_rev[128] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 15, -1, 10, 17,
    21, 20, 26, 30, 7,  5,  -1, -1, -1, -1, -1, -1, -1, 29, -1, 24, 13, 25, 9,  8,  23, -1, 18, 22, 31, 27,
    19, -1, 1,  0,  3,  16, 11, 28, 12, 14, 6,  4,  2,  -1, -1, -1, -1, -1, -1, 29, -1, 24, 13, 25, 9,  8,
    23, -1, 18, 22, 31, 27, 19, -1, 1,  0,  3,  16, 11, 28, 12, 14, 6,  4,  2,  -1, -1, -1, -1, -1};

int bech32_encode(char *const output, const size_t out_len, const char *const hrp, const uint8_t *const data,
                  const size_t data_len) {
    uint32_t chk = 1;
    size_t out_off = 0;
    {
        size_t i = 0;
        while (hrp[i] != '\0') {
            if (hrp[i] >= 'A' && hrp[i] <= 'Z')
                return 0;
            if (!(hrp[i] >> 5))
                return 0;
            chk = bech32_polymod_step(chk) ^ (hrp[i] >> 5);
            ++i;
        }
        if (i + 7 + data_len > 90)
            return 0;
        chk = bech32_polymod_step(chk);
    }
    for (size_t i = 0; hrp[i] != '\0'; ++i) {
        chk = bech32_polymod_step(chk) ^ (hrp[i] & 0x1f);
        if (out_off >= out_len)
            return 0;
        output[out_off++] = hrp[i];
    }
    if (out_off >= out_len)
        return 0;
    output[out_off++] = '1';
    for (size_t i = 0; i < data_len; ++i) {
        if (data[i] >> 5)
            return 0;
        chk = bech32_polymod_step(chk) ^ data[i];
        if (out_off >= out_len)
            return 0;
        output[out_off++] = charset[data[i]];
    }
    for (size_t i = 0; i < 6; ++i) {
        chk = bech32_polymod_step(chk);
    }
    chk ^= 1;
    for (size_t i = 0; i < 6; ++i) {
        if (out_off >= out_len)
            return 0;
        output[out_off++] = charset[(chk >> ((5 - i) * 5)) & 0x1f];
    }
    output[out_off] = 0;
    return 1;
}

int bech32_decode(char *hrp, uint8_t *data, size_t data_max, size_t *data_len, const char *input) {
    uint32_t chk = 1;
    size_t i;
    size_t input_len = strlen(input);
    size_t hrp_len;
    int have_lower = 0, have_upper = 0;
    if (input_len < 8 || input_len > 90) {
        return 0;
    }
    size_t data_idx = 0;
    while (data_idx < input_len && input[(input_len - 1) - data_idx] != '1') {
        ++data_idx;
    }
    hrp_len = input_len - (1 + data_idx);
    if (hrp_len < 1 || data_idx < 6) {
        return 0;
    }
    if (data_idx > data_max) {
        return 0;
    }
    *data_len = data_idx;
    *(data_len) -= 6;
    for (i = 0; i < hrp_len; ++i) {
        int ch = input[i];
        if (ch < 33 || ch > 126) {
            return 0;
        }
        if (ch >= 'a' && ch <= 'z') {
            have_lower = 1;
        } else if (ch >= 'A' && ch <= 'Z') {
            have_upper = 1;
            ch = (ch - 'A') + 'a';
        }
        hrp[i] = ch;
        chk = bech32_polymod_step(chk) ^ (ch >> 5);
    }
    hrp[i] = 0;
    chk = bech32_polymod_step(chk);
    for (i = 0; i < hrp_len; ++i) {
        chk = bech32_polymod_step(chk) ^ (input[i] & 0x1f);
    }
    ++i;
    while (i < input_len) {
        int v = (input[i] & 0x80) ? -1 : charset_rev[(int)input[i]];
        if (input[i] >= 'a' && input[i] <= 'z')
            have_lower = 1;
        if (input[i] >= 'A' && input[i] <= 'Z')
            have_upper = 1;
        if (v == -1) {
            return 0;
        }
        chk = bech32_polymod_step(chk) ^ v;
        if (i + 6 < input_len) {
            data[i - (1 + hrp_len)] = v;
        }
        ++i;
    }
    if (have_lower && have_upper) {
        return 0;
    }
    return chk == 1;
}

int convert_bits(
    uint8_t *const out, const size_t outlen_max, size_t *outlen, const int outbits,
    const uint8_t *const in, const size_t inlen, const int inbits,
    int pad)
{
    uint32_t val = 0;
    int bits = 0;
    size_t out_idx = 0;
    const uint32_t maxv = (((uint32_t)1) << outbits) - 1;
    for (size_t inx_idx = 0; inx_idx < inlen; ++inx_idx) {
        val = (val << inbits) | in[inx_idx];
        bits += inbits;
        while (bits >= outbits) {
            bits -= outbits;
            if (out_idx >= outlen_max)
                return 0;
            out[out_idx++] = (val >> bits) & maxv;
        }
    }
    if (pad) {
        if (bits) {
            if (out_idx >= outlen_max)
                return 0;
            out[out_idx++] = (val << (outbits - bits)) & maxv;
        }
    } else if (((val << (outbits - bits)) & maxv) || bits >= inbits) {
        return 0;
    }
    // Set out index
    *outlen = out_idx;
    return 1;
}

int segwit_addr_encode(char *output, size_t out_len, const char *hrp, int witver, const uint8_t *witprog,
                       size_t witprog_len) {
    const size_t data_size = 65;
    uint8_t data[data_size];
    size_t datalen = 0;
    if (witver > 16)
        return 0;
    if (witver == 0 && witprog_len != 20 && witprog_len != 32)
        return 0;
    if (witprog_len < 2 || witprog_len > 40)
        return 0;
    data[0] = witver;
    convert_bits(data + 1, data_size, &datalen, 5, witprog, witprog_len, 8, 1);
    ++datalen;
    return bech32_encode(output, out_len, hrp, data, datalen);
}

int segwit_addr_decode(int *witver, uint8_t *witdata, size_t witdata_len_max, size_t *witdata_len, const char *hrp,
                       const char *addr) {
    const size_t data_max = 84;
    uint8_t data[data_max];
    char hrp_actual[84];
    size_t data_len;
    if (!bech32_decode(hrp_actual, data, data_max, &data_len, addr))
        return 0;
    if (data_len == 0 || data_len > 65)
        return 0;
    if (strncmp(hrp, hrp_actual, 84) != 0)
        return 0;
    if (data[0] > 16)
        return 0;
    *witdata_len = 0;
    if (!convert_bits(witdata, witdata_len_max, witdata_len, 8, data + 1, data_len - 1, 5, 0))
        return 0;
    if (*witdata_len < 2 || *witdata_len > 40)
        return 0;
    if (data[0] == 0 && *witdata_len != 20 && *witdata_len != 32)
        return 0;
    *witver = data[0];
    return 1;
}
