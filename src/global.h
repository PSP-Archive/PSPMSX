#ifndef _GLOBAL_H_
#define _GLOBAL_H_
//LUDO:
# define MSX_RENDER_FAST       0
# define MSX_RENDER_NORMAL     1
# define MSX_RENDER_FIT_HEIGHT 2
# define MSX_RENDER_FIT        3
# define MSX_RENDER_X125       4
# define MSX_RENDER_X15        5
# define MSX_RENDER_X175       6
# define MSX_RENDER_MAX        7
# define MSX_LAST_RENDER       7

# define MAX_PATH   256
# define MSX_MAX_SAVE_STATE 5

# define MSX_WIDTH    272
# define MSX_HEIGHT   228
# define SCR_WIDTH    MSX_WIDTH 
# define SCR_HEIGHT   MSX_HEIGHT
# define SNAP_WIDTH   (MSX_WIDTH/2)
# define SNAP_HEIGHT  (MSX_HEIGHT/2)

# include <psptypes.h>
#include <SDL.h>

# define MSX_MAX_CHEAT    10

#define MSX_CHEAT_NONE    0
#define MSX_CHEAT_ENABLE  1
#define MSX_CHEAT_DISABLE 2

#define MSX_CHEAT_COMMENT_SIZE 25

#define MSX_MAX_RAM_PAGE  32
  
  typedef struct MSX_cheat_t {
    unsigned char  type;
    unsigned short addr;
    unsigned char  value;
    char           comment[MSX_CHEAT_COMMENT_SIZE];
  } MSX_cheat_t;

  typedef struct MSX_save_t {

    SDL_Surface    *surface;
    char            used;
    char            thumb;
    ScePspDateTime  date;

  } MSX_save_t;

  typedef struct MSX_t {
 
    MSX_save_t msx_save_state[MSX_MAX_SAVE_STATE];
    MSX_cheat_t msx_cheat[MSX_MAX_CHEAT];

    int        comment_present;
    char       msx_save_name[MAX_PATH];
    char       msx_home_dir[MAX_PATH];
    int        msx_speed_limiter;
    int        msx_version;
    int        msx_ram_pages;
    int        msx_ntsc;
    int        psp_screenshot_id;
    int        psp_cpu_clock;
    int        psp_reverse_analog;
    char       psp_irdajoy_type;
    char       psp_irdajoy_debug;
    int        psp_sound_volume;
    int        msx_snd_enable;
    int        msx_view_fps;
    int        msx_current_fps;
    int        msx_current_clock;
    int        msx_render_mode;
    int        msx_vsync;
    int        msx_delta_y;
    int        psp_skip_max_frame;
    int        psp_skip_cur_frame;
    int        msx_use_2413;
    int        msx_use_8950;
    int        msx_auto_fire_period;
    int        msx_auto_fire;
    int        msx_auto_fire_pressed;

  } MSX_t;

  extern MSX_t MSX;


//END_LUDO:

# endif
