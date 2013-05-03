/*
 * FS and I/O manager for XXX OS
 * Copyright (C) 2004, 2005	Manish Regmi (manish underscore regmi at hotmail dot com)
 *				            Rajesh R.C (rajee5283 at hotmail dot com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
/*
 * File: io.h
 *
 * basic i/o primitives.
 */

#ifndef __FSIO_IO_H
#define __FSIO_IO_H


/* data for devices requiring IRQ events */
struct devtable{
	char name[8];	/* name of the driver */

	void (*init)(void);	/* constructor for that device driver */
};

struct devtable dev[] = {
						{"keyb", keyb_init},
						{"HDD", ide_init}
						{"NULL", NULL}
					};

#endif	/* __FSIO_IO_H */