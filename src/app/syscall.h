// clang-format off
void  putchar(int c);
void  puts(char *s);
void  puts_with_len(char *s, int l);
void  api_end(void);
int   open(char *buf, int xsiz, int ysiz, int col_inv, char *title);
void  putstr_window(int win, int x, int y, int col, int len, char *str);
void  box_fill_window(int win, int x0, int y0, int x1, int y1, int col);
void  init_malloc(void);
char  *malloc(int size);
void  free(char *addr, int size);
void  draw_point_window(int win, int x, int y, int col);
void  refresh_window(int win, int x0, int y0, int x1, int y1);
void  draw_line_window(int win, int x0, int y0, int x1, int y1, int col);
void  close(int win);
int   get_key(int mode);
int   alloc_timer(void);
void  init_timer(int timer, int data);
void  set_timeout(int timer, int time);
void  clear_timeout(int timer);
void  beep(int tone);
int   fopen(char *fname);
void  fclose(int fhandle);
void  fseek(int fhandle, int offset, int mode);
int   fsize(int fhandle, int mode);
int   fread(char *buf, int maxsize, int fhandle);
int   cmdline(char *buf, int maxsize);
int   get_lang_mode(void);

void  *alloca(int size);