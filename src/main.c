#include "apdu.h"
#include "globals.h"
#include "memory.h"

__attribute__((noreturn)) void app_main(void) {
    apdu_handler const handlers[] = {
        (apdu_handler)PIC(handle_apdu_version),             // 0x00
        (apdu_handler)PIC(handle_apdu_get_wallet_id),       // 0x01
        (apdu_handler)PIC(handle_apdu_get_public_key),      // 0x02
        (apdu_handler)PIC(handle_apdu_get_public_key_ext),  // 0x03
        (apdu_handler)PIC(handle_apdu_sign_hash),           // 0x04
    };

    if(!N_data.initialized) {
        nvram_data data={
            true,
            ADDRESS_MAINNET,
            "mainnet",
        SIGN_HASH_OFF,
            "Off"
        };
        nvm_write((void*)&N_data, (void*)&data, sizeof(N_data));
    }
    main_loop(handlers, NUM_ELEMENTS(handlers));
}
