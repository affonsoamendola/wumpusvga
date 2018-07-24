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

#ifdef MSDOS
#define boardSizeX 29
#define boardSizeY 19
#endif

#ifdef GUNSMOKE
#define boardSizeX 25
#define boardSizeY 25
#endif

//----------------------------------------------------
//These are public functions implemented at wumpus.c:
int wumpus_main();
void set_game_level(int level);

//----------------------------------------------------
// And this is the tiny API that must be
// implemented by platform-specific ports:

//SYSTEM:
void init_system();

//VIDEO:
void init_video();
void clear_screen();
void fill_screen(char color);
void display_title_screen();
void drawScreen(int visited[boardSizeX][boardSizeY]);
void draw_message_box(const char* line1, const char* line2);
void display_game_over_screen();
void print_score(char score);
void after_gameover();

//INPUT:
void wait_for_user_input();
int read_user_input();

//UTILS:
int abs(int value);
int rand();
void integer_to_string(int value, char string[8]);

