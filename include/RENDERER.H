#define GRAPHICS_MODE13 0x13
#define TEXT_MODE 0x03

#define ROM_CHAR_SET_SEG 0xF000
#define ROM_CHAR_SET_OFF 0XFA6E

void set_graphics_mode(int mode);
void fill_screen(int color);
void set_pixel(int x, int y, int color);
int get_pixel(int x, int y);

void draw_line_h(int x1, int x2, int y, int color);
void draw_line_v(int x, int y1, int y2, int color);

void fill_rectangle(int x1, int x2, int y1, int y2, int color);

void print_char(int xc, int yc, char c, int color, int transparent);
void print_string(int x, int y, int color, char *string, int transparent);
