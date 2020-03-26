/* cpuload-config.h - header file for a CPU load measurement feature for an RTOS
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
