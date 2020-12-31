/*

Copyright 2005-2011 - Ludovic Jacomme - All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are
permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this list of
      conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright notice, this list
      of conditions and the following disclaimer in the documentation and/or other materials
      provided with the distribution.

THIS SOFTWARE IS PROVIDED BY LUDOVIC JACOMME 'AS IS'' AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL LUDOVIC JACOMME OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those of the
authors and should not be interpreted as representing official policies, either expressed
or implied, of Ludovic Jacomme.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include <pspctrl.h>
#include <psptypes.h>
#include <png.h>
# ifndef LINUX_MODE
#include <pspgu.h>
#include <pspdisplay.h>
#include <psprtc.h>
# endif

#include "MSX.h"
#include "psp_sdl.h"
#include "psp_gu.h"
#include "psp_danzeff.h"

  extern unsigned char psp_font[];

  SDL_Surface *back_surface;
  SDL_Surface *back2_surface;

  SDL_Surface *background_surface = NULL;
  SDL_Surface *blit_surface = NULL;
  SDL_Surface *help_surface;
  SDL_Surface *splash_surface;
  SDL_Surface *thumb_surface;
  SDL_Surface *save_surface;

uint
psp_sdl_rgb(uchar R, uchar G, uchar B)
{
  return SDL_MapRGB(back_surface->format, R,G,B);
}

ushort *
psp_sdl_get_vram_addr(uint x, uint y)
{
  ushort *vram = (ushort *)back_surface->pixels;
  return vram + x + (y*PSP_LINE_SIZE);
}

void
loc_psp_debug(char *file, int line, char *message)
{
  static int current_line = 0;
  static int current_col  = 10;

  char buffer[128];
  current_line += 10;
  if (current_line > 250)
  {
    if (current_col == 200) {
      psp_sdl_clear_screen(psp_sdl_rgb(0, 0, 0xff));
      current_col = 10;
    } else {
      current_col = 200;
    }
    
    current_line = 10;
  }
  sprintf(buffer,"%s:%d %s", file, line, message);
  psp_sdl_print(current_col, current_line, buffer, psp_sdl_rgb(0xff,0xff,0xff) );
}

void 
psp_sdl_print(int x,int y, char *str, int color)
{
  int index;
  int x0 = x;

  for (index = 0; str[index] != '\0'; index++) {
    psp_sdl_put_char(x, y, color, 0, str[index], 1, 0);
    x += 8;
    if (x >= (PSP_SDL_SCREEN_WIDTH - 8)) {
      x = x0; y++;
    }
    if (y >= (PSP_SDL_SCREEN_HEIGHT - 8)) break;
  }
}

void
psp_sdl_clear_screen(int color)
{
  int x; int y;
  ushort *vram = psp_sdl_get_vram_addr(0,0);
  
  for (y = 0; y < PSP_SDL_SCREEN_HEIGHT; y++) {
    for (x = 0; x < PSP_SDL_SCREEN_WIDTH; x++) {
      vram[x + (y*PSP_LINE_SIZE)] = color;
    }
  }
}

void
psp_sdl_black_screen()
{
  SDL_FillRect(back_surface,NULL,SDL_MapRGB(back_surface->format,0x0,0x0,0x0));
  SDL_Flip(back_surface);
  SDL_FillRect(back_surface,NULL,SDL_MapRGB(back_surface->format,0x0,0x0,0x0));
  SDL_Flip(back_surface);
}

void
psp_sdl_clear_blit(int color)
{
  if (blit_surface) {
    ushort *vram = (ushort *)blit_surface->pixels;
    int my_size = blit_surface->h * blit_surface->w;
    while (my_size > 0) {
      vram[--my_size] = color;
    }
  }
}

void 
psp_sdl_draw_rectangle(int x, int y, int w, int h, int border, int mode) 
{
  ushort *vram = (ushort *)psp_sdl_get_vram_addr(x, y);
  int xo, yo;
  if (mode == PSP_SDL_XOR) {
    for (xo = 0; xo < w; xo++) {
      vram[xo] ^=  border;
      vram[xo + h * PSP_LINE_SIZE] ^=  border;
    }
    for (yo = 0; yo < h; yo++) {
      vram[yo * PSP_LINE_SIZE] ^=  border;
      vram[w + yo * PSP_LINE_SIZE] ^=  border;
    }
    vram[w + h * PSP_LINE_SIZE] ^=  border;
  } else {
    for (xo = 0; xo < w; xo++) {
      vram[xo] =  border;
      vram[xo + h * PSP_LINE_SIZE] =  border;
    }
    for (yo = 0; yo < h; yo++) {
      vram[yo * PSP_LINE_SIZE] =  border;
      vram[w + yo * PSP_LINE_SIZE] =  border;
    }
    vram[w + h * PSP_LINE_SIZE] =  border;
  }
}

void 
psp_sdl_fill_rectangle(int x, int y, int w, int h, int color, int mode)
{
  ushort *vram  = (ushort *)psp_sdl_get_vram_addr(x, y);
  int xo, yo;
  if (mode == PSP_SDL_XOR) {
    for (xo = 0; xo <= w; xo++) {
      for (yo = 0; yo <= h; yo++) {
        if ( ((xo == 0) && ((yo == 0) || (yo == h))) ||
             ((xo == w) && ((yo == 0) || (yo == h))) ) {
          /* Skip corner */
        } else {
          vram[xo + yo * PSP_LINE_SIZE] ^=  color;
        }
      }
    }
  } else {
    for (xo = 0; xo <= w; xo++) {
      for (yo = 0; yo <= h; yo++) {
        vram[xo + yo * PSP_LINE_SIZE] =  color;
      }
    }
  }
}

static int
psp_sdl_get_back2_color(int x, int y)
{
  uchar *back2 = (uchar *)back2_surface->pixels;
  int bytes_per_pixels = back2_surface->format->BytesPerPixel;
  int pitch            = back2_surface->pitch;
  Uint8 r = back2[0 + (y * pitch) + (x * bytes_per_pixels)];
  Uint8 g = back2[1 + (y * pitch) + (x * bytes_per_pixels)];
  Uint8 b = back2[2 + (y * pitch) + (x * bytes_per_pixels)];
	int color = psp_sdl_rgb(r, g, b);

  return color;
}

void 
psp_sdl_back2_rectangle(int x, int y, int w, int h)
{
  if (! back2_surface) {
    psp_sdl_fill_rectangle(x, y, w, h, 0x0, 0);
    return;
  }

  ushort *vram  = (ushort *)psp_sdl_get_vram_addr(x, y);

  int xo, yo;
  for (xo = 0; xo <= w; xo++) {
    for (yo = 0; yo <= h; yo++) {
      vram[xo + yo * PSP_LINE_SIZE] = psp_sdl_get_back2_color(x + xo, y + yo);
    }
  }
}

void 
psp_sdl_put_char(int x, int y, int color, int bgcolor, uchar c, int drawfg, int drawbg)
{
  int cx;
  int cy;
  int b;
  int index;

  ushort *vram = (ushort *)psp_sdl_get_vram_addr(x, y);
  index = ((ushort)c) * 8;

  for (cy=0; cy<8; cy++) {
    b=0x80;
    for (cx=0; cx<8; cx++) {
      if (psp_font[index] & b) {
        if (drawfg) vram[cx + cy * PSP_LINE_SIZE] = color;
      } else {
        if (drawbg) vram[cx + cy * PSP_LINE_SIZE] = bgcolor;
      }
      b = b >> 1;
    }
    index++;
  }
}

void 
psp_sdl_back2_put_char(int x, int y, int color, uchar c)
{
  int cx;
  int cy;
  int bmask;
  int index;

  if (! back2_surface) {
    psp_sdl_put_char(x, y, color, 0x0, c, 1, 1);
    return;
  }

  ushort *vram  = (ushort *)psp_sdl_get_vram_addr(x, y);

  index = ((ushort)c) * 8;

  for (cy=0; cy<8; cy++) {
    bmask=0x80;
    for (cx=0; cx<8; cx++) {
      if (psp_font[index] & bmask) {
        vram[cx + cy * PSP_LINE_SIZE] = color;
      } else {
        vram[cx + cy * PSP_LINE_SIZE] = psp_sdl_get_back2_color(x + cx, y + cy);
      }
      bmask = bmask >> 1;
    }
    index++;
  }
}

unsigned char
psp_convert_utf8_to_iso_8859_1(unsigned char c1, unsigned char c2)
{
  unsigned char res = 0;
  if (c1 == 0xc2) res = c2;
  else
  if (c1 == 0xc3) res = c2 | 0x40;
  return res;
}


void 
psp_sdl_fill_print(int x,int y,const char *str, int color, int bgcolor)
{
  int index;
  int x0 = x;

  for (index = 0; str[index] != '\0'; index++) {
    uchar c = str[index];
    if ((c == 0xc2) || (c == 0xc3)) {
      uchar new_c = psp_convert_utf8_to_iso_8859_1(c, str[index+1]);
      if (new_c) { c = new_c; index++; }
    }
    psp_sdl_put_char(x, y, color, bgcolor, c, 1, 1);
    x += 8;
    if (x >= (PSP_SDL_SCREEN_WIDTH - 8)) {
      x = x0; y++;
    }
    if (y >= (PSP_SDL_SCREEN_HEIGHT - 8)) break;
  }
}

void
psp_sdl_back2_print(int x,int y,const char *str, int color)
{
  int index;
  int x0 = x;

  for (index = 0; str[index] != '\0'; index++) {
    uchar c = str[index];
    if ((c == 0xc2) || (c == 0xc3)) {
      uchar new_c = psp_convert_utf8_to_iso_8859_1(c, str[index+1]);
      if (new_c) { c = new_c; index++; }
    }
    psp_sdl_back2_put_char(x, y, color, c);
    x += 8;
    if (x >= (PSP_SDL_SCREEN_WIDTH - 8)) {
      x = x0; y++;
    }
    if (y >= (PSP_SDL_SCREEN_HEIGHT - 8)) break;
  }
}

void
psp_sdl_lock(void)
{
  SDL_LockSurface(back_surface);
}

void
psp_sdl_load_background()
{
  background_surface = IMG_Load("./background.png");
  thumb_surface = IMG_Load("./thumb.png");
}

void
psp_sdl_blit_background()
{
  static int first = 1;

  if (first && (back2_surface == NULL)) {
    psp_sdl_load_background();
    first = 0;
  }
  back2_surface = background_surface;

  if (back2_surface != NULL) {
	  SDL_BlitSurface(back2_surface, NULL, back_surface, NULL);
  } else {
    psp_sdl_clear_screen(psp_sdl_rgb(0x00, 0x00, 0x00));
  }
}

void
psp_sdl_blit_thumb(int dst_x, int dst_y, SDL_Surface* a_surface)
{
  SDL_Rect dstRect;
  dstRect.x = dst_x;
  dstRect.y = dst_y;
  dstRect.w = a_surface->w;
  dstRect.h = a_surface->h;
  SDL_BlitSurface(a_surface, NULL, back_surface, &dstRect);
}

void
psp_sdl_blit_splash()
{
  if (! splash_surface) {
    splash_surface = IMG_Load("./splash.png");
  }
	SDL_BlitSurface(splash_surface, NULL, back_surface, NULL);
}

void
psp_sdl_blit_help()
{
  if (! help_surface) {
    help_surface = IMG_Load("./help.png");
  }
  back2_surface = help_surface;
	SDL_BlitSurface(back2_surface, NULL, back_surface, NULL);
}

void
psp_sdl_display_splash()
{
  int index = 0;
  SceCtrlData c;

  int x = (480 - (2 + strlen(PSPMSX_VERSION)) * 8) / 2;
  int y = 272 - 24;
  int col = psp_sdl_rgb(0x80, 0x80, 0x80);

  psp_sdl_blit_splash();
  psp_sdl_print(x, y, PSPMSX_VERSION, col);
  psp_sdl_flip();

  psp_sdl_blit_splash();
  psp_sdl_print(x, y, PSPMSX_VERSION, col);
  psp_sdl_flip();

}

void
psp_sdl_unlock(void)
{
  SDL_UnlockSurface(back_surface);
}

void
psp_sdl_flip(void)
{
  SDL_Flip(back_surface);
}

#define  systemRedShift      (back_surface->format->Rshift)
#define  systemGreenShift    (back_surface->format->Gshift)
#define  systemBlueShift     (back_surface->format->Bshift)
#define  systemRedMask       (back_surface->format->Rmask)
#define  systemGreenMask     (back_surface->format->Gmask)
#define  systemBlueMask      (back_surface->format->Bmask)

int
psp_sdl_save_png(SDL_Surface* my_surface, char* filename)
{
  int w = my_surface->w;
  int h = my_surface->h;
  u8* pix = (u8*)my_surface->pixels;
  u8 writeBuffer[512 * 3];

  FILE *fp = fopen(filename,"wb");

  if(!fp) return 0;
  
  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                                NULL,
                                                NULL,
                                                NULL);
  if(!png_ptr) {
    fclose(fp);
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);

  if(!info_ptr) {
    png_destroy_write_struct(&png_ptr,NULL);
    fclose(fp);
    return 0;
  }

  png_init_io(png_ptr,fp);

  png_set_IHDR(png_ptr,
               info_ptr,
               w,
               h,
               8,
               PNG_COLOR_TYPE_RGB,
               PNG_INTERLACE_NONE,
               PNG_COMPRESSION_TYPE_DEFAULT,
               PNG_FILTER_TYPE_DEFAULT);

  png_write_info(png_ptr,info_ptr);

  u8 *b = writeBuffer;

  int sizeX = w;
  int sizeY = h;
  int y;
  int x;

  u16 *p = (u16 *)pix;
  for(y = 0; y < sizeY; y++) {
     for(x = 0; x < sizeX; x++) {
      u16 v = p[x];

      *b++ = ((v & systemRedMask  ) >> systemRedShift  ) << 3; // R
      *b++ = ((v & systemGreenMask) >> systemGreenShift) << 2; // G 
      *b++ = ((v & systemBlueMask ) >> systemBlueShift ) << 3; // B
    }
    p += my_surface->pitch / 2;
    png_write_row(png_ptr,writeBuffer);
     
    b = writeBuffer;
  }

  png_write_end(png_ptr, info_ptr);

  png_destroy_write_struct(&png_ptr, &info_ptr);

  fclose(fp);
  return 1;
}

int
psp_sdl_load_png(SDL_Surface* my_surface, char* filename)
{
  int w = my_surface->w;
  int h = my_surface->h;
  int pitch = my_surface->pitch / 2;
  u16* pix = (u16*)my_surface->pixels;

  FILE *fp = fopen(filename,"rb");
  if (!fp) return 0;

  const size_t nSigSize = 8;
  byte signature[nSigSize];
  if (fread(signature, sizeof(byte), nSigSize, fp) != nSigSize) {
    fclose(fp);
    return 0;
  }

  if (!png_check_sig(signature, nSigSize)) {
    fclose(fp);
    return 0;
  }

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                               NULL,
                                               NULL,
                                               NULL);
  if(!png_ptr) {
    fclose(fp);
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);

  if(!info_ptr) {
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    fclose(fp);
    return 0;
  }

  png_init_io(png_ptr, fp);
  png_set_sig_bytes(png_ptr, nSigSize);
  png_read_png(png_ptr, info_ptr,
    PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING |
    PNG_TRANSFORM_EXPAND | PNG_TRANSFORM_BGR , NULL);

  png_uint_32 width = info_ptr->width;
  png_uint_32 height = info_ptr->height;
  int color_type = info_ptr->color_type;

  if ((width  > w) ||
      (height > h)) {
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    fclose(fp);
    return 0;
  }

  png_byte **pRowTable = info_ptr->row_pointers;
  unsigned int x, y;
  byte r, g, b;

  for (y=0; y<height; y++) {
    png_byte *pRow = pRowTable[y];
    for (x=0; x<width; x++) {
      switch(color_type) {
        case PNG_COLOR_TYPE_GRAY:
          r = g = b = *pRow++;
          break;
        case PNG_COLOR_TYPE_GRAY_ALPHA:
          r = g = b = pRow[0];
          pRow += 2;
          break;
        case PNG_COLOR_TYPE_RGB:
          b = pRow[0];
          g = pRow[1];
          r = pRow[2];
          pRow += 3;
          break;
        case PNG_COLOR_TYPE_RGB_ALPHA:
          b = pRow[0];
          g = pRow[1];
          r = pRow[2];
          pRow += 4;
          break;
        default:
          r = g = b = 0;
          break;
      }

      u16 v = (((r >> 3) << systemRedShift  ) & systemRedMask) |
              (((g >> 2) << systemGreenShift) & systemGreenMask) |
              (((b >> 3) << systemBlueShift ) & systemBlueMask);
      *pix++= v;
    }
    pix += pitch - width;
  }
  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
  fclose(fp);

  return 1;
}

int
psp_sdl_save_screenshot_png(char *filename)
{
  return psp_sdl_save_png(blit_surface, filename);
}

int
psp_sdl_save_thumb_png(SDL_Surface* my_surface, char* filename)
{
  int x;
  int y;
  u16* src_pixel = (u16*)blit_surface->pixels;
  u16* dst_pixel = (u16*)my_surface->pixels;

  for (y = 0; y < MSX_HEIGHT; y += 2) {
    for (x = 0; x < MSX_WIDTH; x += 2) {
      *dst_pixel++ = src_pixel[x];
    }
    src_pixel += MSX_WIDTH * 2;
  }
  /* Then save thumb in file */
  return psp_sdl_save_png(my_surface, filename);
}

int
psp_sdl_load_thumb_png(SDL_Surface* my_surface, char* filename)
{ 
  return psp_sdl_load_png( my_surface, filename);
}

void
psp_sdl_save_screenshot(void)
{
  char TmpFileName[MAX_PATH];

  sprintf(TmpFileName,"%s/scr/screenshot_%d.png", MSX.msx_home_dir, MSX.psp_screenshot_id++);
  if (MSX.psp_screenshot_id >= 10) MSX.psp_screenshot_id = 0;
  psp_sdl_save_screenshot_png(TmpFileName);
}

int
psp_sdl_init(void)
{
  if (SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO) < 0) {
     return 0;
  }

  if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
    return 0;
  }

#ifndef LINUX_MODE
  back_surface=SDL_SetVideoMode(PSP_SDL_SCREEN_WIDTH,PSP_SDL_SCREEN_HEIGHT, 16, 
                                SDL_ANYFORMAT|SDL_DOUBLEBUF|SDL_HWSURFACE|SDL_HWPALETTE);
# else
  back_surface=SDL_SetVideoMode(PSP_SDL_SCREEN_WIDTH,PSP_SDL_SCREEN_HEIGHT, 16 , 
                                SDL_DOUBLEBUF|SDL_HWSURFACE);
# endif

  if ( !back_surface) {
    return 0;
  }

# ifndef LINUX_MODE
  psp_sdl_gu_init();
# endif

  blit_surface = SDL_CreateRGBSurface(SDL_SWSURFACE, 
     SCR_WIDTH, SCR_HEIGHT,
     back_surface->format->BitsPerPixel,
     back_surface->format->Rmask,
     back_surface->format->Gmask,
     back_surface->format->Bmask, 0);
# ifndef LINUX_MODE
  // HACK
  blit_surface->pixels = (void *)0x44088000;
# endif

  SDL_ShowCursor(SDL_DISABLE);

  psp_sdl_display_splash();

  /* Danzeff Keyboard */
  danzeff_load();
  danzeff_set_screen(back_surface);

  /* Create surface for save state */
  int Index = 0;
  for (Index = 0; Index < MSX_MAX_SAVE_STATE; Index++) {
    MSX.msx_save_state[Index].surface = 
       SDL_CreateRGBSurface(SDL_SWSURFACE, 
                            SNAP_WIDTH, SNAP_HEIGHT,
                            back_surface->format->BitsPerPixel,
                            back_surface->format->Rmask,
                            back_surface->format->Gmask,
                            back_surface->format->Bmask, 0);
  }
  save_surface = SDL_CreateRGBSurface(SDL_SWSURFACE, 
                            SNAP_WIDTH, SNAP_HEIGHT,
                            back_surface->format->BitsPerPixel,
                            back_surface->format->Rmask,
                            back_surface->format->Gmask,
                            back_surface->format->Bmask, 0);

  return 1;
}

void
psp_sdl_exit(int status)
{
  SDL_CloseAudio();
  SDL_Quit();

  exit(status);
}
