#include "../syscall.h"

void HariMain(void) {
    char *buf;
    int win, i;
    init_malloc();
    buf = malloc(160 * 100);
    win = create_window(buf, 160, 100, -1, "lines");
    for (i = 0; i < 8; i++) {
        draw_line_window(win + 1, 8, 26, 77, i * 9 + 26, i);
        draw_line_window(win + 1, 88, 26, i * 9 + 88, 89, i);
    }
    refresh_window(win, 6, 26, 154, 90);
    for (;;) {
        if (getkey(1) == 0x0a) {
            break;
        }  // 0x0a = Enter
    }
    close_window(win);
    api_end();
}
