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
 * File: iomain.c
 *
 * This is the main file for I/O manager .
 */

/*
 * Main I/O thread. Here we create threads for each device that needs ISR.
 * They have highest priority and the entry point is driver's initialization
 * routine.
 */

#include "blkdev.h"
#include "chrdev.h"
 
int io_main()
{
//	printf("\nio initializing \n");
	blk_init();
	chr_init();
	
	return 0;
}
