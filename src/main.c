#include "apdu.h"
#include "globals.h"
#include "memory.h"

__attribute__((noreturn)) void app_main(void) {
    // TODO: Consider using static initialization of a const, instead of this
    for (size_t i = 0; i < NUM_ELEMENTS(global.handlers); i++) {
        global.handlers[i] = handle_apdu_error;
    }
    global.handlers[APDU_INS(INS_VERSION)] = handle_apdu_version;
    global.handlers[APDU_INS(INS_GET_WALLET_ID)] = handle_apdu_get_wallet_id;
    global.handlers[APDU_INS(INS_PROMPT_PUBLIC_KEY)] = handle_apdu_get_public_key;
    global.handlers[APDU_INS(INS_SIGN)] = handle_apdu_sign;
    global.handlers[APDU_INS(INS_GIT)] = handle_apdu_git;
    if(!N_data.initialized) {
	    nvram_data data={
		    true,
		    false,
		    "mainnet"
	    };
	    nvm_write((void*)&N_data, (void*)&data, sizeof(N_data));
    }
    main_loop(global.handlers, sizeof(global.handlers));
}
