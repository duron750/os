/*
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	from: @(#)lp.local.h	5.5 (Berkeley) 6/1/90
 *	$Id: lp.local.h,v 1.4 1993/08/01 17:59:08 mycroft Exp $
 */

/*
 * Possibly, local parameters to the spooling system
 */

/*
 * Magic number mapping for binary files, used by lpr to avoid
 *   printing objects files.
 */

#include <a.out.h>
#include <ar.h>

#ifndef A_MAGIC1	/* must be a VM/UNIX system */
#	define A_MAGIC1 OMAGIC
#	define A_MAGIC2 NMAGIC
#	define A_MAGIC3 ZMAGIC
#	undef ARMAG
#	define ARMAG	0177545
#endif

/*
 * Defaults for line printer capabilities data base
 */
#define	DEFLP		"lp"
#define DEFLOCK		"lock"
#define DEFSTAT		"status"
#define	DEFMX		1000
#define DEFMAXCOPIES	0
#define DEFFF		"\f"
#define DEFWIDTH	132
#define DEFLENGTH	66
#define DEFUID		1

/*
 * When files are created in the spooling area, they are normally
 *   readable only by their owner and the spooling group.  If you
 *   want otherwise, change this mode.
 */
#define FILMOD		0660

/*
 * Printer is assumed to support LINELEN (for block chars)
 *   and background character (blank) is a space
 */
#define LINELEN		132
#define BACKGND		' '

#define HEIGHT	9		/* height of characters */
#define WIDTH	8		/* width of characters */
#define DROP	3		/* offset to drop characters with descenders */

/*
 * Define TERMCAP if the terminal capabilites are to be used for lpq.
 */
#define TERMCAP

/*
 * Maximum number of user and job requests for lpq and lprm.
 */
#define MAXUSERS	50
#define MAXREQUESTS	50
