/*
 * Libc for ManRiX OS
 * Copyright (C) 2004, 2005
 * 	Manish Regmi (regmi dot manish at gmail dot com) and
 * 	Rajesh R.C   (rajesh dot rc at gmail dot com)
 *
 * This Program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This Program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
/*
 * termios.h
 * 			define values for termios.
 */

#ifndef __TERMIOS_H
#define __TERMIOS_H

#include <sys/types.h>

#define NCCS		10

#define VTIME		0		/* TIME value. */
#define VEOF		1		/* EOF character. */
#define VEOL		2		/* EOL character. */
#define VERASE		3		/* ERASE character. */
#define VINTR		4		/* INTR character. */
#define VKILL		5		/* KILL character. */
#define VMIN		6		/* MIN value. */
#define VQUIT		7		/* QUIT character. */
#define VSTART		8		/* START character. */
#define VSTOP		9		/* STOP character. */
#define VSUSP		10		/* SUSP character. */


/* Input Modes */
/* The c_iflag field describes the basic terminal input control: */

#define BRKINT		0x01	/* Signal interrupt on break. */
#define ICRNL		0x02	/* Map CR to NL on input. */
#define IGNBRK		0x04	/* Ignore break condition. */
#define IGNCR		0x08	/* Ignore CR. */
#define IGNPAR		0x10	/* Ignore characters with parity errors. */
#define INLCR		0x20	/* Map NL to CR on input. */
#define INPCK		0x40	/* Enable input parity check. */
#define ISTRIP		0x80	/* Strip character. */
#define IXANY		0x100	/* Enable any character to restart output. */
#define IXOFF		0x200	/* Enable start/stop input control. */
#define IXON		0x400	/* Enable start/stop output control. */
#define PARMRK		0x800	/* Mark parity errors. */

/* Output Modes */
/* The c_oflag field specifies the system treatment of output: */

#define OPOST		0x001	/* Post-process output. */
#define ONLCR		0x002	/* Map NL to CR-NL on output. */
#define OCRNL		0x004	/* Map CR to NL on output. */
#define ONOCR		0x008	/* No CR output at column 0. */
#define ONLRET		0x010	/* NL performs CR function. */
#define OFILL		0x020	/* Use fill characters for delay. */
#define NLDLY		0x040	/* Select newline delays: */
#define NL0			0x080	/* Newline type 0. */
#define NL1			0x100	/* Newline type 1. */

#define CRDLY		0x200	/* Select carriage-return delays: */
#define CR0			0x400	/* Carriage-return delay type 0. */
#define CR1			0x800	/* Carriage-return delay type 1. */
#define CR2			0x1000	/* Carriage-return delay type 2. */
#define CR3			0x2000	/* Carriage-return delay type 3. */

#define TABDLY		0x4000	/* Select horizontal-tab delays: */
#define TAB0		0x8000	/* Horizontal-tab delay type 0. */
#define TAB1		0x10000	/* Horizontal-tab delay type 1. */
#define TAB2		0x20000	/* Horizontal-tab delay type 2. */
#define TAB3		0x40000	/* Expand tabs to spaces. */

#define BSDLY		0x80000		/* Select backspace delays: */
#define BS0			0x100000	/* Backspace-delay type 0. */
#define BS1			0x200000	/* Backspace-delay type 1. */

#define VTDLY		0x400000	/* Select vertical-tab delays: */
#define VT0			0x800000	/* Vertical-tab delay type 0. */
#define VT1			0x1000000	/* Vertical-tab delay type 1. */

#define FFDLY		0x2000000	/* Select form-feed delays: */
#define FF0			0x4000000	/* Form-feed delay type 0. */
#define FF1			0x8000000	/* Form-feed delay type 1. */

/* Baud Rate Selection */
#define B0					/* Hang up */
#define B50					/* 50 baud */
#define B75					/* 75 baud */
#define B110				/* 110 baud */
#define B134				/* 134.5 baud */
#define B150				/* 150 baud */
#define B200				/* 200 baud	*/
#define B300				/* 300 baud */
#define B600				/* 600 baud */
#define B1200				/* 1200 baud */
#define B1800				/* 1800 baud */
#define B2400				/* 2400 baud */
#define B4800				/* 4800 baud */
#define B9600				/* 9600 baud */
#define B19200				/* 19200 baud */
#define B38400				/* 38400 baud */

/* Control Modes */
#define CSIZE 				/* Character size:  */
#define CS5 		0x01	/* 5 bits  */
#define CS6			0x02	/* 6 bits */
#define CS7			0x04	/* 7 bits */
#define CS8			0x08	/* 8 bits */

#define CSTOPB		0x10	/* Send two stop bits, else one. */
#define CREAD		0x20	/* Enable receiver. */
#define PARENB		0x40	/* Parity enable. */
#define PARODD		0x80	/* Odd parity, else even. */
#define HUPCL		0x100	/* Hang up on last close. */
#define CLOCAL		0x200	/* Ignore modem status lines. */

/* Local Modes */
#define ECHO		0x001	/* Enable echo. */
#define ECHOE		0x002	/* Echo erase character as error-correcting backspace. */
#define ECHOK		0x004	/* Echo KILL. */
#define ECHONL		0x008	/* Echo NL. */
#define ICANON		0x010	/* Canonical input (erase and kill processing). */
#define IEXTEN		0x020	/* Enable extended input character processing. */
#define ISIG		0x040	/* Enable signals. */
#define NOFLSH		0x080	/* Disable flush after interrupt or quit. */
#define TOSTOP		0x100	/* Send SIGTTOU for background output. */

/* Attribute Selection */
#define TCSANOW 	1		/* Change attributes immediately.  */
#define TCSADRAIN 	2		/* Change attributes when output has drained.  */
#define TCSAFLUSH 	4		/* Change attributes when output has drained;
								also flush pending input.  */

/* Line Control */
/* The following symbolic constants can be used with tcflush() */
#define TCIFLUSH	1		/* Flush pending input. */
#define TCIOFLUSH	2		/* Flush both pending input and untransmitted output. */
#define TCOFLUSH	4		/* Flush untransmitted output. */

/* The following symbolic constants can be used with tcflow() */
#define TCIOFF		1		/* Transmit a STOP character, intended to suspend input data. */
#define TCION		2		/* Transmit a START character, intended to restart input data. */
#define TCOOFF		4		/* Suspend output. */
#define TCOON		8		/* Restart output. */

typedef unsigned int cc_t;
typedef unsigned int speed_t;
typedef unsigned int tcflag_t;

struct termios{
	tcflag_t  c_iflag;     /* Input modes. */
	tcflag_t  c_oflag;     /* Output modes. */
	tcflag_t  c_cflag;     /* Control modes. */
	tcflag_t  c_lflag;     /* Local modes. */
	cc_t      c_cc[NCCS];  /* Control characters. */
};


speed_t cfgetispeed(const struct termios *);
speed_t cfgetospeed(const struct termios *);
int     cfsetispeed(struct termios *, speed_t);
int     cfsetospeed(struct termios *, speed_t);
int     tcdrain(int);
int     tcflow(int, int);
int     tcflush(int, int);

/*
 * tcgetattr - get the parameters associated with the terminal
 */
int     tcgetattr(int, struct termios *);

pid_t   tcgetsid(int);

int     tcsendbreak(int, int);
int     tcsetattr(int, int, const struct termios *);


#endif  /* __TERMIOS_H  */
