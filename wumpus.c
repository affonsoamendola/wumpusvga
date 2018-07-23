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
#include "wumpus.h"

//Most of the global variables below could be
// packed into a game-state struct.
int gameRunning = 1;
int boardPosX = 10;
int boardPosY = 10;
int boardSquareSize = 10;
int boardBorderColor = 30;
int playerColor = 40;
int visitedColor = 20;
int playerPosX = 5;
int playerPosY = 5;
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
int difficulty = 1;
int holeNumber = 3;
int wumpusDistance = 0;
int score = 0;
int escaping = 0;
int showWumpus = 0;
int showGold = 0;
int showHoles = 0;
int cheats = 0;
int eatenByWumpus = 0;
int fellInHole = 0;
int win = 0;
int wumpusDead = 0;
int visited[boardSizeX][boardSizeY];
int hole[boardSizeX][boardSizeY];

#ifdef MSDOS
int current_draw_buffer_page;
int current_frame_buffer_page;
#endif
char score_string[8];
int input;
int x, y;

void select_level()
{
	clear_screen();
	draw_message_box("CHOOSE YOUR CHALLENGE LEVEL:",
	                 "E:EASY  M:MEDIUM  H:HARD  U:ULTRA");

	input = 0;
	while(!(input & (INPUT_LEVEL_EASY |
	                 INPUT_LEVEL_MEDIUM |
	                 INPUT_LEVEL_HARD |
	                 INPUT_LEVEL_ULTRA )))
	{
		input = read_user_input();
		if(input & INPUT_LEVEL_EASY){   difficulty = 1; holeNumber = 6;}
		if(input & INPUT_LEVEL_MEDIUM){ difficulty = 2; holeNumber = 10;}
		if(input & INPUT_LEVEL_HARD){   difficulty = 3; holeNumber = 18;}
		if(input & INPUT_LEVEL_ULTRA){  difficulty = 4; holeNumber = 34;}
		if(input & INPUT_CHEAT_MENU)
		{
			cheats = 1;
			draw_message_box("YOU DIRTY CHEATER",
	                                 "Z:TOGGLE VIEW  X:GOTO ESCAPE");
			input = read_user_input();
			if(input & INPUT_ENABLE_CHEAT1 && cheats == 1)
			{
				showWumpus = !showWumpus;
				showGold = !showGold;
				showHoles = !showHoles;
			}

			if(input & INPUT_ENABLE_CHEAT2 && cheats == 1) escaping = 1;
		}
	}
}


void init_game_params(){
	secretExitWall = rand() % 4;
	secretExitX = rand() % (boardSizeX - 2) + 1;
	secretExitY = rand() % (boardSizeY - 2) + 1;

	do
	{
		wumpusPosX = rand() % (boardSizeX - 2) + 1;
		wumpusPosY = rand() % (boardSizeY - 2) + 1;
	}
	while((wumpusPosX == playerPosX &&
	       wumpusPosY == playerPosY) ||
	      (goldPosX == playerPosX &&
	       goldPosY == playerPosY) ||
	      (goldPosX == wumpusPosX &&
	       goldPosY == wumpusPosY));

	for(x = 1; x < boardSizeX - 1; x++)
	{
		for(y = 1; y < boardSizeY - 1; y++)
		{
			visited[x][y] = 0;
			hole[x][y] = 0;
		}
	}
	visited[playerPosX][playerPosY] = 1;

	while(holeNumber)
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
	}
}

void game_over()
{
#ifdef MSDOS
	draw_page(!current_draw_buffer_page);
#endif

	display_game_over_screen();

	if(eatenByWumpus)
	{
		draw_message_box("YOU GOT EATEN BY THE WUMPUS", "");
	}
	if(fellInHole)
	{
		draw_message_box("YOU FELL IN A HOLE AND DIED", "");
	}
	if(win)
	{
		draw_message_box("YOU WIN! CONGRATULATIONS!", "");
	}
	wait_for_user_input();

	draw_message_box("GAME OVER", "THANK YOU FOR PLAYING");
	wait_for_user_input();

	//integer_to_string(score, score_string);
	//draw_message_box("YOUR SCORE:", score_string);
	//wait_for_user_input();

#ifdef MSDOS
	draw_message_box("PRESS ANY KEY TO RETURN TO DOS", "");
	wait_for_user_input();

	frame_page(!current_frame_buffer_page);
#endif

	after_gameover();
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

void killed_the_wumpus()
{
		wumpusDead = 1;
		draw_message_box("YOU HEAR A HORRIFYING SCREAM", "");
		wait_for_user_input();
		arrowPosX = wumpusPosX;
		arrowPosY = wumpusPosY;
		score += 1000;
}

void fire_to_the_right()
{
	if(playerPosY == wumpusPosY &&
	   playerPosX < wumpusPosX &&
	   wumpusDead == 0)
		killed_the_wumpus();
	else
	{
		arrowPosX = boardSizeX - 1;
		arrowPosY = playerPosY;
		score -= 100;
	}
}

void fire_to_the_left()
{
	if(playerPosY == wumpusPosY &&
	   playerPosX > wumpusPosX &&
	   wumpusDead == 0)
		killed_the_wumpus();
	else
	{
		arrowPosX = 1;
		arrowPosY = playerPosY;
		score -= 100;
	}
}

void fire_upwards()
{
	if(playerPosX == wumpusPosX &&
	   playerPosY > wumpusPosY &&
	   wumpusDead == 0)
		killed_the_wumpus();
	else
	{
		arrowPosY = 1;
		arrowPosX = playerPosX;
		score -= 100;
	}
}

void fire_downwards()
{
	if(playerPosX == wumpusPosX &&
	   playerPosY < wumpusPosY &&
	   wumpusDead == 0)
		killed_the_wumpus();
	else
	{
		arrowPosY = boardSizeY - 1;
		arrowPosX = playerPosX;
		score -= 100;
	}
}

void fire_arrow()
{
	draw_message_box("FIRE IN WHAT DIRECTION?", "");
	input = read_user_input();
	if(input & INPUT_JOYSTICK_RIGHT) fire_to_the_right();
	if(input & INPUT_JOYSTICK_LEFT) fire_to_the_left();
	if(input & INPUT_JOYSTICK_UP) fire_upwards();
	if(input & INPUT_JOYSTICK_RIGHT) fire_downwards();
	hasArrow = 0;
}


void reached_the_exit_door()
{
	score += 1000;
	gameRunning = 0;
	win = 1;
}

void move_right()
{
	if(playerPosX < (boardSizeX-1))
	{
		playerPosX += 1;
		if(visited[playerPosX][playerPosY] == 0)
			score += 10*difficulty;
	}
	else if(secretExitWall == 3 &&
	        playerPosY == secretExitY &&
		escaping)
		reached_the_exit_door();

	visited[playerPosX][playerPosY] = 1;
}

void move_left()
{
	if(playerPosX > 1)
	{
		playerPosX--;
		if(!visited[playerPosX][playerPosY])
			score += 10 * difficulty;
	}
	else if(secretExitWall == 1 &&
	        playerPosY == secretExitY &&
	        escaping)
		reached_the_exit_door();

	visited[playerPosX][playerPosY] = 1;
}

void move_down()
{
	if(playerPosY < (boardSizeY-1))
	{
		playerPosY += 1;
		if(visited[playerPosX][playerPosY] == 0)
			score += 10 * difficulty;
	}
	else if(secretExitWall==2 &&
	        playerPosX == secretExitX &&
	        escaping)
		reached_the_exit_door();

	visited[playerPosX][playerPosY] = 1;
}

void move_up()
{
	if(playerPosY > 1)
	{
		playerPosY--;
		if(visited[playerPosX][playerPosY] == 0)
			score += 10 * difficulty;
	}
	else if(secretExitWall == 0 &&
	        playerPosX == secretExitX &&
	        escaping)
		reached_the_exit_door();

	visited[playerPosX][playerPosY] = 1;
}

int wumpus_is_nearby()
{
	return ((playerPosX + 1 == wumpusPosX && playerPosY == wumpusPosY) ||
	        (playerPosX - 1 == wumpusPosX && playerPosY == wumpusPosY) ||
	        (playerPosY + 1 == wumpusPosY && playerPosX == wumpusPosX) ||
	        (playerPosY - 1 == wumpusPosY && playerPosX == wumpusPosX));
}

int there_is_a_hole_nearby()
{
	return (hole[playerPosX + 1][playerPosY] ||
		hole[playerPosX - 1][playerPosY] ||
		hole[playerPosX][playerPosY + 1] ||
		hole[playerPosX][playerPosY - 1]);
}

void wumpus_hunting_you_messages()
{
	wumpusDistance = abs(playerPosX - wumpusPosX) +
	                 abs(playerPosY - wumpusPosY);

	if(wumpusDistance < 2)
	{
		draw_message_box("YOU HEAR DEATH RIGHT BESIDE YOU", "");
	}
	else if(wumpusDistance < 6)
	{
		draw_message_box("YOU HEAR THE WUMPUS CLOSING IN", "");
	}
	else if(wumpusDistance < 12)
	{
		draw_message_box("YOU HEAR GROWLING A SHORT DISTANCE AWAY", "");
	}
	else if(wumpusDistance < 22)
	{
		draw_message_box("YOU HEAR LOUD NOISES IN THE DISTANCE", "");
	}
}

void found_the_gold()
{
	score += 500;
	goldPosX = -1;
	goldPosY = -1;

	if(wumpusDead == 0)
	{
		draw_message_box("YOU HEAR THE MIGTHY ROAR OF THE WUMPUS", "");
		wait_for_user_input();
	}
	draw_message_box("A SECRET DOOR OPENS NEARBY", "");
	wait_for_user_input();
	escaping = 1;
}


void display_contextual_messages()
{
	if(wumpus_is_nearby()) draw_message_box("YOU SMELL A HORRIBLE STENCH", "");
	if(escaping && !wumpusDead) wumpus_hunting_you_messages();
	if(there_is_a_hole_nearby()) draw_message_box("YOU FEEL A DRAFT", "");

	if(playerPosX == goldPosX &&
	   playerPosY == goldPosY)
		draw_message_box("YOU SEE A GLINT OF LIGHT", "");

	if(playerPosX == wumpusPosX &&
	   playerPosY == wumpusPosY &&
	   wumpusDead)
		draw_message_box("HERE LIES A DEAD WUMPUS", "");

	if(playerPosX == arrowPosX &&
	   playerPosY == arrowPosY)
		draw_message_box("THERE IS AN ARROW ON THE GROUND", "");
}

void perform_player_action()
{
	input = read_user_input();

#ifdef MSDOS
	if(input & INPUT_QUIT_GAME)
	{
		gameRunning = 0;
		return;
	}
#endif

	if(input & INPUT_JOYSTICK_UP) move_up();
	if(input & INPUT_JOYSTICK_DOWN) move_down();
	if(input & INPUT_JOYSTICK_LEFT) move_left();
	if(input & INPUT_JOYSTICK_RIGHT) move_right();
	if(escaping && !wumpusDead) wumpusAI();

	if(input & INPUT_ACTION_GET &&
	   playerPosX == arrowPosX &&
	   playerPosY == arrowPosY)
	{
		hasArrow = 1;
		arrowPosX = -1;
		arrowPosY = -1;
	}

	if(input & INPUT_ACTION_FIRE &&
	   hasArrow == 1) fire_arrow();
}

void ways_to_die()
{
	if(hole[playerPosX][playerPosY])
	{
		score -= 200;
		fellInHole = 1;
		gameRunning = 0;
	}

	if(playerPosX == wumpusPosX &&
	   playerPosY == wumpusPosY &&
	   !wumpusDead)
	{
		score -= 300;
		eatenByWumpus = 1;
		gameRunning = 0;
	}
}

void game_loop()
{
#ifdef MSDOS	
	draw_page(!current_draw_buffer_page);
#endif
	fill_screen(0);
	draw_message_box("FOFONSO`S WUMPUS HUNT", "");
	print_score(score);
	drawScreen(visited);
	display_contextual_messages();

#ifdef MSDOS
	frame_page(!current_frame_buffer_page);
#endif
	perform_player_action();

	if(playerPosX == goldPosX &&
	   playerPosY == goldPosY)
		found_the_gold();

	ways_to_die();
}

int wumpus_main()
{
	display_title_screen();
	wait_for_user_input();

	select_level();

	init_game_params();

	while (gameRunning) game_loop();

	game_over();
	return 0;
}
