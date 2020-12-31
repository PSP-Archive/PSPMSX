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
#include <malloc.h>

#include <zlib.h>
#include <psppower.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <SDL.h>
#include "psp_fmgr.h"
#include "psp_kbd.h"
#include "psp_battery.h"

static char loc_batt_str[128];

char *
psp_get_battery_string()
{
  strcpy(loc_batt_str, "none");

# ifndef LINUX_MODE
  char tmp[128];
  int ret;

  if (scePowerIsBatteryExist()) {
    ret = scePowerGetBatteryLifePercent();
    if (ret >= 0) {
      sprintf(tmp, "%d", ret);
      strcpy(loc_batt_str,tmp);
      strcat(loc_batt_str,"%");
      if(!scePowerIsPowerOnline()){
        if((ret=scePowerGetBatteryLifeTime()) >= 0){
          sprintf(tmp, " %dh", ret/60);
          strcat(loc_batt_str,tmp);
          sprintf(tmp, "%d", (ret%60) + 100);
          strcat(loc_batt_str,tmp+1);
        }
      }
    }
  }
# endif
  return loc_batt_str;
}

int
psp_is_low_battery()
{
  int ret = 0;
# ifndef LINUX_MODE
  if (scePowerIsBatteryExist()) {
    ret = scePowerGetBatteryLifePercent();
    if ((ret > 0) && (ret < 4)) return 1;
  }
# endif
  return 0;
}


