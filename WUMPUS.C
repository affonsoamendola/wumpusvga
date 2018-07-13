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

#include <RENDERER.H>

#define PI 3.14159265

int boardPosX = 10;
int boardPosY = 10;
#define boardSizeX 35
#define boardSizeY 19

int boardSquareSize = 8;

int boardBorderColor = 30;
int playerColor = 40;
int visitedColor = 20;

int playerPosX =5;
int playerPosY =5;

int wumpusPosX;
int wumpusPosY;

int arrowPosX = 0;
int arrowPosY = 0;
int hasArrow = 1;

char scoreChar[8];

int secretExitWall;
int secretExitX;
int secretExitY;

int goldPosX;
int goldPosY;

int difficulty = 1;

int holeNumber = 3;

char key;

int wumpusDistance = 0;

int score = 0;
int escape = 0;

void drawScreen();

void wumpusAI();

int showWumpus =0;
int showGold =0;
int showHoles =0;

int cheats = 0;

int safetyCounter = 0;

int visited[boardSizeX][boardSizeY];
int hole[boardSizeX][boardSizeY];

void print_order_info();

int main()
{

	int gameRunning = 1;
	int x, y;
	int eatenByWumpus = 0;
	int fellInHole = 0;
	int win = 0;
	int wumpusDead = 0;

	set_graphics_mode(GRAPHICS_MODE13);

	fill_rectangle(10,320,95,105,0);
	print_string(10,96,40,"CHOOSE YOUR CHALLENGE LEVEL:",1);
	print_string(10,106,40,"E:EASY  M:MEDIUM  H:HARD  U:ULTRA",1);
	key = 'o';
	while(!(key=='e'||key=='m'||key=='h'||key=='u'))
	{
		key = getch();
		if(key=='e'){difficulty=1;holeNumber=6;}
		if(key=='m'){difficulty=2;holeNumber=10;}
		if(key=='h'){difficulty=3;holeNumber=18;}
		if(key=='u'){difficulty=4;holeNumber=34;}
		if(key=='c')
		{
			cheats = 1;
			print_string(10,200-20,40,"YOU DIRTY CHEATER",1);
			print_string(10,200-10,40,"Z:TOGGLE VIEW  X:GOTO ESCAPE",1);

		}
	}
	srand(time(NULL));

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
		x=rand()%(boardSizeX-3)+2;
		y=rand()%(boardSizeY-3)+2;

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
	while (gameRunning)
	{
		fill_screen(0);

		print_string(10,1,40,"FOFONSO`S WUMPUS HUNT",1);
		print_string(320-10-8*8,1,40,inttostring(score,scoreChar),1);
		drawScreen(visited);
		if((playerPosX+1==wumpusPosX&&playerPosY==wumpusPosY) ||
		   (playerPosX-1==wumpusPosX&&playerPosY==wumpusPosY) ||
		   (playerPosY+1==wumpusPosY&&playerPosX==wumpusPosX) ||
		   (playerPosY-1==wumpusPosY&&playerPosX==wumpusPosX))
		{
			print_string(10,200-9,40,"YOU SMELL A HORRIBLE STENCH",1);
		}
		if(escape==1&&wumpusDead==0)
		{
			wumpusDistance = abs(playerPosX-wumpusPosX)+abs(playerPosY-wumpusPosY);
			if(wumpusDistance <2)
			{
				print_string(10,200-18,40,"YOU HEAR DEATH RIGHT BESIDE YOU",1);
			}else
			if(wumpusDistance <6)
			{
				print_string(10,200-18,40,"YOU HEAR THE WUMPUS CLOSING IN",1);
			}else
			if(wumpusDistance <12)
			{
				print_string(10,200-18,40,"YOU HEAR GROWLING A SHORT DISTANCE AWAY",1);
			}
			else
			if(wumpusDistance <22)
			{
				print_string(10,200-18,40,"YOU HEAR LOUD NOISES IN THE DISTANCE",1);
			}
		}
		if(playerPosX==goldPosX&&playerPosY==goldPosY)
		{
			print_string(10,200-18,40,"YOU SEE A GLINT OF LIGHT",1);
		}
		if(hole[playerPosX+1][playerPosY]==1 ||
		   hole[playerPosX-1][playerPosY]==1 ||
		   hole[playerPosX][playerPosY+1]==1 ||
		   hole[playerPosX][playerPosY-1]==1)
		{
			print_string(10,200-27,40,"YOU FEEL A DRAFT",1);
		}

		if(playerPosX==wumpusPosX&&playerPosY==wumpusPosY &&wumpusDead==1)
		{
			print_string(10,200-9,40,"HERE LIES A DEAD WUMPUS",1);
		}
		if(playerPosX==arrowPosX && playerPosY==arrowPosY)
		{
			print_string(10,200-27,40,"THERE IS AN ARROW ON THE GROUND",1);
		}

		key = getch();


		if(key=='q') gameRunning = 0;
		if(key=='z'&&cheats==1) {showWumpus = !showWumpus; showGold = !showGold; showHoles = !showHoles;}
		if(key=='x'&&cheats==1)escape=1;
		if(key=='u')
		{
			if(playerPosY>1)
			{
				playerPosY-=1;
				if(visited[playerPosX][playerPosY]==0)score+=10*difficulty;
			}else
			if(secretExitWall==0 && playerPosX==secretExitX && escape == 1)
			{
				score+=1000;
				gameRunning=0;
				win = 1;
			}
		}
		if(key=='j')
		{
			if(playerPosY<(boardSizeY-1))
			{
				playerPosY+=1;
				if(visited[playerPosX][playerPosY]==0)score+=10*difficulty;
			}else
			if(secretExitWall==2 && playerPosX == secretExitX && escape == 1)
			{
				score+=1000;
				gameRunning=0;
				win=1;
			}
		}
		if(key=='h')
		{
			if(playerPosX>1)
			{
				playerPosX-=1;
				if(visited[playerPosX][playerPosY]==0)score+=10*difficulty;
			}else
			if(secretExitWall==1 && playerPosY == secretExitY && escape == 1)
			{
				score+=1000;
				gameRunning=0;
				win=1;
			}
		}
		if(key=='k')
		{
			if(playerPosX<(boardSizeX-1))
			{
				playerPosX+=1;
				if(visited[playerPosX][playerPosY]==0)score+=10*difficulty;
			}else
			if(secretExitWall==3&&playerPosY==secretExitY && escape == 1)
			{
				score+=1000;
				gameRunning=0;
				win=1;
			}
		}
		visited[playerPosX][playerPosY]=1;
		if(key=='g'&& playerPosX==goldPosX && playerPosY==goldPosY)
		{
			score+=500;
			goldPosX=-1;
			goldPosY=-1;
			fill_rectangle(10,320,95,105,0);
			print_string(10,96,40,"YOU HEAR THE MIGTHY ROAR OF THE WUMPUS",1);
			getch();
			fill_rectangle(10,320,95,105,0);
			print_string(10,96,40,"A SECRET DOOR OPENS NEARBY",1);
			getch();
			escape=1;
		}
		if(key=='g'&& playerPosX==arrowPosX && playerPosY==arrowPosY)
		{
			hasArrow = 1;
			arrowPosX = -1;
			arrowPosY = -1;
		}
		if(key=='f' && hasArrow ==1)
		{
			fill_rectangle(10,320,95,105,0);
			print_string(10,96,40,"FIRE IN WHAT DIRECTION?",1);
			key = getch();
			if(key=='k')
			{
				hasArrow=0;
				if(playerPosY==wumpusPosY &&playerPosX<wumpusPosX &&wumpusDead==0)
				{
					wumpusDead = 1;
					fill_rectangle(10,320,95,105,0);
					print_string(10,96,40,"YOU HEAR A HORRIFYING SCREAM",1);
					getch();
					arrowPosX = wumpusPosX;
					arrowPosY = wumpusPosY;
					score+=1000;
				}
				else
				{
					arrowPosX = boardSizeX-1;
					arrowPosY = playerPosY;
					score-=100;
				}
			}
			if(key=='h')
			{
				hasArrow=0;
				if(playerPosY==wumpusPosY && playerPosX>wumpusPosX&&wumpusDead==0)
				{
					wumpusDead = 1;
					fill_rectangle(10,320,95,105,0);
					print_string(10,96,40,"YOU HEAR A HORRIFYING SCREAM",1);
					getch();
					arrowPosX = wumpusPosX;
					arrowPosY = wumpusPosY;
					score+=1000;
				}
				else
				{
					arrowPosX = 1;
					arrowPosY = playerPosY;
					score-=100;
				}
			}
			if(key=='u')
			{
				hasArrow=0;
				if(playerPosX==wumpusPosX && playerPosY>wumpusPosY&&wumpusDead==0)
				{
					wumpusDead = 1;
					fill_rectangle(10,320,95,105,0);
					print_string(10,96,40,"YOU HEAR A HORRIFYING SCREAM",1);
					getch();
					arrowPosX = wumpusPosX;
					arrowPosY = wumpusPosY;
					score+=1000;
				}
				else
				{
					arrowPosY = 1;
					arrowPosX = playerPosX;
					score-=100;
				}
			}
			if(key=='j')
			{
				hasArrow=0;
				if(playerPosX==wumpusPosX && playerPosY<wumpusPosY&&wumpusDead==0)
				{
					wumpusDead = 1;
					fill_rectangle(10,320,95,105,0);
					print_string(10,96,40,"YOU HEAR A HORRIFYING SCREAM",1);
					getch();
					arrowPosX = wumpusPosX;
					arrowPosY = wumpusPosY;
					score+=1000;
				}
				else
				{
					arrowPosY=boardSizeY-1;
					arrowPosX=playerPosX;
					score-=100;
				}
			}
		}
		if(hole[playerPosX][playerPosY]==1)
		{
			gameRunning=0;
			score-=200;
			fellInHole=1;
		}
		if(escape==1 && wumpusDead==0)
		{
			wumpusAI();
		}

		if(playerPosX==wumpusPosX&&playerPosY==wumpusPosY &&wumpusDead ==0)
		{
			gameRunning=0;
			score-=300;
			eatenByWumpus=1;
		}
	}

	for(x=0;x<320;x++)
	{
		for(y=0;y<200;y++)
		{
			set_pixel(x,y,(int)((x+y)*(256.0/(320.0+200.0))));
		}
	}

	fill_rectangle(0,320,84,134,0);
	if(eatenByWumpus)
	{
		print_string(10,86,40,"YOU GOT EATEN BY THE WUMPUS",1);
	}
	if(fellInHole)
	{
		print_string(10,86,40,"YOU FELL IN A HOLE AND DIED",1);
	}
	if(win)
	{
		print_string(10,86,40,"YOU WIN! CONGRATULATIONS!",1);
	}
	print_string(10,96,40,"GAME OVER",1);
	print_string(10,106,40,"THANK YOU FOR PLAYING",1);
	print_string(10,116,40,"YOUR SCORE:",1);
	print_string(104,116,40,inttostring(score,scoreChar),1);
	print_string(10,126,40,"PRESS ANY KEY TO RETURN TO DOS",1);

	getch();
	set_graphics_mode(TEXT_MODE);
	print_order_info();
	return 0;
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
		}else
		{
			wumpusPosX+=1;
		}
	}else
	{
		if(wumpusPosY>playerPosY)
		{
			wumpusPosY-=1;
		}else
		{
			wumpusPosY+=1;
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
						boardPosX+((x+1)*boardSquareSize)-1,
						boardPosY+(y*boardSquareSize),
						boardPosY+((y+1)*boardSquareSize)-1,
						boardBorderColor);

			}
			if(x==playerPosX && y==playerPosY)
			{
				fill_rectangle(	boardPosX+(x*boardSquareSize),
						boardPosX+((x+1)*boardSquareSize)-1,
						boardPosY+(y*boardSquareSize),
						boardPosY+((y+1)*boardSquareSize)-1,
						playerColor);
			}
			if(visited[x][y]==0 && (x>0&&y>0&&x<boardSizeX&&y<boardSizeY))
			{
				fill_rectangle(	boardPosX+(x*boardSquareSize),
						boardPosX+((x+1)*boardSquareSize)-1,
						boardPosY+(y*boardSquareSize),
						boardPosY+((y+1)*boardSquareSize)-1,
						visitedColor);
			}
			if((x==wumpusPosX && y==wumpusPosY && showWumpus==1) ||
			    x==wumpusPosX && y==wumpusPosY && visited[x][y]==1)
			{
				fill_rectangle(	boardPosX+(x*boardSquareSize),
						boardPosX+((x+1)*boardSquareSize)-1,
						boardPosY+(y*boardSquareSize),
						boardPosY+((y+1)*boardSquareSize)-1,
						150);
			}
			if(x==goldPosX && y==goldPosY && showGold==1)
			{
				fill_rectangle(	boardPosX+(x*boardSquareSize),
						boardPosX+((x+1)*boardSquareSize)-1,
						boardPosY+(y*boardSquareSize),
						boardPosY+((y+1)*boardSquareSize)-1,
						180);
			}
			if(hole[x][y]==1 && showHoles==1)
			{
				fill_rectangle(	boardPosX+(x*boardSquareSize),
						boardPosX+((x+1)*boardSquareSize)-1,
						boardPosY+(y*boardSquareSize),
						boardPosY+((y+1)*boardSquareSize)-1,
						200);
			}
			if(x==playerPosX && y==playerPosY)
			{
				fill_rectangle(	boardPosX+(x*boardSquareSize),
						boardPosX+((x+1)*boardSquareSize)-1,
						boardPosY+(y*boardSquareSize),
						boardPosY+((y+1)*boardSquareSize)-1,
						playerColor);
			}
		}
	}

	if(escape==1)
	{
		if(secretExitWall==0)
		{
			fill_rectangle( boardPosX + secretExitX*boardSquareSize,
					boardPosX + (secretExitX+1)*boardSquareSize -1,
					boardPosY,
					boardPosY + boardSquareSize-1,
					0);
		}
		if(secretExitWall==1)
		{
			fill_rectangle( boardPosX,
					boardPosX + boardSquareSize-1,
					boardPosY + secretExitY*boardSquareSize,
					boardPosY + (secretExitY+1)*boardSquareSize-1,
					0);
		}
		if(secretExitWall==2)
		{
			fill_rectangle( boardPosX + secretExitX*boardSquareSize,
					boardPosX + (secretExitX+1)*boardSquareSize -1,
					boardPosY + boardSizeY*boardSquareSize,
					boardPosY + (boardSizeY+1)*boardSquareSize-1,
					0);
		}
		if(secretExitWall==3)
		{
			fill_rectangle( boardPosX + boardSizeX*boardSquareSize,
					boardPosX + (boardSizeX+1)*boardSquareSize-1,
					boardPosY + secretExitY*boardSquareSize,
					boardPosY + (secretExitY+1)*boardSquareSize-1,
					0);
		}
	}
}

