#include "glyphs.h"
unsigned int const C_icon_crossmark_colors[] = {
  0x00000000,
  0x00ffffff,
};

unsigned char const C_icon_crossmark_bitmap[] = {
  0x00, 0x80, 0x01, 0xe6, 0xc0, 0x71, 0x38, 0x38, 0x07, 0xfc, 0x00, 0x1e, 0x80, 0x07, 0xf0, 0x03,
  0xce, 0xc1, 0xe1, 0x38, 0x70, 0x06, 0x18, 0x00, 0x00,
};
#ifdef OS_IO_SEPROXYHAL
        #include "os_io_seproxyhal.h"
        const bagl_icon_details_t C_icon_crossmark = { GLYPH_icon_crossmark_WIDTH, GLYPH_icon_crossmark_HEIGHT, 1, C_icon_crossmark_colors, C_icon_crossmark_bitmap };
        #endif // OS_IO_SEPROXYHAL
