/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __NUCLEOS_VIDEO_H
#define __NUCLEOS_VIDEO_H

/* Definitions used by the console driver. */
#define MONO_BASE    0xB0000L   /* base of mono video memory */
#define COLOR_BASE   0xB8000L   /* base of color video memory */
#define MONO_SIZE     0x1000    /* 4K mono video memory */
#define COLOR_SIZE    0x4000    /* 16K color video memory */
#define EGA_SIZE      0x8000    /* EGA & VGA have at least 32K */
#define BLANK_COLOR   0x0700    /* determines cursor color on blank screen */
#define SCROLL_UP          0    /* scroll forward */
#define SCROLL_DOWN        1    /* scroll backward */
#define CONS_RAM_WORDS    80    /* video ram buffer size */
#define MAX_ESC_PARMS      4    /* number of escape sequence params allowed */

/* Constants relating to the controller chips. */
#define M_6845         0x3B4    /* port for 6845 mono */
#define C_6845         0x3D4    /* port for 6845 color */
#define INDEX              0    /* 6845's index register */
#define DATA               1    /* 6845's data register */
#define STATUS             6    /* 6845's status register */
#define VID_ORG           12    /* 6845's origin register */
#define CURSOR            14    /* 6845's cursor register */

/* Beeper. */
#define BEEP_FREQ     0x0533    /* value to put into timer to set beep freq */
#define B_TIME             3    /* length of CTRL-G beep is ticks */

/* definitions used for font management */
#define GA_SEQUENCER_INDEX      0x3C4
#define GA_SEQUENCER_DATA       0x3C5
#define GA_GRAPHICS_INDEX       0x3CE
#define GA_GRAPHICS_DATA        0x3CF
#define GA_VIDEO_ADDRESS        0xA0000L
#define GA_FONT_SIZE            8192

#endif /* __NUCLEOS_VIDEO_H */
