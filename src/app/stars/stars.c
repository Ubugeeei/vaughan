#include "../../apilib.h"

int rand(void);  // 0~32767

void HariMain(void) {
    char *buf;
    int win, i, x, y;
    init_malloc();
    buf = malloc(150 * 100);
    win = open(buf, 150, 100, -1, "stars");
    box_fill_window(win, 6, 26, 143, 93, 0);
    for (i = 0; i < 50; i++) {
        x = (rand() % 137) + 6;
        y = (rand() % 67) + 26;
        draw_point_window(win, x, y, 3);
    }
}

/* https://stackoverflow.com/questions/4768180/rand-implementation/4768194 */
int rand(void) {  // RAND_MAX assumed to be 32767
    static unsigned long int next = 1;
    next = next * 1103515245 + 12345;
    return (unsigned int)(next / 65536) % 32768;
}
