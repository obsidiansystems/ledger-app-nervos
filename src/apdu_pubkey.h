#pragma once

#include "apdu.h"

size_t handle_apdu_get_public_key(uint8_t instruction);
void render_pkh(char *const out, size_t const out_size, render_address_payload_t const *const payload);

