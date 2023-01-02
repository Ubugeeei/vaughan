#include "../syscall.h"

unsigned char rgb2pal(int r, int g, int b, int x, int y);

void HariMain(void) {
    char *buf;
    int win, x, y;
    init_malloc();
    buf = malloc(144 * 164);
    win = create_window(buf, 144, 164, -1, "color2");
    for (y = 0; y < 128; y++) {
        for (x = 0; x < 128; x++) {
            buf[(x + 8) + (y + 28) * 144] = rgb2pal(x * 2, y * 2, 0, x, y);
        }
    }
    refresh_window(win, 8, 28, 136, 156);
    getkey(1);
    api_end();
}

unsigned char rgb2pal(int r, int g, int b, int x, int y) {
    static int table[4] = {3, 1, 0, 2};
    int i;
    x &= 1;  // Odd or even
    y &= 1;
    i = table[x + y * 2];  // Constant
    r = (r * 21) / 256;    // 0~20
    g = (g * 21) / 256;
    b = (b * 21) / 256;
    r = (r + i) / 4;  // 0~5
    g = (g + i) / 4;
    b = (b + i) / 4;
    return 16 + r + g * 6 + b * 36;
}
