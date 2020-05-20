/* cpuload-config.h - header file for a CPU load measurement feature for an RTOS
 *
 * Copyright David Haworth
 *
 * This file is part of Dave's OS toolkit.
 *
 * Dave's OS toolkit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Dave's OS toolkit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Dave's OS toolkit.  If not, see <http://www.gnu.org/licenses/>.
 *
 * This file *must* define at least 3 macros:
 *	CL_ENV			= the name of an environment header file in quotes
 *	CL_INTERVAL		= the duration of an interval in ticks of the timer
 *	CN_N_INTERVAL	= the number of intervals in the sliding window
 *	CN_THRESHOLD	= the number of ticks, above which the interval is considered as "busy"
 *
 *	You may also define:
 *	CL_NBITS		= number of bits in a long integer (32 or 64) [default is in the env header]
 *	CL_SCALE		= scaling factor. The number you define is the number you get at 100% load. [default: 1000]
*/
#ifndef CPULOAD_CONFIG_H
#define CPULOAD_CONFIG_H	1

#if 1					/* Special setup for testing with artificially-injected times on a Linux host */

#define CL_ENV			"cpuload-env-linux.h"
#define CL_INTERVAL		10000000
#define CL_N_INTERVALS	10
#define CL_THRESHOLD	1000

#if 0
#define CL_DEBUG(x)		x;		/* Enable debugging */
#endif

#endif


#endif
