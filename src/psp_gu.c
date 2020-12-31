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
#include <pspctrl.h>
#include <psptypes.h>
#include <png.h>
# ifndef LINUX_MODE
#include <pspgu.h>
#include <pspdisplay.h>
#include <pspkernel.h>
#include <psprtc.h>
# endif

  extern SDL_Surface *blit_surface;
  extern SDL_Surface *back_surface;

# ifndef LINUX_MODE
#define PSP_SLICE_SIZE	(32)
#define SCREEN_WIDTH    (480)
#define SCREEN_HEIGHT   (272)

static unsigned int __attribute__((aligned(16))) list[4096];

void
psp_sdl_gu_init(void)
{
	sceGuStart(GU_DIRECT,list);
	sceGuEnable(GU_TEXTURE_2D);
	sceGuTexMode(GU_PSM_5650, 0, 0, GU_FALSE);
	sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGB);
	sceGuTexFilter(GU_LINEAR, GU_LINEAR);
	sceGuDisable(GU_BLEND);
  sceGuDisable(GU_DEPTH_TEST);
	sceGuFinish();
	sceGuSync(0, 0);

  sceDisplayWaitVblankStart();
}

struct texVertex {
	unsigned short u, v;
	short x, y, z;
};

struct rectVertex {
	short x, y, z;
};

static inline int roundUpToPowerOfTwo (int x)
{
	return 1 << (32 - __builtin_allegrex_clz(x - 1));
}

void
psp_sdl_gu_stretch(SDL_Rect* srcrect, SDL_Rect* dstrect)
{
  SDL_Surface* src = blit_surface;
	unsigned short old_slice = 0; /* set when we load 2nd tex */
	unsigned int slice, num_slices, width, height, tbw, off_x, off_bytes;
	struct texVertex *vertices;
	char *pixels;

	sceKernelDcacheWritebackAll();

	off_bytes = (long)(((char*)src->pixels) + srcrect->x * src->format->BytesPerPixel) & 0xf;
	off_x = off_bytes / src->format->BytesPerPixel;
	width = roundUpToPowerOfTwo(srcrect->w + off_bytes);
	height = roundUpToPowerOfTwo(srcrect->h);
	tbw = src->pitch / src->format->BytesPerPixel;

	/* Align the texture prior to srcrect->x */
	pixels = ((char*)src->pixels) + (srcrect->x - off_x) * src->format->BytesPerPixel + 
		src->pitch * srcrect->y;
	num_slices = (srcrect->w + (PSP_SLICE_SIZE - 1)) / PSP_SLICE_SIZE;

	/* GE doesn't appreciate textures wider than 512 */
	if (width > 512)
		width = 512; 

	sceGuStart(GU_DIRECT,list);
	sceGuEnable(GU_TEXTURE_2D);
	sceGuTexMode(GU_PSM_5650, 0, 0, GU_FALSE);
	sceGuTexImage(0, width, height, tbw, pixels);
	sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGB);
	sceGuTexFilter(GU_LINEAR, GU_LINEAR);

	for (slice = 0; slice < num_slices; slice++) {

		vertices = (struct texVertex*)sceGuGetMemory(2 * sizeof(struct texVertex));

		if ((slice * PSP_SLICE_SIZE) < width) {
			vertices[0].u = slice * PSP_SLICE_SIZE + off_x;
		} else {
			if (!old_slice) {
				/* load another texture (src width > 512) */
				pixels += width * src->format->BytesPerPixel;
				sceGuTexImage(0, roundUpToPowerOfTwo(srcrect->w - width), 
					height, tbw, pixels);
				sceGuTexSync();
				old_slice = slice;
			}
			vertices[0].u = (slice - old_slice) * PSP_SLICE_SIZE + off_x;
		}
		vertices[1].u = vertices[0].u + PSP_SLICE_SIZE;
		if (vertices[1].u > (off_x + srcrect->w))
			vertices[1].u = off_x + srcrect->w;

		vertices[0].v = 0;
		vertices[1].v = vertices[0].v + srcrect->h;

		vertices[0].x = dstrect->x + (slice * PSP_SLICE_SIZE * dstrect->w + (srcrect->w - 1)) / srcrect->w;
		vertices[1].x = vertices[0].x + (PSP_SLICE_SIZE * dstrect->w + (srcrect->w - 1)) / srcrect->w;
		if (vertices[1].x > (dstrect->x + dstrect->w))
			vertices[1].x = dstrect->x + dstrect->w;

		vertices[0].y = dstrect->y;
		vertices[1].y = vertices[0].y + dstrect->h;

		vertices[0].z = 0;
		vertices[1].z = 0;

		sceGuDrawArray(GU_SPRITES,GU_TEXTURE_16BIT|GU_VERTEX_16BIT|GU_TRANSFORM_2D,
			2,0,vertices);
	}

	sceGuFinish();
	sceGuSync(0, 0);
}
# else
void 
psp_sdl_gu_stretch(SDL_Rect* srcrect, SDL_Rect* dstrect)
{
  SDL_BlitSurface(blit_surface, srcrect, back_surface, dstrect);
}
# endif
