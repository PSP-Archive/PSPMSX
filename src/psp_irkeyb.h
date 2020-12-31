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

# ifndef _PSP_IRKEYB_H_
# define _PSP_IRKEYB_H_

#ifdef __cplusplus
extern "C" {
#endif

# ifdef USE_PSP_IRKEYB

#include <pspirkeyb.h>
#include <pspirkeyb_rawkeys.h>

# define PSP_IRKEYB_UP       -100
# define PSP_IRKEYB_DOWN     -101
# define PSP_IRKEYB_LEFT     -102
# define PSP_IRKEYB_RIGHT    -103
# define PSP_IRKEYB_CROSS    -104
# define PSP_IRKEYB_TRIANGLE -105
# define PSP_IRKEYB_CIRCLE   -106
# define PSP_IRKEYB_SQUARE   -107
# define PSP_IRKEYB_SELECT   -108
# define PSP_IRKEYB_START    -109
# define PSP_IRKEYB_LTRIGGER -110
# define PSP_IRKEYB_RTRIGGER -111
# define PSP_IRKEYB_EMPTY    -1

# define PSP_IRKEYB_SUPPR     300
# define PSP_IRKEYB_INSERT    301
# define PSP_IRKEYB_HOME      302
# define PSP_IRKEYB_PAGEUP    303
# define PSP_IRKEYB_PAGEDOWN  304
# define PSP_IRKEYB_END       305

  extern int  psp_irkeyb_init();
  extern void psp_irkeyb_exit();
  extern void psp_irkeyb_wait_key();
  extern int  psp_irkeyb_read_key();
  extern int  psp_irkeyb_set_psp_key(SceCtrlData* c);

# endif

#ifdef __cplusplus
}
#endif

# endif
