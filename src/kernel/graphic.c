/* Graphics */

#include "boot.h"

void init_palette(void) {
    // clang-format off
	static unsigned char table_rgb[16* 3] = {
	    0x00, 0x00, 0x00,  //  0:black
	    0xff, 0x00, 0x00,  //  1:red
	    0x00, 0xff, 0x00,  //  2:green
	    0xff, 0xff, 0x00,  //  3:yellow
	    0x00, 0x00, 0xff,  //  4:blue
	    0xff, 0x00, 0xff,  //  5:purple
	    0x00, 0xff, 0xff,  //  6:water blue
	    0xff, 0xff, 0xff,  //  7:white
	    0xc6, 0xc6, 0xc6,  //  8:gray
	    0x84, 0x00, 0x00,  //  9:dark red
	    0x00, 0x84, 0x00,  // 10:dark green
	    0x84, 0x84, 0x00,  // 11:dark yellow
	    0x00, 0x00, 0x84,  // 12:dark blue
	    0x84, 0x00, 0x84,  // 13:dark purple
	    0x00, 0x84, 0x84,  // 14:dark water blue
	    0x84, 0x84, 0x84,   // 15:dark gray
	};
    // clang-format on
    unsigned char table2[216 * 3];
    int r, g, b;
    set_palette(0, 15, table_rgb);
    for (b = 0; b < 6; b++) {
        for (g = 0; g < 6; g++) {
            for (r = 0; r < 6; r++) {
                table2[(r + g * 6 + b * 36) * 3 + 0] = r * 51;
                table2[(r + g * 6 + b * 36) * 3 + 1] = g * 51;
                table2[(r + g * 6 + b * 36) * 3 + 2] = b * 51;
            }
        }
    }
    set_palette(16, 231, table2);
    return;
}

void set_palette(int start, int end, unsigned char *rgb) {
    int i, eflags;
    eflags = asm_io_load_eflags();
    asm_io_cli();
    asm_io_out8(0x03c8, start);
    for (i = start; i <= end; i++) {
        asm_io_out8(0x03c9, rgb[0] / 4);
        asm_io_out8(0x03c9, rgb[1] / 4);
        asm_io_out8(0x03c9, rgb[2] / 4);
        rgb += 3;
    }
    asm_io_store_eflags(eflags);
    return;
}

// clang-format off
void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1) {
    // clang-format on
    int x, y;
    for (y = y0; y <= y1; y++) {
        for (x = x0; x <= x1; x++) {
            vram[y * xsize + x] = c;
        }
    }
    return;
}

void init_screen8(char *vram, int x, int y) {
    boxfill8(vram, x, COL8_000000, 0, 0, x - 1, y - 29);
    boxfill8(vram, x, COL8_C6C6C6, 0, y - 28, x - 1, y - 28);
    boxfill8(vram, x, COL8_FFFFFF, 0, y - 27, x - 1, y - 27);
    boxfill8(vram, x, COL8_C6C6C6, 0, y - 26, x - 1, y - 1);

    boxfill8(vram, x, COL8_FFFFFF, 3, y - 24, 59, y - 24);
    boxfill8(vram, x, COL8_FFFFFF, 2, y - 24, 2, y - 4);
    boxfill8(vram, x, COL8_848484, 3, y - 4, 59, y - 4);
    boxfill8(vram, x, COL8_848484, 59, y - 23, 59, y - 5);
    boxfill8(vram, x, COL8_000000, 2, y - 3, 59, y - 3);
    boxfill8(vram, x, COL8_000000, 60, y - 24, 60, y - 3);

    boxfill8(vram, x, COL8_848484, x - 47, y - 24, x - 4, y - 24);
    boxfill8(vram, x, COL8_848484, x - 47, y - 23, x - 47, y - 4);
    boxfill8(vram, x, COL8_FFFFFF, x - 47, y - 3, x - 4, y - 3);
    boxfill8(vram, x, COL8_FFFFFF, x - 3, y - 24, x - 3, y - 3);
    return;
}

void putfont8(char *vram, int xsize, int x, int y, char c, char *font) {
    int i;
    char *p, d;
    for (i = 0; i < 16; i++) {
        p = vram + (y + i) * xsize + x;
        d = font[i];
        if ((d & 0x80) != 0) {
            p[0] = c;
        }
        if ((d & 0x40) != 0) {
            p[1] = c;
        }
        if ((d & 0x20) != 0) {
            p[2] = c;
        }
        if ((d & 0x10) != 0) {
            p[3] = c;
        }
        if ((d & 0x08) != 0) {
            p[4] = c;
        }
        if ((d & 0x04) != 0) {
            p[5] = c;
        }
        if ((d & 0x02) != 0) {
            p[6] = c;
        }
        if ((d & 0x01) != 0) {
            p[7] = c;
        }
    }
    return;
}

// clang-format off
void putfonts8_asc(char *vram, int xsize, int x, int y, char c, unsigned char *s) {
    // clang-format on
    extern char hankaku[4096];
    struct Task *task = task_now();
    char *jp_font = (char *)*((int *)0x0fe8), *font;
    int k, t;

    if (task->lang_mode == 0) {
        for (; *s != 0x00; s++) {
            putfont8(vram, xsize, x, y, c, hankaku + *s * 16);
            x += 8;
        }
    } else if (task->lang_mode == 1) {
        for (; *s != 0x00; s++) {
            if (task->lang_byte1 == 0) {
                if ((0x81 <= *s && *s <= 0x9f) || (0xe0 <= *s && *s <= 0xfc)) {
                    task->lang_byte1 = *s;
                } else {
                    putfont8(vram, xsize, x, y, c, jp_font + *s * 16);
                }
            } else {
                if (0x81 <= task->lang_byte1 && task->lang_byte1 <= 0x9f) {
                    k = (task->lang_byte1 - 0x81) * 2;
                } else {
                    k = (task->lang_byte1 - 0xe0) * 2 + 62;
                }
                if (0x40 <= *s && *s <= 0x7e) {
                    t = *s - 0x40;
                } else if (0x80 <= *s && *s <= 0x9e) {
                    t = *s - 0x80 + 63;
                } else {
                    t = *s - 0x9f;
                    k++;
                }
                task->lang_byte1 = 0;
                font = jp_font + 256 * 16 + (k * 94 + t) * 32;
                putfont8(vram, xsize, x - 8, y, c, font);   // Left half
                putfont8(vram, xsize, x, y, c, font + 16);  // Right half
            }
            x += 8;
        }
    } else if (task->lang_mode == 2) {
        // EUC
        for (; *s != 0x00; s++) {
            if (task->lang_byte1 == 0) {
                if (0x81 <= *s && *s <= 0xfe) {
                    task->lang_byte1 = *s;
                } else {
                    putfont8(vram, xsize, x, y, c, jp_font + *s * 16);
                }
            } else {
                k = task->lang_byte1 - 0xa1;
                t = *s - 0xa1;
                task->lang_byte1 = 0;
                font = jp_font + 256 * 16 + (k * 94 + t) * 32;
                putfont8(vram, xsize, x - 8, y, c, font);
                putfont8(vram, xsize, x, y, c, font + 16);
            }
            x += 8;
        }
    }
    return;
}

void init_mouse_cursor8(char *mouse, char bc) {
    // clang-format off
    static char cursor[16][16] = {
        "**************..",
		"*OOOOOOOOOOO*...",
		"*OOOOOOOOOO*....",
        "*OOOOOOOOO*.....",
		"*OOOOOOOO*......",
		"*OOOOOOO*.......",
        "*OOOOOOO*.......",
		"*OOOOOOOO*......",
		"*OOOO**OOO*.....",
        "*OOO*..*OOO*....",
		"*OO*....*OOO*...",
		"*O*......*OOO*..",
        "**........*OOO*.",
		"*..........*OOO*",
		"............*OO*",
        ".............***"
		};
    // clang-format on
    int x, y;
    for (y = 0; y < 16; y++) {
        for (x = 0; x < 16; x++) {
            if (cursor[y][x] == '*') {
                // mouse[y * 16 + x] = COL8_000000;
                mouse[y * 16 + x] = bc;
            }
            if (cursor[y][x] == 'O') {
                mouse[y * 16 + x] = COL8_FFFFFF;
            }
            if (cursor[y][x] == '.') {
                mouse[y * 16 + x] = bc;
            }
        }
    }
    return;
}

void putblock8_8(char *vram, int vxsize, int pxsize, int pysize, int px0,
                 int py0, char *buf, int bxsize) {
    int x, y;
    for (y = 0; y < pysize; y++) {
        for (x = 0; x < pxsize; x++) {
            vram[(py0 + y) * vxsize + (px0 + x)] = buf[y * bxsize + x];
        }
    }
    return;
}
