#include "wumpus.xpm"
#include "char_codes.h"
#include <stdio.h>

#define WIDTH 320
FILE *fp;

void save_tile(int x, int y, int tile_number){
	int address = 16 * tile_number;

	fseek(fp, address, SEEK_SET);
	for (int i=0; i<8; i++){
		char value = 0;
		for (int j=0; j<4; j++){
			if(wumpus[4 + y*8 + 7 - j - 4][x*8 + 7 - i] == ' ')
				value += (0b10001 << j);
		}
		fwrite(&value, 1, 1, fp);

		value = 0;
		for (int j=0; j<4; j++){
			if(wumpus[4 + y*8 + 7 - j][x*8 + 7 - i] == ' ')
				value += (0b10001 << j);
		}
		fwrite(&value, 1, 1, fp);
	}
}

int main(){
	fp = fopen("gs01.11f", "r+");

	save_tile(5, 1, OPEN_FLOOR);
	save_tile(6, 1, HIDDEN_FLOOR);

	save_tile(1, 1, WUMPUS_AWAKEN);
	//save_tile(2, 1, WUMPUS_SLEEPING1);
	//save_tile(2, 2, WUMPUS_SLEEPING2);

	save_tile(3, 1, PLAYER);
	//save_tile(1, 3, GOLD);
	save_tile(3, 3, ARROW);

	save_tile(8, 3, CORNER_TOP_LEFT);
	save_tile(11, 3, CORNER_TOP_RIGHT);
	save_tile(8, 6, CORNER_BOTTOM_LEFT);
	save_tile(11, 6, CORNER_BOTTOM_RIGHT);

	save_tile(9, 3, TOP_BORDER__LEFT);
	save_tile(10, 3, TOP_BORDER__RIGHT);
	save_tile(9, 6, BOTTOM_BORDER__LEFT);
	save_tile(10, 6, BOTTOM_BORDER__RIGHT);

	save_tile(8, 4, LEFT_BORDER__TOP);
	save_tile(8, 5, LEFT_BORDER__BOTTOM);
	save_tile(11, 4, RIGHT_BORDER__TOP);
	save_tile(11, 5, RIGHT_BORDER__BOTTOM);

	fclose(fp);
	return 0;
}

