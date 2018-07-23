/*  Copyright 2018 Affonso Amendola
    
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

#include <bios.h>
#include <dos.h>
#include <time.h>
#include <conio.h>
#include <wumpus.h>
#include <RENDERER.H>

extern int hole[boardSizeX][boardSizeY];
extern int secretExitWall;
extern int visitedColor;
extern int boardBorderColor;
extern int boardSquareSize;
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
extern int goldPosX;
extern int goldPosY;
extern int escaping;
extern int showWumpus;
extern int showWumpus;
extern int showGold;
extern int showHoles;

unsigned char far * title_mem_location = 0xA000E100L;
unsigned char far * sprites_mem_location = 0xA000FC00L;
unsigned char* color_pallette;

int main()
{
	init_system();
        wumpus_main();
	return 0;
}

void init_video()
{
	set_graphics_mode(GRAPHICS_MODEX);

	draw_page(0);
	frame_page(1);
}

void init_system(){
	srand(time(0));

	init_video();
}

void integer_to_string(int value, char string[8])
{
	inttostring(value, string);
}

void display_title_screen()
{
	color_pallette = malloc(256*3);
	get_pallette(color_pallette, 0, 255);
	load_pallette("plt/TITLE.PLT", 12);
	load_pallette("plt/SPRITES.PLT", 12);
	load_pgm("graphix/title.pgm", title_mem_location, 216, 124);
	load_pgm("graphix/sprites.pgm", sprites_mem_location, 60, 40);

	copy_vmem_to_dbuffer(title_mem_location, (SCREEN_WIDTH>>1)-(216>>1), (SCREEN_HEIGHT>>1)-(124>>1), 0, 0, 216, 124, 216);

	print_string((SCREEN_WIDTH>>1)-100,227,40,"By Affonso Amendola, 2018",1);
	frame_page(0);
}

void clear_screen()
{
	fill_screen(0);
}

void wait_for_user_input()
{
	getch();
}

void draw_message_box(const char* line1,
                      const char* line2)
{
	fill_rectangle(10, 320, 95, 105, 0);
	print_string(10, 96, 40, line1, 1);
	print_string(10, 106, 40, line2, 1);
}

void print_score(char score){
	//TODO: implement-me!
}

int read_user_input(){
	int input = 0;
	switch (getch())
	{
		case 'q': input |= INPUT_QUIT_GAME; break;
		case 'c': input |= INPUT_CHEAT_MENU; break;
		case 'z': input |= INPUT_ENABLE_CHEAT1; break;
		case 'x': input |= INPUT_ENABLE_CHEAT2; break;
		case 'u': input |= (INPUT_JOYSTICK_UP |
		                    INPUT_LEVEL_ULTRA); break;
		case 'h': input |= (INPUT_JOYSTICK_LEFT |
		                    INPUT_LEVEL_HARD); break;
		case 'e': input |= INPUT_LEVEL_EASY; break;
		case 'm': input |= INPUT_LEVEL_MEDIUM; break;
		case 'j': input |= INPUT_JOYSTICK_DOWN; break;
		case 'k': input |= INPUT_JOYSTICK_RIGHT; break;
		case 'f': input |= INPUT_ACTION_FIRE; break;
		case 'g': input |= INPUT_ACTION_GET; break;
		default: break;
	}
	return input;
}

void display_game_over_screen(){
	int x,y;
	for(x=0;x<320;x++)
	{
		for(y=0;y<240;y++)
		{
			set_pixel(x,y,(int)((y*x)*(256.0/(320.0*240.0))));
		}
	}
}

void drawScreen(int visited[boardSizeX][boardSizeY])
{
	int x;
	int y;

	for(x=0;x<=boardSizeX;x++)
	{
		for(y=0;y<=boardSizeY;y++)
		{
			if(x==0 || y==0 || x==boardSizeX || y==boardSizeY)
			{
				fill_rectangle(	boardPosX+(x*boardSquareSize),
								boardPosX+((x+1)*boardSquareSize-1),
								boardPosY+(y*boardSquareSize),
								boardPosY+((y+1)*boardSquareSize-1),
								boardBorderColor);

			}
			if(x==playerPosX && y==playerPosY)
			{
				/*
				fill_rectangle(	boardPosX+(x*boardSquareSize),
						boardPosX+((x+1)*boardSquareSize)-1,
						boardPosY+(y*boardSquareSize),
						boardPosY+((y+1)*boardSquareSize)-1,
						playerColor);
				*/
				copy_vmem_to_dbuffer(sprites_mem_location,
				                     boardPosX + (x * boardSquareSize), 
				                     boardPosY+(y*boardSquareSize), 
				                     0, 0, 10, 10, 60);
			}
			if(visited[x][y] == 0 &&
			   (x > 0 && y > 0 &&
			    x < boardSizeX &&
			    y < boardSizeY))
			{
				fill_rectangle(	boardPosX + (x * boardSquareSize),
						boardPosX + ((x + 1) * boardSquareSize) - 1,
						boardPosY + (y * boardSquareSize),
						boardPosY + ((y + 1) * boardSquareSize) - 1,
						visitedColor);
			}
			if((x == wumpusPosX && y == wumpusPosY && showWumpus == 1) ||
			    x == wumpusPosX && y == wumpusPosY && visited[x][y] == 1)
			{
				fill_rectangle(boardPosX + (x * boardSquareSize),
				               boardPosX + ((x + 1) * boardSquareSize) - 1,
				               boardPosY + (y * boardSquareSize),
				               boardPosY + ((y + 1) * boardSquareSize) - 1,
				               150);
			}
			if(x == goldPosX && y == goldPosY && showGold == 1)
			{
				fill_rectangle(boardPosX + (x * boardSquareSize),
				               boardPosX + ((x + 1) * boardSquareSize) - 1,
				               boardPosY + (y * boardSquareSize),
				               boardPosY + ((y + 1) * boardSquareSize) - 1,
				               180);
			}
			if(hole[x][y] == 1 && showHoles == 1)
			{
				fill_rectangle(boardPosX + (x * boardSquareSize),
				               boardPosX + ((x + 1) * boardSquareSize) - 1,
				               boardPosY + (y * boardSquareSize),
				               boardPosY + ((y + 1) * boardSquareSize) - 1,
				               200);
			}
		}
	}

	if(escaping)
	{
		if(secretExitWall == 0)
		{
			fill_rectangle(boardPosX + secretExitX * boardSquareSize,
			               boardPosX + (secretExitX + 1) * boardSquareSize - 1,
			               boardPosY,
			               boardPosY + boardSquareSize - 1,
			               0);
		}
		if(secretExitWall==1)
		{
			fill_rectangle(boardPosX,
			               boardPosX + boardSquareSize - 1,
			               boardPosY + secretExitY * boardSquareSize,
			               boardPosY + (secretExitY + 1) * boardSquareSize - 1,
			               0);
		}
		if(secretExitWall==2)
		{
			fill_rectangle(boardPosX + secretExitX * boardSquareSize,
			               boardPosX + (secretExitX + 1) * boardSquareSize - 1,
			               boardPosY + boardSizeY * boardSquareSize,
			               boardPosY + (boardSizeY + 1) * boardSquareSize - 1,
			               0);
		}
		if(secretExitWall==3)
		{
			fill_rectangle(boardPosX + boardSizeX * boardSquareSize,
			               boardPosX + (boardSizeX + 1) * boardSquareSize - 1,
			               boardPosY + secretExitY * boardSquareSize,
			               boardPosY + (secretExitY + 1) * boardSquareSize - 1,
			               0);
		}
	}
}

void print_order_info()
{
	int i = 0;

	textcolor(WHITE);
	cprintf("\n");
	cprintf("        ");
	textcolor(RED);
	for(i=0;i<64;i++)cprintf("%c",0xDB);
	cprintf("        ");
	cprintf("        ");
	cprintf("%c",0xDB);
	textcolor(WHITE);
	textbackground(RED);
	cprintf("FOFONSO'S WUMPUS HUNT");
	textcolor(YELLOW);
	cprintf(": The hottest new game for ");
	textbackground(LIGHTGRAY);
	textcolor(RED);
	cprintf("V");
	textcolor(GREEN);
	cprintf("G");
	textcolor(BLUE);
	cprintf("A");
	textcolor(YELLOW);
	textbackground(RED);
	cprintf(" PC's.      ");
	textbackground(BLACK);
	cprintf("        ");
	cprintf("        ");
	textbackground(RED);
	cprintf(" ______________________________________________________________ ");
	textbackground(BLACK);
	cprintf("        ");
	cprintf("        ");
	textbackground(RED);
	cprintf("                                                                ");
	textbackground(BLACK);
	cprintf("        ");
	cprintf("        ");
	textbackground(RED);
	cprintf(" Explore a dark cave filled with ");
	textcolor(LIGHTCYAN);
	cprintf("TREASURE");
	textcolor(YELLOW);
	cprintf("... But... Be careful  ");
	textbackground(BLACK);
	cprintf("        ");
	cprintf("        ");
	textbackground(RED);
	cprintf(" for you are not alone... The cave houses a ");
	textcolor(GREEN);
	cprintf("TERRIBLE");
	textcolor(YELLOW);
	cprintf(" and ");
	textcolor(GREEN);
	cprintf("SMELLY ");
	textcolor(YELLOW);
	textbackground(BLACK);
	cprintf("        ");
	cprintf("        ");
	textbackground(RED);
	cprintf(" creature called ");
	textcolor(BROWN);
	cprintf("THE TERRIBLY SMELLY WUMPUS");
	textcolor(YELLOW);
	cprintf("... That and It is   ");
	textbackground(BLACK);
	cprintf("        ");
	cprintf("        ");
	textbackground(RED);
	cprintf(" filled with bottomless pits of ");
	textcolor(MAGENTA);
	cprintf("DEATH");
	textcolor(YELLOW);
	cprintf(", but don`t worry, you     ");
	textbackground(BLACK);
	cprintf("        ");
	cprintf("        ");
	textbackground(RED);
	cprintf(" came prepared.  Pretty ");
	textcolor(MAGENTA);
	cprintf("BADLY");
	textcolor(YELLOW);
	cprintf("... But prepared nonetheless.      ");
	textbackground(BLACK);
	cprintf("        ");
	cprintf("        ");
	textbackground(RED);
	cprintf("                                                                ");
	textbackground(BLACK);
	cprintf("        ");
	cprintf("        ");
	textbackground(RED);
	cprintf(" This game was created by ");
	textcolor(LIGHTCYAN);
	cprintf("Affonso Amendola");
	textcolor(YELLOW);
	cprintf(", a Brazilian         ");
	textbackground(BLACK);
	cprintf("        ");
	cprintf("        ");
	textbackground(RED);
	cprintf(" undergrad student at the ");
	textcolor(MAGENTA);
	cprintf("Institute of Astronomy, Geophysics    ");
	textbackground(BLACK);
	cprintf("        ");
	cprintf("        ");
	textbackground(RED);
	cprintf(" and Atmospheric Sciences");
	textcolor(YELLOW);
	cprintf(" of the University of Sao Paulo,       ");
	textbackground(BLACK);
	cprintf("        ");
	cprintf("        ");
	textbackground(RED);
	cprintf(" after finding an old book on ");
	textbackground(LIGHTGRAY);
	textcolor(RED);
	cprintf("V");
	textcolor(GREEN);
	cprintf("G");
	textcolor(BLUE);
	cprintf("A");
	textcolor(YELLOW);
	textbackground(RED);
	cprintf(" game development at ");
	textcolor(GREEN);
	cprintf("Garoa     ");
	textbackground(BLACK);
	cprintf("        ");
	cprintf("        ");
	textbackground(RED);
	cprintf(" Hacker Clube");
	textcolor(YELLOW);
	cprintf(", a hackerspace in Sao Paulo, and spending a       ");
	textbackground(BLACK);
	cprintf("        ");
	cprintf("        ");
	textbackground(RED);
	cprintf(" a sleepless weekend making it.                                 ");
	textbackground(BLACK);
	cprintf("        ");
	cprintf("        ");
	textbackground(RED);
	cprintf(" Thank you so much for playing. I hope you had fun.             ");
	textbackground(BLACK);
	cprintf("        ");
	cprintf("        ");
	textbackground(RED);
	cprintf("                                                                ");
	textbackground(BLACK);
	cprintf("        ");
	cprintf("        ");
	textbackground(RED);
	cprintf(" Affonso Amendola, affonso.gino.neto@usp.br                     ");
	textbackground(BLACK);
	cprintf("        ");
	cprintf("        ");
	textbackground(RED);
	cprintf(" http://www.github.com/affonsoamendola/wumpusvga                ");
	textbackground(BLACK);
	cprintf("        ");
	cprintf("        ");
	textbackground(RED);
	cprintf(" 1:21 AM, 11 July 2018, Sao Paulo, Brazil, Earth                ");
	textbackground(BLACK);
	cprintf("        ");
	cprintf("        ");
	textbackground(RED);
	cprintf("                                                                ");
	textbackground(BLACK);
	cprintf("        ");
	cprintf("        ");
	textbackground(BLACK);
	cprintf("                                                                ");
}

void after_gameover()
{
	unsigned char* color_temp;
        while(!kbhit())
        {
                delay(50);
                color_pallette = malloc(255*3);
                color_temp = malloc(1*3);
                get_pallette(color_pallette,1,255);
                get_pallette(color_temp,0,0);
                set_pallette(color_pallette,0,254);
                set_pallette(color_temp,255,255);
                free(color_pallette);
                free(color_temp);
        }
        set_graphics_mode(TEXT_MODE);
        print_order_info();
}
