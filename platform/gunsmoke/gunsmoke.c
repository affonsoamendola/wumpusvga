/*
    (c) 2018 Affonso Amendola
    (c) 2018 Felipe Sanches <juca@members.fsf.org>
    
    This file is part of WumpusVGA.

    WumpusVGA is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    WumpusVGA is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with WumpusVGA.  If not, see <https://www.gnu.org/licenses/>
*/

#include "wumpus.h"
#include "char_codes.h"

#define SYSTEM ((char*) 0xC000)
#define P1 ((char*) 0xC001)
#define P2 ((char*) 0xC002)
#define DSW1 ((char*) 0xC003)
#define DSW2 ((char*) 0xC004)
#define SOUND_COMMAND ((char*) 0xC800)
#define NO_SOUND ((char) 0xff)
#define HWCFG ((char*) 0xC804)
#define VIDEORAM ((char*) 0xD000) //videoram area: D000-D3FF
#define COLORRAM ((char*) 0xD400) //videoram area: D400-D7FF
#define SCROLLX ((char*) 0xD800)
#define SCROLLY ((char*) 0xD802)
#define VIDEOCFG ((char*) 0xD806)
#define SPRITERAM ((char*) 0xF000) //spriteram area: F000-FFFF

#define CLEAR_COLOR ((char) 0x80) //why?!
#define SCENARIO_COLOR 9
#define EXAMPLE_COLOR 4
#define OPENFLOOR_COLOR 5

#define SCENARIO_WIDTH 28
#define SCENARIO_HEIGHT 32

#define bool char
#define true ((char) 0xFF)
#define false ((char) 0x00)
#define UPPERCASE(ascii) (ascii & ~0b00100000)

extern int boardPosX;
extern int boardPosY;
extern int playerPosX;
extern int playerPosY;
extern int wumpusPosX;
extern int wumpusPosY;
extern int arrowPosX;
extern int arrowPosY;
extern int hasArrow;
extern int secretExitX;
extern int secretExitY;
//extern int goldPosX;
//extern int goldPosY;
extern int escaping;
extern int showWumpus;
extern int showWumpus;
//extern int showGold;
//extern int showHoles;
extern int gameRunning;

int scroll_x_pos;

//routine for placing a character on screen
void set_char(int x, int y, char char_code, char color){
	*(COLORRAM + 32 * x + (32 - y)) = color;
	*(VIDEORAM + 32 * x + (32 - y)) = char_code;
}

// Routine to print a line of text at a
// given screen coordinate
void print_line(const char* str, int x, int y, char color){
	int i = 0;
	int px = 3;
	while (px<x){
		set_char(px++, y, BLANK_CHAR, color);
	}

	while (str[i] != 0 && x < 27){
		if (str[i]==' '){
			set_char(x++, y, BLANK_CHAR, color);
		} else {
			set_char(x++, y, UPPERCASE(str[i]) - 55, color);
		}
		i++;
	}

	px = x;
	while (px<27){
		set_char(px++, y, BLANK_CHAR, color);
	}
}

void print_score(char score)
{
	score;
	//TODO: Implement-me!
}

void clear_screen(){
	char* cram = COLORRAM;
	char* vram = VIDEORAM;
	int i;
	for (i=0; i < 1024; cram++, vram++, i++)
	{
		*(cram) = CLEAR_COLOR;
		*(vram) = BLANK_CHAR;
	}

	draw_scenario();
}

void draw_scenario(){
	int x,y;

	set_char(                   2,                     1, CORNER_TOP_LEFT,      SCENARIO_COLOR);
	set_char(                   2, SCENARIO_HEIGHT-1 + 1, CORNER_BOTTOM_LEFT,   SCENARIO_COLOR);
	set_char(SCENARIO_WIDTH-1 + 2,                     1, CORNER_TOP_RIGHT,     SCENARIO_COLOR);
	set_char(SCENARIO_WIDTH-1 + 2, SCENARIO_HEIGHT-1 + 1, CORNER_BOTTOM_RIGHT,  SCENARIO_COLOR);

	for (x=1; x<SCENARIO_WIDTH/2; x++){
		set_char(x + 2,                     1,    TOP_BORDER__LEFT,  SCENARIO_COLOR);
		set_char(x + 2, SCENARIO_HEIGHT-1 + 1, BOTTOM_BORDER__LEFT,  SCENARIO_COLOR);
	}

	for (x=SCENARIO_WIDTH/2; x<SCENARIO_WIDTH-1; x++){
		set_char(x + 2,                     1,    TOP_BORDER__RIGHT,  SCENARIO_COLOR);
		set_char(x + 2, SCENARIO_HEIGHT-1 + 1, BOTTOM_BORDER__RIGHT,  SCENARIO_COLOR);
	}

	for (y=1; y<SCENARIO_HEIGHT/2; y++){
		set_char(                   2, y + 1, TOP_BORDER__LEFT,   SCENARIO_COLOR);
		set_char(SCENARIO_WIDTH-1 + 2, y + 1, TOP_BORDER__RIGHT,  SCENARIO_COLOR);
	}

	for (y=SCENARIO_HEIGHT/2; y<SCENARIO_HEIGHT-1; y++){
		set_char(                   2, y + 1, BOTTOM_BORDER__LEFT,   SCENARIO_COLOR);
		set_char(SCENARIO_WIDTH-1 + 2, y + 1, BOTTOM_BORDER__RIGHT,  SCENARIO_COLOR);
	}
}

void init_video(){
	scroll_x_pos = 0x0000;
	*VIDEOCFG = 0x30; // enables bg / enables sprites / selects sprite3bank #0
	*HWCFG = ((char) 0x80); // unflip screen and enable chars

	clear_screen();
}

void init_system(){
	*SOUND_COMMAND = NO_SOUND;

	init_video();
	draw_scenario();
	set_game_level(3);//max level by default
	draw_message_box("SYSTEM", "INIT");
	gameRunning = 1;

	while(true) {
	__asm
    ei
  __endasm;
	}
}

void integer_to_string(int value, char string[8])
{
	value;
	string;
	//TODO: Implement-me!
}

int read_user_input()
{
	int input = 0;
	char p1 = ((char) 0x00);

	while (p1 != ((char) 0xFF)) {
		p1 = *P1;
	}

	while(p1 == ((char) 0xFF)) {
		p1 = *P1;
	}

	if ((p1 & (1 << 0)) == 0) input |= INPUT_JOYSTICK_RIGHT;
	if ((p1 & (1 << 1)) == 0) input |= INPUT_JOYSTICK_LEFT;
	if ((p1 & (1 << 2)) == 0) input |= INPUT_JOYSTICK_DOWN;
	if ((p1 & (1 << 3)) == 0) input |= INPUT_JOYSTICK_UP;
	if ((p1 & (1 << 4)) == 0) input |= INPUT_ACTION_FIRE;
	if ((p1 & (1 << 5)) == 0) input |= INPUT_ACTION_GET;
/*
	char dsw1 = 0;
	dsw1 = *DSW1;
	if (dsw1 & (1 << 0)) input |= INPUT_ENABLE_CHEAT1;
	if (dsw1 & (1 << 1)) input |= INPUT_ENABLE_CHEAT2;
	switch ((dsw1 >> 2) & 3)
	{
		case 0: input |= INPUT_LEVEL_EASY; break;
		case 1: input |= INPUT_LEVEL_MEDIUM; break;
		case 2: input |= INPUT_LEVEL_HARD; break;
		case 3: input |= INPUT_LEVEL_ULTRA; break;
	}
*/
	input |= INPUT_LEVEL_ULTRA;
	return input;
}

void wait_for_user_input()
{
	//This is a bit silly...
	read_user_input();
}

void set_scrollx(int pos){
	*(SCROLLX) = pos&0xFF;
	*(SCROLLX+1) = (pos>>8)&0xFF;
}

/*
void main_loop(){
	static int mainloop_counter = 0;

	mainloop_counter++;
	if (mainloop_counter > 10){
		wumpus_main();
		mainloop_counter = 0;
	}

	set_scrollx(scroll_x_pos++);
}*/

void main_loop()
{
	wumpus_main();
}

void draw_message_box(const char* line1,
                      const char* line2)
{
	print_line(line1, 4, 4, 1);
	print_line(line2, 4, 5, 2);
}

void display_title_screen()
{
	clear_screen();
	draw_message_box("TITLE SCREEN", "IMPLEMENT ME");
}

void display_game_over_screen()
{
	clear_screen();
	draw_message_box("GAME OVER SCREEN", "IMPLEMENT ME");
}

int rand(){
	static char* address = (char*) 0xe000; //RAM
	int value = *(address++);
	value = value << 8 | *(address++);

	if (address > ((char*) 0xe7ff)) address = (char*) 0xe000;
	return value;
}

void fill_screen(char color)
{
	color;
	//TODO: Implement-me!
}

int abs(int value)
{
	return (value > 0) ? value : -value;
}

void after_gameover()
{
	//TODO: Implement-me!
	draw_message_box("THIS IS AFTER", "THE GAME OVER SCREEN");
}

void drawScreen(int visited[boardSizeX][boardSizeY])
{
	int x, y;
	int borderX = 3, borderY = 6;
	for(x = 0; x <= boardSizeX; x++)
	{
		for(y = 0; y <= boardSizeY; y++)
		{
			set_char(borderX + x, borderY + y, OPEN_FLOOR, OPENFLOOR_COLOR);
			if(x == 0 || x == boardSizeX ||
			   y == 0 || y == boardSizeY)
				set_char(borderX + x, borderY + y, HIDDEN_FLOOR, EXAMPLE_COLOR);

			if(x == playerPosX && y == playerPosY)
				set_char(borderX + x, borderY + y, PLAYER, SCENARIO_COLOR);

			if(visited[x][y] == 0 &&
			   x > 0 && x < boardSizeX &&
			   y > 0 && y < boardSizeY)
				set_char(borderX + x, borderY + y, HIDDEN_FLOOR, SCENARIO_COLOR);

//			if(hole[x][y]==1 && showHoles==1)
//				set_char(borderX + x, borderY + y, HOLE, SCENARIO_COLOR);
//
//			if(x==goldPosX && y==goldPosY && showGold==1)
//				set_char(borderX + x, borderY + y, GOLD, SCENARIO_COLOR);

			if((x == wumpusPosX && y == wumpusPosY) &&
			   (showWumpus == 1 || visited[x][y] == 1))
				set_char(borderX + x, borderY + y, WUMPUS_AWAKEN, SCENARIO_COLOR); //TODO: animate AWAKEN / SLEEPING1 / SLEEPING2
		}
	}

	if(escaping==1)
		set_char(borderX + secretExitX, borderY + secretExitY, HIDDEN_FLOOR, SCENARIO_COLOR);
}
