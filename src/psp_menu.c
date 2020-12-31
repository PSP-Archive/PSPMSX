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
#include "psp_menu_joy.h"
#include "psp_menu_set.h"
#include "psp_menu_cheat.h"
#include "psp_battery.h"
#include "psp_menu_help.h"
#include "psp_editor.h"
#include "psp_irkeyb.h"

extern SDL_Surface *back_surface;
static int psp_menu_dirty = 1;

# define MENU_SCREENSHOT   0

# define MENU_LOAD_SLOT    1
# define MENU_SAVE_SLOT    2
# define MENU_DEL_SLOT     3

# define MENU_HELP         4

# define MENU_LOAD_ROM     5
# define MENU_LOAD_DISK0   6
# define MENU_LOAD_DISK1   7
# define MENU_EJECT_ROM    8

# define MENU_EDITOR       9
# define MENU_CHEATS      10
# define MENU_KEYBOARD    11
# define MENU_JOYSTICK    12
# define MENU_SETTINGS    13

# define MENU_RESET       14
# define MENU_BACK        15
# define MENU_EXIT        16

# define MAX_MENU_ITEM (MENU_EXIT + 1)

  static menu_item_t menu_list[] =
  { 
    { "Save Screenshot :" },

    { "Load Slot" },
    { "Save Slot" },
    { "Delete Slot" },

    { "Help" },

    { "Load Rom" },     
    { "Load Disk 0" },
    { "Load Disk 1" },
    { "Eject ROM" },

    { "Comments" },
    { "Cheats" },
    { "Keyboard" }, 
    { "Joystick" },
    { "Settings" },

    { "Reset MSX" },
    { "Back to MSX" },
    { "Exit" }
  };

  static int cur_menu_id = MENU_LOAD_ROM;
  static int cur_slot    = 0;

void 
psp_menu_display_save_name()
{
  char buffer[128];
  int Length;

  snprintf(buffer, 20, "Game: %s", MSX.msx_save_name);
  Length = strlen(buffer);
  psp_sdl_back2_print(460 - (8*Length), 25, buffer, PSP_MENU_TEXT2_COLOR);
}

void
string_fill_with_space(char *buffer, int size)
{
  int length = strlen(buffer);
  int index;

  for (index = length; index < size; index++) {
    buffer[index] = ' ';
  }
  buffer[size] = 0;
}

void
psp_display_screen_menu_battery(void)
{
  char buffer[64];

  int Length;
  int color;

  snprintf(buffer, 50, " [%s] ", psp_get_battery_string());
  Length = strlen(buffer);

  if (psp_is_low_battery()) color = PSP_MENU_RED_COLOR;
  else                      color = PSP_MENU_GREEN_COLOR;

  psp_sdl_back2_print(240 - ((8*Length) / 2), 6, buffer, color);
}

void 
psp_display_screen_menu(void)
{
  char buffer[64];
  int menu_id = 0;
  int slot_id = 0;
  int color   = 0;
  int x       = 0;
  int y       = 0;
  int y_step  = 0;

  //if (psp_menu_dirty) 
  {
    psp_sdl_blit_background();
    psp_menu_dirty = 0;

    psp_sdl_draw_rectangle(10,10,459,249,PSP_MENU_BORDER_COLOR,0);
    psp_sdl_draw_rectangle(11,11,457,247,PSP_MENU_BORDER_COLOR,0);

    psp_sdl_back2_print( 30, 6, " L: Settings ", PSP_MENU_NOTE_COLOR);

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
  
  for (menu_id = 0; menu_id < MAX_MENU_ITEM; menu_id++) {
    color = PSP_MENU_TEXT_COLOR;
    if (cur_menu_id == menu_id) color = PSP_MENU_SEL_COLOR;
    else 
    if (menu_id == MENU_EXIT) color = PSP_MENU_WARNING_COLOR;
    else
    if (menu_id == MENU_HELP) color = PSP_MENU_GREEN_COLOR;

    if (menu_id == MENU_EDITOR) {
      if (MSX.comment_present) {
        psp_sdl_back2_print(x, y, "View comment", color);
      } else {
        psp_sdl_back2_print(x, y, "New comment", color);
      }

    } else {
      psp_sdl_back2_print(x, y, menu_list[menu_id].title, color);
    }

    if (menu_id == MENU_SCREENSHOT) {
      sprintf(buffer,"%d", MSX.psp_screenshot_id);
      string_fill_with_space(buffer, 4);
      psp_sdl_back2_print(160, y, buffer, color);
      y += y_step;
    } else
    if (menu_id == MENU_DEL_SLOT) {
      y += y_step;
    } else
    if (menu_id == MENU_EJECT_ROM) {
      y += y_step;
    } else
    if (menu_id == MENU_SETTINGS) {
      y += y_step;
    } else
    if (menu_id == MENU_BACK) {
      y += y_step;
    } else
    if (menu_id == MENU_CHEATS) {
    } else
    if (menu_id == MENU_HELP) {
      y += y_step;
    }

    y += y_step;
  }
  y_step = 10;
  y      = 45;

  for (slot_id = 0; slot_id < MSX_MAX_SAVE_STATE; slot_id++) {
    if (slot_id == cur_slot) color = PSP_MENU_SEL2_COLOR;
    else                     color = PSP_MENU_TEXT_COLOR;

    if (MSX.msx_save_state[slot_id].used) {
      ScePspDateTime* my_date = &MSX.msx_save_state[slot_id].date;
# ifndef LINUX_MODE
      sprintf(buffer, "- %02d/%02d %02d:%02d:%02d",
         my_date->day, my_date->month, 
         my_date->hour, my_date->minute, my_date->second );
# else
      sprintf(buffer, "- used");
# endif
    } else {
      sprintf(buffer, "- empty");
    }
    string_fill_with_space(buffer, 32);
    psp_sdl_back2_print(120, y, buffer, color);

    y += y_step;
  }

  if (MSX.msx_save_state[cur_slot].thumb) {
    psp_sdl_blit_thumb(300, 40, MSX.msx_save_state[cur_slot].surface);
  } else {
    psp_sdl_blit_thumb(300, 40, thumb_surface);
  }

  psp_menu_display_save_name();
}

static void
psp_main_menu_reset(void)
{
  /* Reset ! */
  psp_display_screen_menu();
  psp_sdl_back2_print(140, 160, "Reset MSX !", PSP_MENU_WARNING_COLOR);
  psp_menu_dirty = 1;
  psp_sdl_flip();
  emulator_reset();
  sleep(1);
}

static int
psp_main_menu_load(int format, int drive_id)
{
  int ret;

  ret = psp_fmgr_menu(format, drive_id);
  if (ret ==  1) /* load OK */
  {
    psp_menu_dirty = 1;
    psp_display_screen_menu();
    psp_sdl_back2_print(140, 160, "File loaded !", 
                       PSP_MENU_NOTE_COLOR);
    psp_menu_dirty = 1;
    psp_sdl_flip();
    sleep(1);
    return 1;
  }
  else 
  if (ret == -1) /* Load Error */
  {
    psp_menu_dirty = 1;
    psp_display_screen_menu();
    psp_sdl_back2_print(140, 160, "Can't load file !", 
                       PSP_MENU_WARNING_COLOR);
    psp_menu_dirty = 1;
    psp_sdl_flip();
    sleep(1);
  }
  return 0;
}

/* used by hotkeys */
int
psp_main_menu_load_current()
{
  if (MSX.msx_save_state[cur_slot].used) {
    return msx_snapshot_load_slot(cur_slot);
  }
  return -1;
}

int
psp_main_menu_save_current()
{
  msx_save_back_to_blit();
  return msx_snapshot_save_slot(cur_slot);
}

static int
psp_main_menu_load_slot()
{
  int error;

  if (! MSX.msx_save_state[cur_slot].used) {

    psp_display_screen_menu();
    psp_sdl_back2_print(140, 160, "Slot is empty !", 
                       PSP_MENU_NOTE_COLOR);
    psp_menu_dirty = 1;
    psp_sdl_flip();
    sleep(1);

    return 0;
  }

  error = msx_snapshot_load_slot(cur_slot);

  if (! error) /* save OK */
  {
    psp_display_screen_menu();
    psp_sdl_back2_print(140, 160, "File loaded !", 
                       PSP_MENU_NOTE_COLOR);
    psp_menu_dirty = 1;
    psp_sdl_flip();
    sleep(1);

    return 1;
  }
  else 
  {
    psp_display_screen_menu();
    psp_sdl_back2_print(140, 160, "Can't load file !", 
                       PSP_MENU_WARNING_COLOR);
    psp_menu_dirty = 1;
    psp_sdl_flip();
    sleep(1);
  }
  return 0;
}

static void
psp_main_menu_save_slot()
{
  int error;
  error = msx_snapshot_save_slot(cur_slot);

  if (! error) /* save OK */
  {
    psp_display_screen_menu();
    psp_sdl_back2_print(140, 160, "File saved !", 
                       PSP_MENU_NOTE_COLOR);
    psp_menu_dirty = 1;
    psp_sdl_flip();
    sleep(1);
  }
  else 
  {
    psp_display_screen_menu();
    psp_sdl_back2_print(140, 160, "Can't save file !", 
                       PSP_MENU_WARNING_COLOR);
    psp_menu_dirty = 1;
    psp_sdl_flip();
    sleep(1);
  }
}

static void
psp_main_menu_del_slot()
{
  int error;

  if (! MSX.msx_save_state[cur_slot].used) {

    psp_display_screen_menu();
    psp_sdl_back2_print(140, 160, "Slot is empty !", 
                       PSP_MENU_NOTE_COLOR);
    psp_menu_dirty = 1;
    psp_sdl_flip();
    sleep(1);

    return;
  }

  error = msx_snapshot_del_slot(cur_slot);

  if (! error) /* save OK */
  {
    psp_display_screen_menu();
    psp_sdl_back2_print(140, 160, "File deleted !", 
                       PSP_MENU_NOTE_COLOR);
    psp_menu_dirty = 1;
    psp_sdl_flip();
    sleep(1);
  }
  else 
  {
    psp_display_screen_menu();
    psp_sdl_back2_print(140, 160, "Can't delete file !", 
                       PSP_MENU_WARNING_COLOR);
    psp_menu_dirty = 1;
    psp_sdl_flip();
    sleep(1);
  }
}

static void
psp_main_menu_eject_rom()
{
  int error;

  error = msx_eject_rom();

  psp_display_screen_menu();
  psp_sdl_back2_print(140, 160, "Rom ejected !", 
                     PSP_MENU_NOTE_COLOR);
  psp_sdl_flip();
  sleep(1);
}

static void
psp_main_menu_cur_slot(int step)
{
  if (step == 1) {
    cur_slot++; if (cur_slot >= MSX_MAX_SAVE_STATE) cur_slot = 0;
  } else if (step == -1) {
    cur_slot--; if (cur_slot < 0) cur_slot = MSX_MAX_SAVE_STATE - 1;
  }
}

static void
psp_main_menu_screenshot(void)
{
  psp_screenshot_mode = 10;
}

int
psp_main_menu_exit(void)
{
  SceCtrlData c;

  psp_display_screen_menu();
  psp_sdl_back2_print(140, 160, "press X to confirm !", PSP_MENU_WARNING_COLOR);
  psp_menu_dirty = 1;
  psp_sdl_flip();

  psp_kbd_wait_no_button();

  do
  {
    myCtrlPeekBufferPositive(&c, 1);
    c.Buttons &= PSP_ALL_BUTTON_MASK;

    if (c.Buttons & PSP_CTRL_CROSS) psp_sdl_exit(0);

  } while (c.Buttons == 0);

  psp_kbd_wait_no_button();

  return 0;
}

void
psp_main_menu_editor()
{
  char TmpFileName[MAX_PATH];
  sprintf(TmpFileName, "%s/txt/%s.txt", MSX.msx_home_dir, MSX.msx_save_name);
  psp_editor_menu( TmpFileName );
}

int 
psp_main_menu(void)
{
  SceCtrlData c;
  long        new_pad;
  long        old_pad;
  int         last_time;
  int         end_menu;

  audio_pause();

  msx_save_back_to_blit();

  psp_kbd_wait_no_button();

  old_pad   = 0;
  last_time = 0;
  end_menu  = 0;

  psp_menu_dirty = 1;

  while (! end_menu)
  {
    psp_display_screen_menu();
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
      psp_settings_menu();
      psp_menu_dirty = 1;
      old_pad = new_pad = 0;
    } else
    if ((c.Buttons & PSP_CTRL_RTRIGGER) == PSP_CTRL_RTRIGGER) {
      psp_main_menu_reset();
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
        case MENU_LOAD_SLOT : if (step) psp_main_menu_cur_slot(step);
                              else
                              if (psp_main_menu_load_slot()) {
                                end_menu = 1;
                              }
        break;
        case MENU_SAVE_SLOT : if (step) psp_main_menu_cur_slot(step);
                              else      psp_main_menu_save_slot();
        break;
        case MENU_DEL_SLOT  : if (step) psp_main_menu_cur_slot(step);
                              else      psp_main_menu_del_slot();
        break;

        case MENU_LOAD_ROM  : psp_menu_dirty = 1;
                              if (psp_main_menu_load(FMGR_FORMAT_ROM, 0)) {
                                end_menu = 1;
                              }
                              psp_menu_dirty = 1;
                              old_pad = new_pad = 0;
        break;              

        case MENU_EDITOR    : psp_menu_dirty = 1;
                              psp_main_menu_editor();
                              psp_menu_dirty = 1;
                              old_pad = new_pad = 0;
        break;

        case MENU_CHEATS    : psp_menu_dirty = 1;
                              psp_cheat_menu();
                              psp_menu_dirty = 1;
                              old_pad = new_pad = 0;
        break;              
        case MENU_LOAD_DISK0 : psp_menu_dirty = 1;
                               if (psp_main_menu_load(FMGR_FORMAT_DISK, 0)) end_menu = 1;
                               psp_menu_dirty = 1;
                               old_pad = new_pad = 0;
        break;              

        case MENU_LOAD_DISK1 : psp_menu_dirty = 1;
                               if (psp_main_menu_load(FMGR_FORMAT_DISK, 1)) end_menu = 1;
                               psp_menu_dirty = 1;
                               old_pad = new_pad = 0;
        break;              

        case MENU_KEYBOARD   : psp_keyboard_menu();
                               psp_menu_dirty = 1;
                               old_pad = new_pad = 0;
        break;
        case MENU_JOYSTICK   : psp_joystick_menu();
                               psp_menu_dirty = 1;
                               old_pad = new_pad = 0;
        break;
        case MENU_SETTINGS   : psp_menu_dirty = 1;
                               psp_settings_menu();
                               psp_menu_dirty = 1;
                               old_pad = new_pad = 0;
        break;

        case MENU_EJECT_ROM  : psp_main_menu_eject_rom();
                               psp_menu_dirty = 1;
                               old_pad = new_pad = 0;
        break;

        case MENU_SCREENSHOT : psp_main_menu_screenshot();
                               end_menu = 1;
        break;              

        case MENU_RESET     : psp_main_menu_reset();
                              end_menu = 1;
        break;

        case MENU_BACK      : end_menu = 1;
        break;

        case MENU_EXIT      : psp_main_menu_exit();
        break;

        case MENU_HELP      : psp_menu_dirty = 1;
                              psp_help_menu();
                              old_pad = new_pad = 0;
                              psp_menu_dirty = 1;
        break;              
      }

    } else
    if(new_pad & PSP_CTRL_UP) {

      if (cur_menu_id > 0) cur_menu_id--;
      else                 cur_menu_id = MAX_MENU_ITEM-1;

    } else
    if(new_pad & PSP_CTRL_DOWN) {

      if (cur_menu_id < (MAX_MENU_ITEM-1)) cur_menu_id++;
      else                                 cur_menu_id = 0;

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
 
  psp_kbd_wait_no_button();

  psp_sdl_clear_screen( PSP_MENU_BLACK_COLOR );
  psp_sdl_flip();
  psp_sdl_clear_screen( PSP_MENU_BLACK_COLOR );
  psp_sdl_flip();

  psp_sdl_clear_blit(0);

#ifndef LINUX_MODE
  psp_sdl_gu_init();
#endif

  audio_resume();

  return 1;
}

