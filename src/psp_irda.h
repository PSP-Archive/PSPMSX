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

# ifndef _PSP_IRDA_JOY_H_
# define _PSP_IRDA_JOY_H_

# ifdef __cplusplus
extern "C" {
# endif

# define IRDAJOY_TYPE_NONE          0
# define IRDAJOY_TYPE_JOYSTICK      1
# define IRDAJOY_TYPE_PADDLE        2
# define IRDAJOY_TYPE_DUAL_PADDLE   3
# define IRDAJOY_MAX_TYPE           3


#include <psptypes.h>

  typedef struct irda_joy_t {
     u16 paddleA;
     u16 paddleB;
      u8 atariJ;

  } irda_joy_t;

# define JOY_ATARI_UP_MASK 0x10
# define JOY_ATARI_DOWN_MASK 0x08
# define JOY_ATARI_LEFT_MASK 0x04
# define JOY_ATARI_RIGHT_MASK 0x02
# define JOY_ATARI_BUTTON_MASK 0x01

  extern int psp_irda_get_joy_event( irda_joy_t* irda_joy_evt );
  extern void psp_irda_joy_convert( irda_joy_t* i_joy, SceCtrlData* psp_c );
  extern int psp_irda_joy_init();

  extern int psp_load_irda_mode();
  extern int psp_irda_is_kbd_mode();
  extern int psp_irda_is_joy_mode();

  extern int psp_irda_get_saved_mode();
  extern int psp_irda_set_saved_mode();

# ifdef __cplusplus
}
# endif

# endif
