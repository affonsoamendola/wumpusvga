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

#ifndef FONFLIB_H
#define FONFLIB_H

#define SET_BITS(x,bits) (x|bits)
#define RESET_BITS(x,bits) (x&~bits)

char* inttostring(int i, char b[]);

int pow_int(int number, int to_the_power_of);

//void fputi(int number, int size, FILE * file);

#endif /* FONFLIB_H */
