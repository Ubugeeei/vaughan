#include "../../include/stdio.h"

void HariMain(void) {
	static char s[9] = { 0xb2, 0xdb, 0xca, 0xc6, 0xce, 0xcd, 0xc4, 0x0a, 0x00 }; // Hankaku "i ro ha ni ho he to" + new line + 0
	puts(s);
	exit();
}
