#include <bios.h>
#include <dos.h>
#include <memory.h>
#include <math.h>
#include <time.h>

#include <RENDERER.H>

#define PI 3.14159265

int boardPosX = 10;
int boardPosY = 10;
#define boardSizeX 35
#define boardSizeY 19

#define holeChance 3

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

int secretExitWall;
int secretExitX;
int secretExitY;

int goldPosX;
int goldPosY;

int wumpusDistance=0;

int score = 0;
int escape = 0;

void drawScreen();

void wumpusAI();

int main()
{
	int visited[boardSizeX][boardSizeY];
	int hole[boardSizeX][boardSizeY];
	int gameRunning = 1;
	int x, y;
	int eatenByWumpus = 0;
	int fellInHole = 0;
	int win = 0;
	int wumpusDead = 0;

	srand(time(NULL));

	secretExitWall = rand()%4;
	secretExitX = rand()%(boardSizeX-2)+1;
	secretExitY = rand()%(boardSizeY-2)+1;

	for(x=0;x<boardSizeX;x++)
	{
		for(y=0;y<boardSizeY;y++)
		{
			visited[x][y]=0;
			hole[x][y]=0;
			if(x==playerPosX && y==playerPosY)
			{
				visited[x][y]=1;
			}

			if((rand()%100)<holeChance)
			{
				hole[x][y]=1;
			}
			//wumpusPosX=2;
			//wumpusPosY=2;
			//goldPosX=4;
			//goldPosY=4;
			wumpusPosX=(rand()%boardSizeX)+1;
			wumpusPosY=(rand()%boardSizeY)+1;
			goldPosX=(rand()%boardSizeX)+1;
			goldPosY=(rand()%boardSizeY)+1;
		}
	}
	set_graphics_mode(GRAPHICS_MODE13);
	while (gameRunning)
	{
		char key;
		char scoreChar[8];

		fill_screen(0);

		print_string(10,1,40,"FOFONSO`S WUMPUS HUNT",1);
		print_string(320-10-8*8,1,40,itoa(score,scoreChar),1);
		drawScreen(visited);
		if((playerPosX+1==wumpusPosX&&playerPosY==wumpusPosY) ||
		   (playerPosX-1==wumpusPosX&&playerPosY==wumpusPosY) ||
		   (playerPosY+1==wumpusPosY&&playerPosX==wumpusPosX) ||
		   (playerPosY-1==wumpusPosY&&playerPosX==wumpusPosX))
		{
			print_string(10,200-9,40,"YOU SMELL A HORRIBLE STENCH",1);
		}
		if(escape==1)
		{
			wumpusDistance = abs(playerPosX-wumpusPosX)+abs(playerPosY-wumpusPosY);
			if(wumpusDistance <2)
			{
				print_string(10,200-18,40,"YOU HEAR DEATH RIGHT BESIDE YOU",1);
			}else
			if(wumpusDistance <6)
			{
				print_string(10,200-18,40,"YOU HEAR SOUNDS OF THE WUMPUS CLOSING IN",1);
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
		if(key=='u')
		{
			if(playerPosY>1)
			{
				playerPosY-=1;
				if(visited[playerPosX][playerPosY]==0)score+=10;
			}else
			if(secretExitWall==0 && playerPosX==secretExitX)
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
				if(visited[playerPosX][playerPosY]==0)score+=10;
			}else
			if(secretExitWall==2 && playerPosX == secretExitX)
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
				if(visited[playerPosX][playerPosY]==0)score+=10;
			}else
			if(secretExitWall==1 && playerPosY == secretExitY)
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
				if(visited[playerPosX][playerPosY]==0)score+=10;
			}else
			if(secretExitWall==3&&playerPosY==secretExitY)
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
			goldPosX=0;
			goldPosY=0;
			fill_rectangle(10,320,95,105,0);
			print_string(10,96,40,"YOU HEAR THE MIGTHY ROAR OF THE WUMPUS",1);
			getch();
			fill_rectangle(10,320,95,105,0);
			print_string(10,96,40,"A SECRET DOOR OPENS NEARBY",1);
			escape=1;
		}
		if(key=='g'&& playerPosX==arrowPosX && playerPosY==arrowPosY)
		{
			hasArrow = 1;
			arrowPosX = 0;
			arrowPosY = 0;
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
		if(playerPosX==wumpusPosX&&playerPosY==wumpusPosY &&wumpusDead ==0)
		{
			gameRunning=0;
			score-=300;
			eatenByWumpus=1;
		}
		if(escape==1)
		{
			wumpusAI();
		}
	}

	for(x=0;x<320;x++)
	{
		for(y=0;y<200;y++)
		{
			set_pixel(x,y,(int)(x*y*(256.0/(320.0*200.0))));
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
	print_string(10,116,40,"MADE BY AFFONSO AMENDOLA, 2018",1);
	print_string(10,126,40,"PRESS ANY KEY TO RETURN TO DOS",1);

	getch();
	set_graphics_mode(TEXT_MODE);
	return 0;
}

void wumpusAI()
{
	int xDistance;
	int yDistance;

	xDistance = playerPosX-wumpusPosX;
	yDistance = playerPosY-wumpusPosY;

	if(xDistance>=yDistance)
	{
		if(xDistance<0)
		{
			wumpusPosX-=1;
		}else
		if(xDistance>=0)
		{
			wumpusPosX+=1;
		}
	}else
	{
		if(yDistance<0)
		{
			wumpusPosY-=1;
		}else
		if(yDistance>=0)
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
		}
	}

	if(escape==1)
	{
		if(secretExitWall==0)
		{
			score=0;
			fill_rectangle( boardPosX + secretExitX*boardSquareSize,
					boardPosX + (secretExitX+1)*boardSquareSize -1,
					boardPosY,
					boardPosY + boardSquareSize-1,
					0);
		}
		if(secretExitWall==1)
		{
			score=1;
			fill_rectangle( boardPosX,
					boardPosX + boardSquareSize-1,
					boardPosY + secretExitY*boardSquareSize,
					boardPosY + (secretExitY+1)*boardSquareSize-1,
					0);
		}
		if(secretExitWall==2)
		{
			score=2;
			fill_rectangle( boardPosX + secretExitX*boardSquareSize,
					boardPosX + (secretExitX+1)*boardSquareSize -1,
					boardPosY + boardSizeY*boardSquareSize,
					boardPosY + (boardSizeY+1)*boardSquareSize-1,
					0);
		}
		if(secretExitWall==3)
		{
			score=3;
			fill_rectangle( boardPosX + boardSizeX*boardSquareSize,
					boardPosX + (boardSizeX+1)*boardSquareSize-1,
					boardPosY + secretExitY*boardSquareSize,
					boardPosY + (secretExitY+1)*boardSquareSize-1,
					0);
		}
	}
}

