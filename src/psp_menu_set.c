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
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL/SDL.h>

#include <pspctrl.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspiofilemgr.h>

#include "MSX.h"
#include "global.h"
#include "psp_sdl.h"
#include "psp_kbd.h"
#include "psp_menu.h"
#include "psp_fmgr.h"
#include "psp_menu_kbd.h"
#include "psp_menu_set.h"
#include "psp_battery.h"
#include "psp_irkeyb.h"

extern SDL_Surface *back_surface;
static int psp_menu_dirty = 1;

# define MENU_SET_SOUND         0
# define MENU_SET_USE_2413      1
# define MENU_SET_USE_8950      2
# define MENU_SET_VOLUME        3
# define MENU_SET_IRDA_MODE     4
# define MENU_SET_VIDEO         5
# define MENU_SET_SPEED_LIMIT   6
# define MENU_SET_SKIP_FPS      7
# define MENU_SET_VIEW_FPS      8
# define MENU_SET_MSX_VERSION   9
# define MENU_SET_MSX_RAM_PAGES 10
# define MENU_SET_RENDER        11
# define MENU_SET_DELTA_Y       12
# define MENU_SET_VSYNC         13
# define MENU_SET_CLOCK         14

# define MENU_SET_LOAD          15
# define MENU_SET_SAVE          16
# define MENU_SET_RESET         17
# define MENU_SET_BACK          18

# define MAX_MENU_SET_ITEM (MENU_SET_BACK + 1)

  static menu_item_t menu_list[] =
  {
    { "Sound enabled      :"},
    { "Music FM-PAC       :"},
    { "Music Module       :"},
    { "Sound Volume       :"},
    { "IRDA device        :"},
    { "Video Mode         :"},
    { "Speed limiter      :"},
    { "Skip frame         :"},
    { "Display fps        :"},
    { "MSX version        :"},
    { "MSX ram size       :"},
    { "Render mode        :"},
    { "Delta Y            :"},
    { "Vsync              :"},
    { "Clock frequency    :"},

    { "Load settings"       },
    { "Save settings"       },
    { "Reset settings"      },
    { "Back to Menu"        }
  };

  static int cur_menu_id = MENU_SET_LOAD;

  static int msx_snd_enable       = 0;
  static int msx_use_8950         = 0;
  static int msx_use_2413         = 0;
  static int msx_view_fps         = 0;
  static int msx_render_mode      = 0;
  static int msx_vsync          = 0;
  static int msx_delta_y        = 0;
  static int msx_speed_limiter    = 50;
  static int psp_cpu_clock        = 222;
  static int psp_sound_volume     = 1;
  static int msx_version          = 0;
  static int msx_ram_pages        = 16;
  static int msx_ntsc             = 1;
  static int msx_skip_fps         = 0;
  static int psp_irda_mode       = 0;

static void
psp_settings_menu_reset(void);

static void 
psp_display_screen_settings_menu(void)
{
  char buffer[64];
  int menu_id = 0;
  int color   = 0;
  int x       = 0;
  int y       = 0;
  int y_step  = 0;

  //if (psp_menu_dirty) 
  {

    psp_sdl_blit_help();
    psp_menu_dirty = 0;

    psp_sdl_draw_rectangle(10,10,459,249,PSP_MENU_BORDER_COLOR,0);
    psp_sdl_draw_rectangle(11,11,457,247,PSP_MENU_BORDER_COLOR,0);

    psp_sdl_back2_print( 30, 6, " L: Keyboard ", PSP_MENU_NOTE_COLOR);

    psp_display_screen_menu_battery();

    psp_sdl_back2_print( 370, 6, " R: Reset ", PSP_MENU_WARNING_COLOR);

    psp_sdl_back2_print(30, 254, " []: Cancel  O/X: Valid  SELECT: Back ", 
                       PSP_MENU_BORDER_COLOR);

    psp_sdl_back2_print(370, 254, " By Zx-81 ",
                       PSP_MENU_AUTHOR_COLOR);
  }
  
  x      = 20;
  y      = 25;
  y_step = 10;
  
  for (menu_id = 0; menu_id < MAX_MENU_SET_ITEM; menu_id++) {
    color = PSP_MENU_TEXT_COLOR;
    if (cur_menu_id == menu_id) color = PSP_MENU_SEL_COLOR;

    psp_sdl_back2_print(x, y, menu_list[menu_id].title, color);

    if (menu_id == MENU_SET_SOUND) {
      if (msx_snd_enable) strcpy(buffer,"yes");
      else                 strcpy(buffer,"no ");
      string_fill_with_space(buffer, 4);
      psp_sdl_back2_print(190, y, buffer, color);
    } else
    if (menu_id == MENU_SET_USE_8950) {
      if (msx_use_8950) strcpy(buffer,"yes");
      else              strcpy(buffer,"no ");
      string_fill_with_space(buffer, 4);
      psp_sdl_back2_print(190, y, buffer, color);
    } else
    if (menu_id == MENU_SET_USE_2413) {
      if (msx_use_2413) strcpy(buffer,"yes");
      else              strcpy(buffer,"no ");
      string_fill_with_space(buffer, 4);
      psp_sdl_back2_print(190, y, buffer, color);
    } else
    if (menu_id == MENU_SET_VIEW_FPS) {
      if (msx_view_fps) strcpy(buffer,"yes");
      else                strcpy(buffer,"no ");
      string_fill_with_space(buffer, 4);
      psp_sdl_back2_print(190, y, buffer, color);
    } else
    if (menu_id == MENU_SET_VOLUME) {
      sprintf(buffer,"%d", psp_sound_volume);
      string_fill_with_space(buffer, 7);
      psp_sdl_back2_print(190, y, buffer, color);
    } else
    if (menu_id == MENU_SET_SKIP_FPS) {
      sprintf(buffer,"%d", msx_skip_fps);
      string_fill_with_space(buffer, 4);
      psp_sdl_back2_print(190, y, buffer, color);
    } else
    if (menu_id == MENU_SET_VIDEO) {
      if (msx_ntsc) strcpy(buffer,"NTSC");
      else          strcpy(buffer,"PAL ");
      string_fill_with_space(buffer, 5);
      psp_sdl_back2_print(190, y, buffer, color);
    } else
    if (menu_id == MENU_SET_SPEED_LIMIT) {
      if (msx_speed_limiter == 0)  strcpy(buffer, "no");
      else sprintf(buffer, "%d fps", msx_speed_limiter);
      string_fill_with_space(buffer, 7);
      psp_sdl_back2_print(190, y, buffer, color);
    } else
    if (menu_id == MENU_SET_MSX_VERSION) {
      if (msx_version == 0) strcpy(buffer, "MSX1");
      else
      if (msx_version == 1) strcpy(buffer, "MSX2");
      else                  strcpy(buffer, "MSX2+");
      string_fill_with_space(buffer, 6);
      psp_sdl_back2_print(190, y, buffer, color);
    } else
    if (menu_id == MENU_SET_MSX_RAM_PAGES) {
      sprintf(buffer, "%d kB", msx_ram_pages * 16);
      string_fill_with_space(buffer, 10);
      psp_sdl_back2_print(190, y, buffer, color);
    } else
    if (menu_id == MENU_SET_VSYNC) {
      if (msx_vsync) strcpy(buffer,"yes");
      else                strcpy(buffer,"no ");
      string_fill_with_space(buffer, 4);
      psp_sdl_back2_print(190, y, buffer, color);
    } else
    if (menu_id == MENU_SET_RENDER) {

      if (msx_render_mode == MSX_RENDER_FAST) strcpy(buffer, "fast");
      else 
      if (msx_render_mode == MSX_RENDER_NORMAL) strcpy(buffer, "normal");
      else 
      if (msx_render_mode == MSX_RENDER_FIT_HEIGHT) strcpy(buffer, "fit height");
      else 
      if (msx_render_mode == MSX_RENDER_X125) strcpy(buffer, "x125");
      else 
      if (msx_render_mode == MSX_RENDER_X15) strcpy(buffer, "x15");
      else 
      if (msx_render_mode == MSX_RENDER_X175) strcpy(buffer, "x175");
      else 
      if (msx_render_mode == MSX_RENDER_FIT ) strcpy(buffer, "fit");
      else                                    strcpy(buffer, "max");
      string_fill_with_space(buffer, 13);
      psp_sdl_back2_print(190, y, buffer, color);
    } else
    if (menu_id == MENU_SET_DELTA_Y) {
      sprintf(buffer,"%d", msx_delta_y);
      string_fill_with_space(buffer, 4);
      psp_sdl_back2_print(190, y, buffer, color);
    } else
    if (menu_id == MENU_SET_CLOCK) {
      sprintf(buffer,"%d", psp_cpu_clock);
      string_fill_with_space(buffer, 4);
      psp_sdl_back2_print(190, y, buffer, color);
    } else
    if (menu_id == MENU_SET_IRDA_MODE) {
      if (psp_irda_mode == 0) strcpy(buffer, "none");
      else
      if (psp_irda_mode == 1) strcpy(buffer, "keyboard");
      else                    strcpy(buffer, "joystick");
      string_fill_with_space(buffer, 10);
      psp_sdl_back2_print(190, y, buffer, color);
    } else
    if (menu_id == MENU_SET_RESET) {
      y += y_step;
    }

    y += y_step;
  }

  psp_menu_display_save_name();
}


#define MAX_CLOCK_VALUES 5
static int clock_values[MAX_CLOCK_VALUES] = { 133, 222, 266, 300, 333 };

static void
psp_settings_menu_clock(int step)
{
  int index;
  for (index = 0; index < MAX_CLOCK_VALUES; index++) {
    if (psp_cpu_clock == clock_values[index]) break;
  }
  if (step > 0) {
    index++;
    if (index >= MAX_CLOCK_VALUES) index = 0;
    psp_cpu_clock = clock_values[index];

  } else {
    index--;

    if (index < 0) index = MAX_CLOCK_VALUES - 1;
    psp_cpu_clock = clock_values[index];
  }
}

static void
psp_settings_menu_skip_fps(int step)
{
  if (step > 0) {
    if (msx_skip_fps < 25) msx_skip_fps++;
  } else {
    if (msx_skip_fps > 0) msx_skip_fps--;
  }
}

static void
psp_settings_menu_delta_y(int step)
{
  if (step > 0) {
    if (msx_delta_y < 37) msx_delta_y++;
  } else {
    if (msx_delta_y >  -37) msx_delta_y--;
  }
}

static void
psp_settings_menu_msx_version(int step)
{
  if (step > 0) {
    if (msx_version < 2) msx_version++;
  } else {
    if (msx_version > 0) msx_version--;
  }
}

static void
psp_settings_menu_msx_ram_pages(int step)
{
  if (step > 0) {

    if (msx_ram_pages <= 8) msx_ram_pages = 16;
    else
    if (msx_ram_pages <= 16) msx_ram_pages = 32;

  } else {

    if (msx_ram_pages <= 16) msx_ram_pages = 8;
    else
    if (msx_ram_pages <= 32) msx_ram_pages = 16;
  }
}

static void
psp_settings_menu_limiter(int step)
{
  if (step > 0) {
    if (msx_speed_limiter < 60) msx_speed_limiter++;
    else                        msx_speed_limiter = 0;
  } else {
    if (msx_speed_limiter > 0) msx_speed_limiter--;
    else                       msx_speed_limiter  = 60;
  }
}

static void
psp_settings_menu_volume(int step)
{
  if (step > 0) {
    if (psp_sound_volume < 10) psp_sound_volume++;
    else                       psp_sound_volume = 1;
  } else {
    if (psp_sound_volume >  1) psp_sound_volume--;
    else                       psp_sound_volume = 10;
  }
}

static void
psp_settings_menu_irda_mode(int step)
{
  if (step > 0) {
    if (psp_irda_mode < 2) psp_irda_mode++;
    else                   psp_irda_mode = 0;
  } else {
    if (psp_irda_mode >  0) psp_irda_mode--;
    else                    psp_irda_mode  = 2;
  }
}

static void
psp_settings_menu_ntsc(int step)
{
  if (msx_ntsc) {
    if (msx_speed_limiter == 60) msx_speed_limiter = 50;
  } else {
    if (msx_speed_limiter == 50) msx_speed_limiter = 60;
  }
  msx_ntsc = ! msx_ntsc; 
}

static void
psp_settings_menu_render(int step)
{
  if (step > 0) {
    if (msx_render_mode < MSX_LAST_RENDER) msx_render_mode++;
    else                                   msx_render_mode = 0;
  } else {
    if (msx_render_mode > 0) msx_render_mode--;
    else                     msx_render_mode = MSX_LAST_RENDER;
  }
}

static void
psp_settings_menu_init(void)
{
  msx_snd_enable       = MSX.msx_snd_enable;
  msx_use_2413         = MSX.msx_use_2413;
  msx_use_8950         = MSX.msx_use_8950;
  msx_render_mode      = MSX.msx_render_mode;
  msx_vsync            = MSX.msx_vsync;
  msx_delta_y          = MSX.msx_delta_y;
  msx_speed_limiter    = MSX.msx_speed_limiter;
  msx_skip_fps         = MSX.psp_skip_max_frame;
  msx_view_fps         = MSX.msx_view_fps;
  msx_version          = MSX.msx_version;
  msx_ram_pages        = MSX.msx_ram_pages;
  msx_ntsc             = MSX.msx_ntsc;
  psp_cpu_clock        = MSX.psp_cpu_clock;
  psp_irda_mode       = psp_irda_get_saved_mode();
  psp_sound_volume     = MSX.psp_sound_volume;
}

static void
psp_settings_menu_load(int format)
{
  int ret;

  ret = psp_fmgr_menu(format, 0);
  if (ret ==  1) /* load OK */
  {
    psp_display_screen_settings_menu();
    psp_sdl_back2_print(270,  80, "File loaded !", 
                       PSP_MENU_NOTE_COLOR);
    psp_menu_dirty = 1;
    psp_sdl_flip();
    sleep(1);
    psp_settings_menu_init();
  }
  else 
  if (ret == -1) /* Load Error */
  {
    psp_display_screen_settings_menu();
    psp_sdl_back2_print(270,  80, "Can't load file !", 
                       PSP_MENU_WARNING_COLOR);
    psp_menu_dirty = 1;
    psp_sdl_flip();
    sleep(1);
  }
}

static void
psp_settings_menu_validate(void)
{
  MSX.msx_snd_enable       = msx_snd_enable;
  MSX.msx_use_8950         = msx_use_8950;
  MSX.msx_use_2413         = msx_use_2413;
  MSX.msx_render_mode      = msx_render_mode;
  MSX.msx_vsync         = msx_vsync;
  MSX.msx_delta_y       = msx_delta_y;
  MSX.psp_cpu_clock        = psp_cpu_clock;
  MSX.msx_speed_limiter    = msx_speed_limiter;
 
  MSX.msx_ntsc = msx_ntsc;
  if (MSX.msx_ntsc) VPeriod = CPU_V262;
  else              VPeriod = CPU_V313;

  msx_change_render_mode(msx_render_mode);

  if ( (MSX.msx_version   != msx_version  ) ||
       (MSX.msx_ram_pages != msx_ram_pages) ) {
    MSX.msx_version = msx_version;
    MSX.msx_ram_pages = msx_ram_pages;
    /* Do a full reset */
    msx_eject_rom();
  }

  MSX.psp_skip_max_frame  = msx_skip_fps;
  MSX.msx_view_fps        = msx_view_fps;
  MSX.psp_sound_volume    = psp_sound_volume;
  MSX.psp_skip_cur_frame  = 0;

  if (psp_irda_set_saved_mode( psp_irda_mode )) {
      
    psp_display_screen_settings_menu();
    psp_sdl_back2_print(270,  80, "IRDA config saved", 
                       PSP_MENU_NOTE_COLOR);
    psp_sdl_back2_print(240,  90, "Emulator restart needed !", 
                       PSP_MENU_NOTE_COLOR);
    psp_menu_dirty = 1;
    psp_sdl_flip();
    sleep(2);
  }

  myPowerSetClockFrequency(MSX.psp_cpu_clock);
}

static void
psp_settings_menu_save()
{
  int error;

  psp_settings_menu_validate();
  error = msx_save_settings();

  if (! error) /* save OK */
  {
    psp_display_screen_settings_menu();
    psp_sdl_back2_print(270,  80, "File saved !", 
                       PSP_MENU_NOTE_COLOR);
    psp_menu_dirty = 1;
    psp_sdl_flip();
    sleep(1);
  }
  else 
  {
    psp_display_screen_settings_menu();
    psp_sdl_back2_print(270,  80, "Can't save file !", 
                       PSP_MENU_WARNING_COLOR);
    psp_menu_dirty = 1;
    psp_sdl_flip();
    sleep(1);
  }
}

static void
psp_settings_menu_reset(void)
{
  psp_display_screen_settings_menu();
  psp_sdl_back2_print(270, 80, "Reset Settings !", 
                     PSP_MENU_WARNING_COLOR);
  psp_menu_dirty = 1;
  psp_sdl_flip();
  msx_default_settings();
  psp_settings_menu_init();
  sleep(1);
}

int 
psp_settings_menu(void)
{
  SceCtrlData c;
  long        new_pad;
  long        old_pad;
  int         last_time;
  int         end_menu;

  psp_kbd_wait_no_button();

  old_pad   = 0;
  last_time = 0;
  end_menu  = 0;

  psp_settings_menu_init();

  psp_menu_dirty = 1;

  while (! end_menu)
  {
    psp_display_screen_settings_menu();
    psp_sdl_flip();

    while (1)
    {
      myCtrlPeekBufferPositive(&c, 1);
      c.Buttons &= PSP_ALL_BUTTON_MASK;

# ifdef USE_PSP_IRKEYB
      psp_irkeyb_set_psp_key(&c);
# endif
      if (c.Buttons) break;
    }

    new_pad = c.Buttons;

    if ((old_pad != new_pad) || ((c.TimeStamp - last_time) > PSP_MENU_MIN_TIME)) {
      last_time = c.TimeStamp;
      old_pad = new_pad;

    } else continue;

    if ((c.Buttons & (PSP_CTRL_LTRIGGER|PSP_CTRL_RTRIGGER|PSP_CTRL_START)) ==
        (PSP_CTRL_LTRIGGER|PSP_CTRL_RTRIGGER|PSP_CTRL_START)) {
      /* Exit ! */
      psp_sdl_exit(0);
    } else
    if ((c.Buttons & PSP_CTRL_LTRIGGER) == PSP_CTRL_LTRIGGER) {
      psp_keyboard_menu();
      end_menu = 1;
    } else
    if ((c.Buttons & PSP_CTRL_RTRIGGER) == PSP_CTRL_RTRIGGER) {
      psp_settings_menu_reset();
      end_menu = 1;
    } else
    if ((new_pad & PSP_CTRL_CROSS ) || 
        (new_pad & PSP_CTRL_CIRCLE) || 
        (new_pad & PSP_CTRL_RIGHT ) ||
        (new_pad & PSP_CTRL_LEFT  )) 
    {
      int step;

      if (new_pad & PSP_CTRL_LEFT)  step = -1;
      else 
      if (new_pad & PSP_CTRL_RIGHT) step =  1;
      else                          step =  0;

      switch (cur_menu_id ) 
      {
        case MENU_SET_SOUND      : msx_snd_enable = ! msx_snd_enable;
        break;              
        case MENU_SET_USE_2413   : msx_use_2413 = ! msx_use_2413;
        break;              
        case MENU_SET_USE_8950   : msx_use_8950 = ! msx_use_8950;
        break;              
        case MENU_SET_SPEED_LIMIT : psp_settings_menu_limiter( step );
        break;              
        case MENU_SET_VIEW_FPS   : msx_view_fps = ! msx_view_fps;
        break;              
        case MENU_SET_VOLUME     : psp_settings_menu_volume( step );
        break;              
        case MENU_SET_VIDEO      : psp_settings_menu_ntsc( step );
        break;
        case MENU_SET_MSX_VERSION : psp_settings_menu_msx_version( step );
        break;
        case MENU_SET_MSX_RAM_PAGES : psp_settings_menu_msx_ram_pages( step );
        break;
        case MENU_SET_SKIP_FPS   : psp_settings_menu_skip_fps( step );
        break;              
        case MENU_SET_DELTA_Y    : psp_settings_menu_delta_y( step );
        break;              
        case MENU_SET_RENDER     : psp_settings_menu_render( step );
        break;              
        case MENU_SET_VSYNC      : msx_vsync = ! msx_vsync;
        break;              
        case MENU_SET_CLOCK      : psp_settings_menu_clock( step );
        break;
        case MENU_SET_IRDA_MODE  : psp_settings_menu_irda_mode( step );
        break;
        case MENU_SET_LOAD       : psp_settings_menu_load(FMGR_FORMAT_SET);
                                   psp_menu_dirty = 1;
                                   old_pad = new_pad = 0;
        break;              
        case MENU_SET_SAVE       : psp_settings_menu_save();
                                   psp_menu_dirty = 1;
                                   old_pad = new_pad = 0;
        break;                     
        case MENU_SET_RESET      : psp_settings_menu_reset();
        break;                     
                                   
        case MENU_SET_BACK       : end_menu = 1;
        break;                     
      }

    } else
    if(new_pad & PSP_CTRL_UP) {

      if (cur_menu_id > 0) cur_menu_id--;
      else                 cur_menu_id = MAX_MENU_SET_ITEM-1;

    } else
    if(new_pad & PSP_CTRL_DOWN) {

      if (cur_menu_id < (MAX_MENU_SET_ITEM-1)) cur_menu_id++;
      else                                     cur_menu_id = 0;

    } else  
    if(new_pad & PSP_CTRL_SQUARE) {
      /* Cancel */
      end_menu = -1;
    } else 
    if(new_pad & PSP_CTRL_SELECT) {
      /* Back to MSX */
      end_menu = 1;
    }
  }
 
  if (end_menu > 0) {
    psp_settings_menu_validate();
  }

  psp_kbd_wait_no_button();

  psp_sdl_clear_screen( PSP_MENU_BLACK_COLOR );
  psp_sdl_flip();
  psp_sdl_clear_screen( PSP_MENU_BLACK_COLOR );
  psp_sdl_flip();

  return 1;
}

