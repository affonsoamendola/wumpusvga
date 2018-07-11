#define GRAPHICS_MODE13 0x13
#define TEXT_MODE 0x03
#define GRAPHICS_MODEZ 0xFF

#define ROM_CHAR_SET_SEG 0xF000
#define ROM_CHAR_SET_OFF 0XFA6E

#define SEQUENCER 0x3C4
#define CRT_CONTROLLER 0x3D4
#define GFX_CONTROLLER 0x3CE
#define ATTR_CONTROLLER_FF 0x3DA
#define ATTR_CONTROLLER_DATA 0x3C0

#define CRT_MAX_SCANLINE 0x09
#define CRT_ADDR_MODE 0x14
#define CRT_MODE_CONTROL 0x17

#define GFX_WRITE_MODE 0x05
#define GFX_MISC 0x06

#define SEQ_PLANE_ENABLE 0x02
#define SEQ_MEMORY_MODE 0x04

#define SET_BITS(x,bits)(x|bits)
#define RESET_BITS(x,bits)(x&~bits)

void set_graphics_mode(int mode);
void fill_screen(int color);
void set_pixel(int x, int y, int color);
int get_pixel(int x, int y);

void draw_line_h(int x1, int x2, int y, int color);
void draw_line_v(int x, int y1, int y2, int color);

void fill_rectangle(int x1, int x2, int y1, int y2, int color);

void print_char(int xc, int yc, char c, int color, int transparent);
void print_string(int x, int y, int color, char *string, int transparent);

char* inttostring(int i, char b[]);