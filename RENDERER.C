#include <renderer.h>

#include <bios.h>
#include <dos.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

unsigned char far* video_buffer = 0xA0000000L;
unsigned char far* rom_char_set = 0xF000FA6EL;

int current_video_mode = TEXT_MODE;

void set_graphics_mode(int mode)
{
	if(mode == TEXT_MODE)
	{
		_asm	{
			mov ax,0003h;
			int 10h;
			}
	}

	if(mode == GRAPHICS_MODE13)
	{
		_asm	{
			mov ax,0013h;
			int 10h;
			}
	}

	if(mode == GRAPHICS_MODEZ)
	{
		int data;
		_asm	{
			mov ax,0013h;
			int 10h;
			}

		outp(CRT_CONTROLLER,CRT_MAX_SCANLINE);
		data = inp(CRT_CONTROLLER+1);
		outp(CRT_CONTROLLER+1,RESET_BITS(data,0x0f));

		outp(CRT_CONTROLLER,CRT_ADDR_MODE);
		data=inp(CRT_CONTROLLER+1);
		outp(GFX_CONTROLLER+1,RESET_BITS(data,0x40));

		outp(CRT_CONTROLLER,CRT_MODE_CONTROL);
		data=inp(CRT_CONTROLLER+1);
		outp(CRT_CONTROLLER+1,SET_BITS(data,0x40));

		outp(GFX_CONTROLLER,GFX_WRITE_MODE);
		data=inp(GFX_CONTROLLER+1);
		outp(GFX_CONTROLLER+1,RESET_BITS(data,0x10));

		outp(GFX_CONTROLLER,GFX_MISC);
		data=inp(GFX_CONTROLLER+1);
		outp(GFX_CONTROLLER+1,RESET_BITS(data,0x02));

		outp(SEQUENCER,SEQ_MEMORY_MODE);
		data=inp(SEQUENCER+1);
		data=RESET_BITS(data,0x08);
		data=SET_BITS(data,0x04);
		outp(SEQUENCER+1,data);
		outp(SEQUENCER,SEQ_PLANE_ENABLE);
		outp(SEQUENCER+1,0x0f);

		_asm	{
			les di, video_buffer;
			xor ax,ax;
			mov cx,320*400/8;
			rep stosw;
			}
	}
	current_video_mode = mode;
}

void fill_screen(int color)
{
	if(current_video_mode == GRAPHICS_MODE13)
	{
	_asm	{
		les di,video_buffer;
		mov al,BYTE PTR color;
		mov ah,al;
		mov cx,320*200/2;
		rep stosw;
		}
	}
	if(current_video_mode == GRAPHICS_MODEZ)
	{
	_asm	{
		mov dx,SEQUENCER;
		mov al,SEQ_PLANE_ENABLE;
		mov ah,0fh;
		out dx,ax;
		les di,video_buffer;
		mov al,BYTE PTR color;
		mov ah,al;
		mov cx,320*400/8;
		rep stosw;
		}
	}
}

void set_pixel(int x, int y, int color)
{
	if(current_video_mode == GRAPHICS_MODE13)
	{
		video_buffer[(y<<8)+(y<<6)+x] = (unsigned char)color;
	}
	if(current_video_mode == GRAPHICS_MODEZ)
	{
		_asm	{
			mov dx,SEQUENCER;
			mov al,SEQ_PLANE_ENABLE;
			mov cl,BYTE PTR x;
			and cl,03h;
			mov ah,1;
			shl ah,cl;
			out dx,ax;
			}
		video_buffer[(y<<6)+(y<<4)+(x>>2)] = (unsigned char)color;
	}
}

int get_pixel(int x, int y)
{
	if(current_video_mode == GRAPHICS_MODE13)
	{
		return((int)(video_buffer[(y<<8)+(y<<6)+x]));
	}
	if(current_video_mode == GRAPHICS_MODEZ)
	{
		_asm	{
			mov dx,SEQUENCER;
			mov al,SEQ_PLANE_ENABLE;
			mov cl,BYTE PTR x;
			and cl,03h;
			mov ah,1;
			shl ah,cl;
			out dx,ax;
			}
		return((int)(video_buffer[(y<<6)+(y<<4)+(x>>2)]));
	}
	return -1;
}

void draw_line_h(int x1, int x2, int y, int color)
{
	if(current_video_mode == GRAPHICS_MODE13)
	{
		int temp;

		if(x1>x2)
		{
			temp = x1;
			x1 = x2;
			x2 = temp;
		}

		_fmemset((char far *)(video_buffer + ((y<<8) +(y<<6))+x1),(unsigned char)color, x2-x1+1);
	}
}

void draw_line_v(int x, int y1, int y2, int color)
{
	if(current_video_mode == GRAPHICS_MODE13)
	{
		int temp;
		int length;
		int i;
		unsigned char far *start_offset;

		if(y1>y2)
		{
			temp = y1;
			y1 = y2;
			y2 = temp;
		}

		start_offset = video_buffer + ((y1<<8) + (y1<<6)) + x;

		length = y2-y1;

		for(i=0;i<=length;i++)
		{
			*start_offset = (unsigned char)color;
			start_offset+=320;
		}
	}
}

void fill_rectangle(int x1, int x2, int y1, int y2, int color)
{
	if(current_video_mode == GRAPHICS_MODE13)
	{
	int temp;
	int i;
	int height;
	unsigned char far * start_offset;

	if(x1>x2)
	{
		temp = x1;
		x1 = x2;
		x2 = temp;
	}
	if(y1>y2)
	{
		temp = y1;
		y1 = y2;
		y2 = temp;
	}

	start_offset = video_buffer + ((y1<<8) + (y1<<6)) + x1;
	height = y2-y1;

	for(i=0;i<=height;i++)
	{
		_fmemset(start_offset,(unsigned char)color, x2-x1+1);
		start_offset+=320;
	}
	}
}

void print_char(int xc, int yc, char c, int color, int transparent)
{
	if(current_video_mode == GRAPHICS_MODE13)
	{
		int offset, x, y;

		unsigned char far *work_char;

		unsigned char bit_mask;

		work_char = rom_char_set + c*8;

		offset = (yc<<8)+(yc<<6)+xc;

		for(y=0; y<8; y++)
		{
			bit_mask = 0x80;
			for(x=0; x<8; x++)
			{
				if((*work_char & bit_mask))
					video_buffer[offset+x] = (unsigned char)color;
				else
				if(!transparent)
					video_buffer[offset+x] = 0;

				bit_mask = (bit_mask>>1);
			}
			offset+=320;
			work_char++;
		}
	}
}

void print_string(int x, int y, int color, char *string, int transparent)
{
	if(current_video_mode == GRAPHICS_MODE13)
	{
		int i, length;

		length = strlen(string);

		for(i=0; i<length; i++)
		{
			print_char(x+(i<<3), y, string[i], color, transparent);
		}
	}
}

char* inttostring(int i, char b[])
{
	char const digit[] = "0123456789";
	char* p = b;
	int shifter = i;

	if(i<0)
	{
		*p++ = '-';
		i*= -1;
	}

	do
	{
		++p;
		shifter = shifter/10;
	}while(shifter);
	*p = '\0';
	do
	{
		*--p = digit[i%10];
		i = i/10;
	}while(i);
	return b;
}
