#include "apdu_pubkey.h"

#include "apdu.h"
#include "cx.h"
#include "globals.h"
#include "keys.h"
#include "protocol.h"
#include "to_string.h"
#include "ui.h"
#ifdef BAKING_APP
#include "baking_auth.h"
#endif // BAKING_APP

#include <string.h>

#define G global.apdu.u.pubkey

static bool pubkey_ok(void) {
    delayed_send(provide_pubkey(G_io_apdu_buffer, &G.public_key));
    return true;
}

#define BIP32_HARDENED_PATH_BIT 0x80000000

void bip32_path_to_string(
  char *const out,
  size_t const out_size,
  derived_pubkey_t const *const pubkey)
{
  size_t out_current_offset = 0;
  for(int i=0;i<MAX_BIP32_PATH && i<pubkey->key.length; i++) {
    bool is_hardened = pubkey->key.components[i] & BIP32_HARDENED_PATH_BIT;
    uint32_t component = pubkey->key.components[i] & ~BIP32_HARDENED_PATH_BIT;
    number_to_string_indirect32(out+out_current_offset, out_size-out_current_offset, &component);
    out_current_offset = strlen(out);
    if(is_hardened) {
      out[out_current_offset] = '\'';
      out_current_offset+=1;
    }
    out[out_current_offset] = '/';
    out_current_offset+=1;
    out[out_current_offset]='\0';
  }
}

__attribute__((noreturn))
static void prompt_path(
    ui_callback_t ok_cb,
    ui_callback_t cxl_cb
    ) {
  static size_t const TYPE_INDEX = 0;
  static size_t const ADDRESS_INDEX = 1;

  static const char *const pubkey_labels[] = {
    PROMPT("Provide"),
    PROMPT("Public Key For"),
    NULL,
  };
  REGISTER_STATIC_UI_VALUE(TYPE_INDEX, "Public Key");
  register_ui_callback(ADDRESS_INDEX, bip32_path_to_string, &G.key);
  ui_prompt(pubkey_labels, ok_cb, cxl_cb);
}

size_t handle_apdu_get_public_key(uint8_t _U_ instruction) {
  uint8_t *dataBuffer = G_io_apdu_buffer + OFFSET_CDATA;

  if (READ_UNALIGNED_BIG_ENDIAN(uint8_t, &G_io_apdu_buffer[OFFSET_P1]) != 0) THROW(EXC_WRONG_PARAM);

  size_t const cdata_size = READ_UNALIGNED_BIG_ENDIAN(uint8_t, &G_io_apdu_buffer[OFFSET_LC]);

  read_bip32_path(&G.key, dataBuffer, cdata_size);
  generate_public_key(&G.public_key, &G.key);

  // instruction == INS_PROMPT_PUBLIC_KEY || instruction == INS_AUTHORIZE_BAKING
  ui_callback_t cb;
  bool bake;
  // INS_PROMPT_PUBLIC_KEY
  cb = pubkey_ok;
  bake = false;
  prompt_path(cb, delay_reject);
}
