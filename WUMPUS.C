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
#include <memory.h>
#include <math.h>
#include <time.h>

#include <conio.h>
#include <io.h>

#include <VGA.H>
#include <FONFLIB.H>

#define PI 3.14159265

int boardPosX = 10;
int boardPosY = 10;
#define boardSizeX 29
#define boardSizeY 19

int boardSquareSize = 10;

int boardBorderColor = 30;
int playerColor = 40;
int fogColor = 20;
int groundColor = 24;

int playerPosX =5;
int playerPosY =5;

int wumpusPosX;
int wumpusPosY;

int arrowPosX = 0;
int arrowPosY = 0;

char scoreChar[8];

int secretExitWall;
int secretExitX;
int secretExitY;

int goldPosX;
int goldPosY;

int difficulty = 1;

int holeNumber = 3;

char key;

int wumpus_distance = 0;
int wumpus_distance_state = 0;

int score = 0;

int showWumpus =0;
int showGold =0;
int showHoles =0;

int cheats = 0;

int safetyCounter = 0;

int visited[boardSizeX][boardSizeY];
int hole[boardSizeX][boardSizeY];

int current_frame_buffer_page = 1;
int current_draw_buffer_page = 0; 

int gameRunning = 1;

int x, y;
int i,j,k;

int state_flags = 0;
int game_turn_flags = 0;

#define STATE_ESCAPE 	 	(1<<0)
#define STATE_HAS_ARROW  	(1<<1)
#define STATE_WUMPUS_DEAD	(1<<2)
#define STATE_FELL 		 	(1<<3)
#define STATE_EATEN 	 	(1<<4)
#define STATE_WIN		 	(1<<5)
#define STATE_GAME_OVER   	(1<<6)

#define FLAG_STENCH		 	(1<<0)
#define FLAG_DRAFT		 	(1<<1)
#define FLAG_OVER_GOLD   	(1<<2)
#define FLAG_OVER_WUMPUS 	(1<<3)
#define FLAG_OVER_ARROW	 	(1<<4)

#define KEY_UP			'u'
#define KEY_RIGHT		'k'
#define KEY_DOWN		'j'
#define KEY_LEFT		'h'
#define KEY_GET			'g'
#define KEY_FIRE		'f'
#define KEY_CHEAT1		'z'
#define KEY_CHEAT2		'x'
#define KEY_QUIT		'q'

#define DIRECTION_UP	 0
#define DIRECTION_RIGHT	 1
#define DIRECTION_DOWN	 2
#define DIRECTION_LEFT	 3

unsigned char* color_pallette;
unsigned char* color_temp;

unsigned char far * title_mem_location = 0xA000E100L;
unsigned char far * sprites_mem_location = 0xA000FC00L;
		
void print_order_info();

void draw_screen();

void wumpusAI();

void draw_end_screen();

void update_logic();

void show_message_box(char * line, int posY, int color)
{
	fill_rectangle( 0, SCREEN_WIDTH, posY, posY+12, 0);
	print_string( 10, posY+2, color, line, 1);
}

void move_player(int direction)
{
	int lastPositionX = playerPosX;
	int lastPositionY = playerPosY;

	switch(direction)
	{
		case DIRECTION_UP:
			playerPosY--;
			break;
		case DIRECTION_RIGHT:
			playerPosX++;
			break;
		case DIRECTION_DOWN:
			playerPosY++;
			break;
		case DIRECTION_LEFT:
			playerPosX--;
			break;
		default:
			break;
	}

	if(	playerPosX == 0 || playerPosX == boardSizeX-1 ||
		playerPosY == 0 || playerPosY == boardSizeY-1)
	{
		if((state_flags & STATE_ESCAPE != 0) && ( ((secretExitWall == DIRECTION_UP || secretExitWall == DIRECTION_DOWN) && playerPosX == secretExitX) ||
						   				  		 ((secretExitWall == DIRECTION_LEFT || secretExitWall == DIRECTION_RIGHT) && playerPosY == secretExitY)))
		{
			score += 1000;
			state_flags |= STATE_GAME_OVER;
			state_flags |= STATE_WIN;
		}
		else 
		{
			playerPosX = lastPositionX;
			playerPosY = lastPositionY;
		}
	}

	if(visited[playerPosX][playerPosY] == 0)
	{
		score += 10*difficulty;
		visited[playerPosX][playerPosY] = 1;
	}
}

void fire_arrow(int direction)
{
	int hitMonster = 0;
	int dropPositionX = -1;
	int dropPositionY = -1;

	switch(direction)
	{
		case DIRECTION_UP:
			if(	wumpusPosY < playerPosY &&
				wumpusPosX == playerPosX)
			{
				hitMonster = 1;
			}
			else
			{
				dropPositionX = playerPosX;
				dropPositionY = 1;
			}
			break;

		case DIRECTION_RIGHT:
			if(	wumpusPosX > playerPosX &&
				wumpusPosY == playerPosY)
			{
				hitMonster = 1;
			}
			else
			{
				dropPositionY = playerPosY;
				dropPositionX = boardSizeX-2;
			}
			break;

		case DIRECTION_DOWN:
			if(	wumpusPosY > playerPosY &&
				wumpusPosX == playerPosX)
			{
				hitMonster = 1;
			}
			else
			{
				dropPositionX = playerPosX;
				dropPositionY = boardSizeY-2;
			}
			break;
		case DIRECTION_LEFT:
			if(	wumpusPosX < playerPosX &&
				wumpusPosY == playerPosY)
			{
				hitMonster = 1;
			}
			else
			{
				dropPositionY = playerPosY;
				dropPositionX = 1;
			}
			break;
		default:
			break;
	}


	show_message_box(	"YOU SEE YOUR ARROW HEAD INTO THE DARK",
							( (SCREEN_HEIGHT >> 1) - ((8+4) >> 1) ), 
							40);
	getch();
	arrowPosX = dropPositionX;
	arrowPosY = dropPositionY;

	state_flags &= ~STATE_HAS_ARROW;

	if(hitMonster && (state_flags & STATE_WUMPUS_DEAD) == 0)
	{
		state_flags |= STATE_WUMPUS_DEAD;
		show_message_box(	"YOU HEAR A HORRIFYING SCREAM",
						( (SCREEN_HEIGHT >> 1) - ((8+4) >> 1) ), 
						40);
		getch();
		arrowPosX = wumpusPosX;
		arrowPosY = wumpusPosY;
	}
}

void get_gold()
{
	score += 500;
	goldPosX = -1;
	goldPosY = -1;
	
	if(state_flags & STATE_WUMPUS_DEAD == 0)
	{
		show_message_box(	"YOU HEAR THE MIGHTY ROAR OF THE WUMPUS",
						( (SCREEN_HEIGHT >> 1) - ((8+4) >> 1) ), 
						40);
		getch();
	}
	
	show_message_box(	"A SECRET DOOR OPENS NEARBY",
						( (SCREEN_HEIGHT >> 1) - ((8+4) >> 1) ), 
						40);
	getch();
	
	state_flags |= STATE_ESCAPE;	
}

void init_system()
{
	set_graphics_mode(GRAPHICS_MODEX);

	draw_page(0);
	frame_page(0);
	color_pallette = malloc(256*3);

	get_pallette(color_pallette, 0, 255);
	load_pallette("plt/TITLE.PLT", 12);
	load_pallette("plt/SPRITES.PLT", 27);

	load_pgm("graphix/title.pgm", title_mem_location, 216, 124);
	load_pgm("graphix/sprites.pgm", sprites_mem_location, 60, 40);

	srand(time(NULL));

	
	
	state_flags = 0;
	state_flags |= STATE_HAS_ARROW;

	game_turn_flags = 0;	
}

void init_board()
{
	secretExitWall = rand()%4;
	secretExitX = rand()%(boardSizeX-2)+1;
	secretExitY = rand()%(boardSizeY-2)+1;

	wumpusPosX=rand()%(boardSizeX-2)+1;
	wumpusPosY=rand()%(boardSizeY-2)+1;

	while(safetyCounter<10000)
	{
		x=rand()%(boardSizeX-2)+1;
		y=rand()%(boardSizeY-2)+1;
		if(x!=playerPosX || y!=playerPosY)
		{
			wumpusPosX=x;
			wumpusPosY=y;
			break;
		}
		safetyCounter++;
	}

	safetyCounter = 0;

	while(safetyCounter<10000)
	{
		x=rand()%(boardSizeX-2)+1;
		y=rand()%(boardSizeY-2)+1;
		if((x!=wumpusPosX || y!=wumpusPosY)&&
		   (x!=playerPosX || y!=playerPosY))
		{
			goldPosX=x;
			goldPosY=y;
			break;
		}
		safetyCounter++;
	}
	safetyCounter = 0;

	for(x=1;x<boardSizeX-1;x++)
	{
		for(y=1;y<boardSizeY-1;y++)
		{
			visited[x][y]=0;
			hole[x][y]=0;
			if(x==playerPosX && y==playerPosY)
			{
				visited[x][y]=1;
			}
		}
	}

	while(holeNumber>0 && safetyCounter < 10000)
	{
		x=rand()%(boardSizeX-4)+2;
		y=rand()%(boardSizeY-4)+2;

		if((x!=wumpusPosX || y!= wumpusPosY) &&
		   (x!=goldPosX || y!= goldPosY) &&
		   (x!=playerPosX || y!= playerPosY) &&
		   hole[x][y]==0)
		{
			hole[x][y]=1;
			holeNumber--;
		}

		safetyCounter++;
	}

	update_logic();
}

void show_title()
{
	draw_page(0);
	frame_page(1);
	
	copy_vmem_to_dbuffer(	title_mem_location, 
							(SCREEN_WIDTH>>1)-(216>>1), 
							(SCREEN_HEIGHT>>1)-(124>>1), 
							0, 216, 
							0, 124, 
							216);

	print_string((SCREEN_WIDTH>>1)-100,227,40,"By Affonso Amendola, 2018",1);
	
	frame_page(0);

	getch();

	fill_screen(0);
}

void handle_input()
{
	key = getch();

	switch(key)
	{
		case KEY_QUIT:
			state_flags |= STATE_GAME_OVER;
			break;

		case KEY_CHEAT1:
			if(cheats == 1)
			{
				showWumpus = !showWumpus; 
				showGold = !showGold; 
				showHoles = !showHoles;
			}
			break;

		case KEY_CHEAT2:
			if(cheats == 1)
			{
				state_flags |= STATE_ESCAPE;
			}
			break;

		case KEY_UP:
			move_player(DIRECTION_UP);
			break;

		case KEY_RIGHT:
			move_player(DIRECTION_RIGHT);
			break;

		case KEY_DOWN:
			move_player(DIRECTION_DOWN);
			break;

		case KEY_LEFT:
			move_player(DIRECTION_LEFT);
			break;

		case KEY_GET:
			if(	(game_turn_flags & FLAG_OVER_GOLD) != 0)
			{
				get_gold();
			}

			if( (game_turn_flags & FLAG_OVER_ARROW) != 0)
			{
				state_flags |= STATE_HAS_ARROW;
				arrowPosX = -1;
				arrowPosY = -1;
			}
			break;

		case KEY_FIRE:
			if( (state_flags & STATE_HAS_ARROW) != 0)
			{
				show_message_box(	"FIRE IN WHAT DIRECTION?",
								( 	(SCREEN_HEIGHT >> 1) - ((8+4) >> 1) ), 
									40);
				key = getch();

				switch(key)
				{
					case KEY_UP:
						fire_arrow(DIRECTION_UP);
						break;
					case KEY_RIGHT:
						fire_arrow(DIRECTION_RIGHT);
						break;
					case KEY_DOWN:
						fire_arrow(DIRECTION_DOWN);
						break;
					case KEY_LEFT:
						fire_arrow(DIRECTION_LEFT);
						break;
				}
			}
			break;
	}
}

int main()
{
	init_system();

	show_title();

	show_message_box(	"CHOOSE YOUR CHALLENGE LEVEL:", 
						(SCREEN_HEIGHT >> 1) - 12,
						40);

	show_message_box(	"E:EASY  M:MEDIUM  H:HARD  U:ULTRA", 
						(SCREEN_HEIGHT >> 1),
						40);

	key = 'o';
	while(!(key == 'e' || key=='m' || key=='h'|| key=='u' ||
			key == 'E' || key=='M' || key=='H'|| key=='U'))
	{
		
		key = getch();
		if(key=='e'||key=='E'){difficulty=1;holeNumber=6;}
		if(key=='m'||key=='M'){difficulty=2;holeNumber=10;}
		if(key=='h'||key=='H'){difficulty=3;holeNumber=18;}
		if(key=='u'||key=='U'){difficulty=4;holeNumber=34;}
		if(key=='c')
		{
			cheats = 1;

			show_message_box(	"YOU DIRTY CHEATER", 
								SCREEN_HEIGHT - 20,
								40);
			show_message_box(	"Z:TOGGLE VIEW  X:GOTO ESCAPE", 
								SCREEN_HEIGHT - 10,
								40);
		}
	}

	init_board();

	while ((state_flags & STATE_GAME_OVER) == 0)
	{	
		while(!kbhit())
		{
			draw_screen();
		}	
		handle_input();
		update_logic();
	}

	draw_end_screen();

	set_graphics_mode(TEXT_MODE);
	print_order_info();
}

void draw_end_screen()
{
	fill_screen(0);
	flip_draw_page();
	load_pallette("plt/RAINBOW.PLT", 256);

	for(x=0;x<320;x++)
	{
		for(y=0;y<240;y++)
		{
			set_pixel(x,y,(int)( ((float)(y)*(float)(x)) * (255.0 / (320.0*240.0) )  ));
		}
	}

	fill_rectangle(0,320,84,134,150);

	if((state_flags & STATE_EATEN) != 0)
	{
		print_string(10,86,30,"YOU GOT EATEN BY THE WUMPUS",1);
	}
	else if((state_flags & STATE_FELL) != 0)
	{
		print_string(10,86,30,"YOU FELL IN A HOLE AND DIED",1);
	}
	
	if((state_flags & STATE_WIN) != 0)
	{
		print_string(10,86,30,"YOU WIN! CONGRATULATIONS!",1);
	}
	print_string(10,96,30,"GAME OVER",1);
	print_string(10,106,30,"THANK YOU FOR PLAYING",1);
	print_string(10,116,30,"YOUR SCORE:",1);
	print_string(104,116,30,inttostring(score,scoreChar),1);
	print_string(10,126,30,"PRESS ANY KEY TO RETURN TO DOS",1);
	
	frame_page(current_draw_buffer_page);

	color_pallette = malloc(255*3);
	color_temp = malloc(1*3);

	while(!kbhit())
	{

		delay(50);	
		
		get_pallette(color_pallette,1,255);	
		get_pallette(color_temp,0,0);
		set_pallette(color_pallette,0,254);
		set_pallette(color_temp,255,255);
		
	}

	free(color_pallette);
	free(color_temp);
}

void update_logic()
{
	game_turn_flags = 0;

	if(	(state_flags & STATE_ESCAPE) 		!= 0 && 
		(state_flags & STATE_WUMPUS_DEAD) 	== 0)
	{
			wumpusAI();
	}

	if(	(playerPosX+1 == wumpusPosX && playerPosY == wumpusPosY) ||
	 	(playerPosX-1 == wumpusPosX && playerPosY == wumpusPosY) ||
 		(playerPosY+1 == wumpusPosY && playerPosX == wumpusPosX) ||
	    (playerPosY-1 == wumpusPosY && playerPosX== wumpusPosX))
		
		{
			game_turn_flags |= FLAG_STENCH;
		}

	wumpus_distance = abs(playerPosX-wumpusPosX) + abs(playerPosY-wumpusPosY);
	
	if(wumpus_distance <3)
	{
		wumpus_distance_state = 0;
	}
	else if(wumpus_distance <6)
	{
		wumpus_distance_state = 1;
	}
	else if(wumpus_distance <12)
	{
		wumpus_distance_state = 2;
	}
	else if(wumpus_distance <22)
	{
		wumpus_distance_state = 3;
	}else
	{
		wumpus_distance_state = 4;
	}

	if(	playerPosX == goldPosX &&
		playerPosY == goldPosY)
	{
		game_turn_flags |= FLAG_OVER_GOLD;
	}

	if(	hole[playerPosX+1][playerPosY] == 1 ||
	   	hole[playerPosX-1][playerPosY] == 1 ||
	   	hole[playerPosX][playerPosY+1] == 1 ||
	   	hole[playerPosX][playerPosY-1] == 1)
	{
		game_turn_flags |= FLAG_DRAFT;
	}

	if(	playerPosX == wumpusPosX && 
		playerPosY == wumpusPosY)
	{
		game_turn_flags |= FLAG_OVER_WUMPUS;
	}

	if(playerPosX==arrowPosX && playerPosY==arrowPosY)
	{
		game_turn_flags |=  FLAG_OVER_ARROW;
	}

	if(hole[playerPosX][playerPosY] == 1)
	{
		state_flags |= STATE_GAME_OVER;
		state_flags |= STATE_FELL;
		score -= 200; 	
	}

	if(	(game_turn_flags & FLAG_OVER_WUMPUS) != 0 &&
		(state_flags & STATE_WUMPUS_DEAD) == 0 )
	{
		state_flags |= STATE_GAME_OVER;
		state_flags |= STATE_EATEN;
		score -= 300;
	}
}

void wumpusAI()
{
	int xDistance;
	int yDistance;

	xDistance = playerPosX-wumpusPosX;
	yDistance = playerPosY-wumpusPosY;

	if(abs(xDistance)>abs(yDistance))
	{
		if(wumpusPosX>playerPosX)
		{
			wumpusPosX-=1;
		}
		else
		{
			wumpusPosX+=1;
		}

	}
	else
	{
		if(wumpusPosY>playerPosY)
		{
			wumpusPosY-=1;
		}
		else
		{
			wumpusPosY+=1;
		}
	}
}

void draw_square(int posX, int posY, int color)
{
	fill_rectangle(          (posX * boardSquareSize ) + boardPosX, 
						(((posX+1) * boardSquareSize) - 1 ) + boardPosX,
					         (posY * boardSquareSize ) + boardPosY, 
						(((posY+1) * boardSquareSize) - 1 ) + boardPosY,
						color);
}

void draw_screen()
{
	flip_draw_page();
	
	fill_screen(0);
	print_string(10,1,40,"FOFONSO`S WUMPUS HUNT",1);
	print_string(320-10-8*8,1,40,inttostring(score,scoreChar),1);

	//DRAW BACKGROUND
	for(x = 0; x < boardSizeX; x++)
	{
		for(y = 0; y < boardSizeY; y++)
		{

			if(x == 0 || y == 0 || 
			   x == boardSizeX-1 || y == boardSizeY-1 )
			{

				//WALLS
				//TOP LEFT WALL
				
				if(x==0 && y==0)
				{
					copy_vmem_to_dbuffer(   sprites_mem_location, 
				                            boardPosX + x * boardSquareSize, 
				                            boardPosY + y * boardSquareSize, 
				                            0,
				                            9,
				                            10,
				                            19,
				                            60);

				}
				//------------
				//LEFT WALL

				if(x==0 && (y>0 && y<boardSizeY-1))
				{
					
					if(secretExitWall == DIRECTION_LEFT && y == secretExitY && (state_flags & STATE_ESCAPE))
					{
						copy_vmem_to_dbuffer(   sprites_mem_location, 
				                            boardPosX + x * boardSquareSize, 
				                            boardPosY + y * boardSquareSize, 
				                            30,
				                            39,
				                            20,
				                            29,
				                            60);
					}
					else
					{
						copy_vmem_to_dbuffer(   sprites_mem_location, 
				                            boardPosX + x * boardSquareSize, 
				                            boardPosY + y * boardSquareSize, 
				                            0,
				                            9,
				                            20,
				                            29,
				                            60);
					}
				}
				//------------
				//BOTTOM LEFT 

				if(x==0 && y==(boardSizeY-1))
				{
					copy_vmem_to_dbuffer(   sprites_mem_location, 
				                            boardPosX + x * boardSquareSize, 
				                            boardPosY + y * boardSquareSize, 
				                            0,
				                            9,
				                            30,
				                            39,
				                            60);

				}
				//------------
				//TOP
				if(y==0 && (x>0 && x<boardSizeX-1))
				{

					if(secretExitWall == DIRECTION_UP && x == secretExitX && (state_flags & STATE_ESCAPE))
					{
						copy_vmem_to_dbuffer(   sprites_mem_location, 
				                            boardPosX + x * boardSquareSize, 
				                            boardPosY + y * boardSquareSize, 
				                            30,
				                            39,
				                            20,
				                            29,
				                            60);
					}
					else
					{
						copy_vmem_to_dbuffer(   sprites_mem_location, 
				                            boardPosX + x * boardSquareSize, 
				                            boardPosY + y * boardSquareSize, 
				                            10,
				                            19,
				                            10,
				                            19,
				                            60);
					}
					
				}
				//------------
				//BOTTOM
				if(y==(boardSizeY-1) && (x>0 && x<boardSizeX-1))
				{

					if(secretExitWall == DIRECTION_DOWN && x == secretExitX && (state_flags & STATE_ESCAPE))
					{
						copy_vmem_to_dbuffer(   sprites_mem_location, 
				                            boardPosX + x * boardSquareSize, 
				                            boardPosY + y * boardSquareSize, 
				                            30,
				                            39,
				                            20,
				                            29,
				                            60);
					}
					else
					{
						copy_vmem_to_dbuffer(   sprites_mem_location, 
					                            boardPosX + x * boardSquareSize, 
					                            boardPosY + y * boardSquareSize, 
					                            10,
					                            19,
					                            30,
					                            39,
					                            60);
					}

				}

				//------------
				//TOP RIGHT

				if(x==(boardSizeX-1) && y==0)
				{
					copy_vmem_to_dbuffer(   sprites_mem_location, 
				                            boardPosX + x * boardSquareSize, 
				                            boardPosY + y * boardSquareSize, 
				                            20,
				                            29,
				                            10,
				                            19,
				                            60);

				}

				//------------
				//RIGHT

				if(x==(boardSizeX-1) && (y>0 && y<boardSizeY-1))
				{
					if(secretExitWall == DIRECTION_RIGHT && y == secretExitY && (state_flags & STATE_ESCAPE))
					{
						copy_vmem_to_dbuffer(   sprites_mem_location, 
				                            boardPosX + x * boardSquareSize, 
				                            boardPosY + y * boardSquareSize, 
				                            30,
				                            39,
				                            20,
				                            29,
				                            60);
					}
					else
					{
						copy_vmem_to_dbuffer(   sprites_mem_location, 
				                            boardPosX + x * boardSquareSize, 
				                            boardPosY + y * boardSquareSize, 
				                            20,
				                            29,
				                            20,
				                            29,
				                            60);
					}
				}

				//------------
				//BOTTOM RIGHT

				if(x==(boardSizeX-1) && y==(boardSizeY-1))
				{
					copy_vmem_to_dbuffer(   sprites_mem_location, 
				                            boardPosX + x * boardSquareSize, 
				                            boardPosY + y * boardSquareSize, 
				                            20,
				                            29,
				                            30,
				                            39,
				                            60);

				}
			
			}
			else if(x > 0 && x < boardSizeX-1 &&
					y > 0 && y < boardSizeY-1)
			{

				if(visited[x][y] == 1)
				{
					copy_vmem_to_dbuffer(   sprites_mem_location, 
				                            boardPosX + x * boardSquareSize, 
				                            boardPosY + y * boardSquareSize, 
				                            10,
				                            19,
				                            20,
				                            29,
				                            60);
				}
				else 
				{
					copy_vmem_to_dbuffer(   sprites_mem_location, 
				                            boardPosX + x * boardSquareSize, 
				                            boardPosY + y * boardSquareSize, 
				                            30,
				                            39,
				                            20,
				                            29,
				                            60);
				}

				if(hole[x][y] == 1 && showHoles == 1)
				{
					draw_square(x, y, 40);
				}

				if(x == wumpusPosX && y == wumpusPosY && showWumpus == 1)
				{
					draw_square(x, y, 46);
				}

				if(x == goldPosX && y == goldPosY && showGold == 1)
				{
					draw_square(x, y, 43);
				}
			}
		}
	}

	//DRAW PLAYER

	copy_vmem_to_dbuffer(   sprites_mem_location, 
                            boardPosX + playerPosX * boardSquareSize, 
                            boardPosY + playerPosY * boardSquareSize, 
                            0,
                            9,
                            0,
                            9,
                            60);

	if(game_turn_flags & FLAG_DRAFT)
	{
		show_message_box(	"YOU FEEL A DRAFT", 
							SCREEN_HEIGHT-12,
							40);
	}

	if(game_turn_flags & FLAG_OVER_ARROW)
	{
		show_message_box(	"YOU SEE AN ARROW ON THE GROUND", 
							SCREEN_HEIGHT-12,
							40);
	}

	if(game_turn_flags & FLAG_STENCH)
	{
		show_message_box(	"YOU SMELL A HORRIBLE STENCH", 
							SCREEN_HEIGHT-24,
							40);
	}

	if(game_turn_flags & FLAG_OVER_WUMPUS)
	{
		show_message_box(	"HERE LIES A DEAD WUMPUS", 
							SCREEN_HEIGHT-24,
							40);
	}

	if(game_turn_flags & FLAG_OVER_GOLD)
	{
		show_message_box(	"YOU SEE A GLINT OF LIGHT", 
							SCREEN_HEIGHT-36,
							40);
	}
	if((state_flags & STATE_ESCAPE) && !(state_flags & STATE_WUMPUS_DEAD))
	{
		switch(wumpus_distance_state)
		{
			case 0:
				show_message_box(	"YOU HEAR DEATH RIGHT BESIDE YOU", 
									SCREEN_HEIGHT-36,
									40);
				break;

			case 1:
				show_message_box(	"YOU HEAR THE WUMPUS CLOSING IN", 
									SCREEN_HEIGHT-36,
									40);
				break;

			case 2:
				show_message_box(	"YOU HEAR GROWLING A SHORT DISTANCE AWAY", 
									SCREEN_HEIGHT-36,
									40);
				break;

			case 3:
				show_message_box(	"YOU HEAR LOUD NOISES IN THE DISTANCE", 
									SCREEN_HEIGHT-36,
									40);
				break;

			default:
				show_message_box(	"", 
									SCREEN_HEIGHT-36,
									40);
				break;
		}
	}

	frame_page(current_draw_buffer_page);
}

void print_order_info()
{
	int i =0;

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





