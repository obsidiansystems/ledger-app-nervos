#include "globals.h"
#include "types.h"

#define G global.ui


void switch_screen(uint32_t which) {
    if (which >= MAX_SCREEN_COUNT)
        THROW(EXC_MEMORY_ERROR);
    const char *label = (const char *)PIC(global.ui.prompt.prompts[which]);
    strncpy(global.ui.prompt.active_prompt, label, sizeof(global.ui.prompt.active_prompt));
    if (global.ui.prompt.callbacks[which] == NULL)
        THROW(EXC_MEMORY_ERROR);
    global.ui.prompt.callbacks[which](global.ui.prompt.active_value, sizeof(global.ui.prompt.active_value),
                                      global.ui.prompt.callback_data[which]);
}

void ui_prompt_debug(size_t screen_count) {
    for (uint32_t i = 0; i < screen_count; i++) {
        G.switch_screen(i);
        PRINTF("Prompt %d:\n%s\n%s\n", i, global.ui.prompt.active_prompt, global.ui.prompt.active_value);
    }
}

__attribute__((noreturn)) void ui_prompt_with_cb(void (*switch_screen_cb)(size_t), size_t screen_count,
                                                 ui_callback_t ok_c, ui_callback_t cxl_c) {
    check_null(switch_screen_cb);
    if (screen_count > MAX_SCREEN_COUNT)
        THROW(EXC_MEMORY_ERROR);

    G.switch_screen = switch_screen_cb;
    G.prompt.offset = MAX_SCREEN_COUNT - screen_count;

    ui_prompt_debug(screen_count);

    G.ok_callback = ok_c;
    G.cxl_callback = cxl_c;

    // Validate everything
    G.ok_callback();
    THROW(ASYNC_EXCEPTION);
}

__attribute__((noreturn)) void ui_prompt(const char *const *labels, ui_callback_t ok_c, ui_callback_t cxl_c) {
    check_null(labels);
    global.ui.prompt.prompts = labels;

    size_t const screen_count = ({
        size_t i = 0;
        while (i < MAX_SCREEN_COUNT && labels[i] != NULL) {
            i++;
        }
        i;
    });

    G.switch_screen = switch_screen;
    // We fill the destination buffers at the end instead of the beginning so we can
    // use the same array for any number of screens.
    G.prompt.offset = MAX_SCREEN_COUNT - screen_count;

    ui_prompt_debug(screen_count);

    G.ok_callback = ok_c;
    G.cxl_callback = cxl_c;

    // Validate everything
    G.ok_callback();
    THROW(ASYNC_EXCEPTION);
}
