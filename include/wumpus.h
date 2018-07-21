#define INPUT_QUIT_GAME      (1 <<  0)
#define INPUT_CHEAT_MENU     (1 <<  1)
#define INPUT_ENABLE_CHEAT1  (1 <<  2)
#define INPUT_ENABLE_CHEAT2  (1 <<  3)
#define INPUT_JOYSTICK_UP    (1 <<  4)
#define INPUT_JOYSTICK_LEFT  (1 <<  5)
#define INPUT_JOYSTICK_DOWN  (1 <<  6)
#define INPUT_JOYSTICK_RIGHT (1 <<  7)
#define INPUT_ACTION_FIRE    (1 <<  8)
#define INPUT_ACTION_GET     (1 <<  9)
#define INPUT_LEVEL_EASY     (1 << 10)
#define INPUT_LEVEL_MEDIUM   (1 << 11)
#define INPUT_LEVEL_HARD     (1 << 12)
#define INPUT_LEVEL_ULTRA    (1 << 13)

int wumpus_main();

// This is the tiny API that must be
// implemented by platform-specific ports:
void init_system();
void init_video();
void display_title_screen();
void wait_for_user_input();
void draw_message_box(const char* line1, const char* line2);
int read_user_input();
void display_game_over_screen();
int rand();
void print_score(char score);
void fill_screen(char color);
int abs(int value);
void drawScreen(int** visited);
void integer_to_string(int value, char string[8]);
void after_gameover();

