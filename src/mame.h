/***************************************************************************

	mame.h

	Controls execution of the core MAME system.

***************************************************************************/

#ifndef MACHINE_H
#define MACHINE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <compat/posix_string.h>
#include <compat/msvc.h>
#if defined(__EMSCRIPTEN__)
#include <strings.h>
#endif
#include "fileio.h"
#include "log.h"
#include "drawgfx.h"
#include "palette.h"


extern int gbPriorityBitmapIsDirty;
extern retro_environment_t environ_cb;

/***************************************************************************

	Parameters

***************************************************************************/

#define FRAMES_PER_FPS_UPDATE		12

#define MAX_GFX_ELEMENTS 32
#define MAX_MEMORY_REGIONS 32

#define APPNAME					"mame2003-plus"

/***************************************************************************

	Core description of the currently-running machine

***************************************************************************/

struct RegionInfo
{
	UINT8 *		base;
	size_t		length;
	UINT32		type;
	UINT32		flags;
};


struct RunningMachine
{
	/* ----- game-related information ----- */

	/* points to the definition of the game machine */
	const struct GameDriver *gamedrv;

	/* points to the constructed MachineDriver */
	const struct InternalMachineDriver *drv;

	/* array of memory regions */
	struct RegionInfo		memory_region[MAX_MEMORY_REGIONS];


	/* ----- video-related information ----- */

	/* array of pointers to graphic sets (chars, sprites) */
	struct GfxElement *		gfx[MAX_GFX_ELEMENTS];

	/* main bitmap to render to (but don't do it directly!) */
	struct mame_bitmap *	scrbitmap;

	/* current visible area, and a prerotated one adjusted for orientation */
	struct rectangle 		visible_area;
	struct rectangle		absolute_visible_area;

	/* remapped palette pen numbers. When you write directly to a bitmap in a
	   non-paletteized mode, use this array to look up the pen number. For example,
	   if you want to use color #6 in the palette, use pens[6] instead of just 6. */
	pen_t *					pens;

	/* lookup table used to map gfx pen numbers to color numbers */
	UINT16 *				game_colortable;

	/* the above, already remapped through Machine->pens */
	pen_t *					remapped_colortable;

	/* video color depth: 16, 15 or 32 */
	int						color_depth;

	/* video orientation; obsolete; always set to 0 */
	int						orientation;


	/* ----- audio-related information ----- */

	/* the digital audio sample rate; 0 if sound is disabled. */
	int						sample_rate;

	/* samples loaded from disk */
	struct GameSamples *	samples;


	/* ----- input-related information ----- */

	/* the input ports definition from the driver is copied here and modified */
	struct InputPort *		input_ports;

	/* original input_ports without modifications */
	struct InputPort *		input_ports_default;


	/* ----- user interface-related information ----- */

	/* font used by the user interface */
	struct GfxElement *		uifont;

	/* font parameters */
	int 					uifontwidth, uifontheight;

	/* user interface visible area */
	int 					uixmin, uiymin;
	int 					uiwidth, uiheight;

	/* user interface orientation */
	int 					ui_orientation;


	/* ----- debugger-related information ----- */

	/* bitmap where the debugger is rendered */
	struct mame_bitmap *	debug_bitmap;

	/* pen array for the debugger, analagous to the pens above */
	pen_t *					debug_pens;

	/* colortable mapped through the pens, as for the game */
	pen_t *					debug_remapped_colortable;

	/* font used by the debugger */
	struct GfxElement *		debugger_font;
};



/***************************************************************************

	Options passed from the frontend to the main core

***************************************************************************/

#define ARTWORK_USE_ALL			(~0)
#define ARTWORK_USE_NONE		(0)
#define ARTWORK_USE_BACKDROPS	0x01
#define ARTWORK_USE_OVERLAYS	0x02
#define ARTWORK_USE_BEZELS		0x04
#define RETROPAD_MAME			0
#define RETROPAD_MODERN			1
#define RETROPAD_SNES			2


/* The host platform should fill these fields with the preferences specified in the GUI */
/* or on the commandline. */
struct GameOptions
{
  mame_file *record;			       /* handle to file to record input to */
  mame_file *playback;		       /* handle to file to playback input from */
  mame_file *language_file;	     /* handle to file for localization */

  char   *romset_filename_noext;
  char   *libretro_content_path;
  char   *libretro_system_path;
  char   *libretro_save_path;

  int		   mame_debug;		       /* 1 to enable debugging */
  int 	   skip_gameinfo;		     /* 1 to skip the game info screen at startup */
  int      skip_warnings;        /* 1 to skip the warning messages at startup */
  int      display_setup;        /* 1 to display the MAME setup menu until reset to 0 */

  unsigned dial_share_xy;
  unsigned mouse_device;
  unsigned input_interface;        /* can be set to RETRO_DEVICE_JOYPAD, RETRO_DEVICE_KEYBOARD, or 0 (both simultaneously) */
  unsigned retropad_layout;        /* can be set to RETROPAD_MODERN, RETROPAD_SNES, or RETROPAD_MAME */
  unsigned rstick_to_btns;
  unsigned tate_mode;

  int      crosshair_enable;
  unsigned activate_dcs_speedhack;

  int		   samplerate;		       /* sound sample playback rate, in KHz */
  int		   use_samples;	         /* 1 to enable external .wav samples */

  float	   brightness;		       /* brightness of the display */
  float	   pause_bright;		     /* additional brightness when in pause */
  float	   gamma;			           /* gamma correction of the display */
  int      frameskip;
  int		   color_depth;	         /* valid: 15, 16, or 32. any other value means auto */
  int		   ui_orientation;	     /* orientation of the UI relative to the video */
      
  int		   vector_width;	       /* requested width for vector games; 0 means default (640) */
  int		   vector_height;	       /* requested height for vector games; 0 means default (480) */
  int		   beam;			           /* vector beam width */
  int	     vector_flicker;	     /* vector beam flicker effect control */
  float	   vector_intensity_correction;   
  int		   translucency;	       /* 1 to enable translucency on vectors */
  int 	   antialias;		         /* 1 to enable antialiasing on vectors */
  unsigned vector_resolution_multiplier;

  int		   use_artwork;	         /* bitfield indicating which artwork pieces to use */
  int		   artwork_res;	         /* 1 for 1x game scaling, 2 for 2x */
  int		   artwork_crop;	       /* 1 to crop artwork to the game screen */

  char	   savegame;		         /* character representing a savegame to load */
  int      crc_only;             /* specify if only CRC should be used as checksum */
  unsigned skip_rom_verify;       
  char *	 bios;			           /* specify system bios (if used), 0 is default */

  int		   debug_width;	         /* requested width of debugger bitmap */
  int		   debug_height;	       /* requested height of debugger bitmap */
  int		   debug_depth;	         /* requested depth of debugger bitmap */

};



/***************************************************************************

	Display state passed to the OSD layer for rendering

***************************************************************************/

/* these flags are set in the mame_display struct to indicate that */
/* a particular piece of state has changed since the last call to */
/* osd_update_video_and_audio() */
#define GAME_BITMAP_CHANGED			0x00000001
#define GAME_PALETTE_CHANGED		0x00000002
#define GAME_VISIBLE_AREA_CHANGED	0x00000004
#define VECTOR_PIXELS_CHANGED		0x00000008
#define DEBUG_BITMAP_CHANGED		0x00000010
#define DEBUG_PALETTE_CHANGED		0x00000020
#define DEBUG_FOCUS_CHANGED			0x00000040
#define LED_STATE_CHANGED			0x00000080


/* the main mame_display structure, containing the current state of the */
/* video display */
struct mame_display
{
    /* bitfield indicating which states have changed */
    UINT32					changed_flags;

    /* game bitmap and display information */
    struct mame_bitmap *	game_bitmap;			/* points to game's bitmap */
    struct rectangle		game_bitmap_update;		/* bounds that need to be updated */
    const rgb_t *			game_palette;			/* points to game's adjusted palette */
    UINT32					game_palette_entries;	/* number of palette entries in game's palette */
    UINT32 *				game_palette_dirty;		/* points to game's dirty palette bitfield */
    struct rectangle 		game_visible_area;		/* the game's visible area */
    void *					vector_dirty_pixels;	/* points to X,Y pairs of dirty vector pixels */

    /* debugger bitmap and display information */
    struct mame_bitmap *	debug_bitmap;			/* points to debugger's bitmap */
    const rgb_t *			debug_palette;			/* points to debugger's palette */
    UINT32					debug_palette_entries;	/* number of palette entries in debugger's palette */
    UINT8					debug_focus;			/* set to 1 if debugger has focus */

    /* other misc information */
    UINT8					led_state;				/* bitfield of current LED states */
};



/***************************************************************************

	Performance data

***************************************************************************/

struct performance_info
{
	int						partial_updates_this_frame; /* # of partial updates last frame */
};



/***************************************************************************

	Globals referencing the current machine and the global options

***************************************************************************/

extern struct GameOptions options;
extern struct RunningMachine *Machine;



/***************************************************************************

	Function prototypes

***************************************************************************/

/* ----- core system management ----- */

/* execute a given game by index in the drivers[] array */
int run_game(int game);

/* construct a machine driver */
struct InternalMachineDriver;
void expand_machine_driver(void (*constructor)(struct InternalMachineDriver *), struct InternalMachineDriver *output);

/* ----- screen rendering and management ----- */

/* set the current visible area of the screen bitmap */
void set_visible_area(int min_x, int max_x, int min_y, int max_y);

/* force an erase and a complete redraw of the video next frame */
void schedule_full_refresh(void);

/* called by cpuexec.c to reset updates at the end of VBLANK */
void reset_partial_updates(void);

/* force a partial update of the screen up to and including the requested scanline */
void force_partial_update(int scanline);

/* finish updating the screen for this frame */
void draw_screen(void);

/* update the video by calling down to the OSD layer */
void update_video_and_audio(void);

/* update the screen, handling frame skipping and rendering */
/* (this calls draw_screen and update_video_and_audio) */
int updatescreen(void);

void mame_done(void);


/* ----- miscellaneous bits & pieces ----- */

/* set the state of a given LED */
void set_led_status(int num, int on);

/* return current performance data */
const struct performance_info *mame_get_performance_info(void);

/* return the index of the given CPU, or -1 if not found */
int mame_find_cpu_index(const char *tag);

#endif
