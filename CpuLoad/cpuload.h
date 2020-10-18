/* cpuload.h - header file for a CPU load measurement feature for an RTOS
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
*/
#ifndef CPULOAD_H
#define CPULOAD_H	1

#include "cpuload-config.h"
#include CL_ENV

#ifndef CL_BITS
#define CL_BITS		32
#endif

#ifndef CL_SCALE
#define CL_SCALE	1000
#endif

#ifndef CL_DEBUG
#define CL_DEBUG(x)
#endif

typedef unsigned int u32_t;
typedef unsigned int bool_t;

#if CL_BITS == 64
typedef unsigned long u64_t;
#else
typedef unsigned long long u64_t;		/* Requires c99 compatibility */
#endif

typedef struct cpuload_s cpuload_t;

struct cpuload_s
{
	u64_t t_window;					/* Length of a window, in ticks. = t_interval * CL_N_INTERVAL */
	u64_t t_interval;				/* Length of an interval, in ticks */
	u64_t t_threshold;				/* Boundary value for decision: busy or idle */

	u64_t t_used;					/* Amount of current interval accounted for. */
	u64_t t_busy;					/* Amount of current interval that was busy */
	u32_t rolling[CL_N_INTERVALS];	/* Percent busy in each interval */
	u32_t rolling_sum;				/* Sum of the rolling array */
	u32_t rolling_i;				/* Next position in the rolling array */
	u32_t average;					/* Most recent average. = rolling_sum/CL_N_INTERVAL */
	u32_t peak;						/* Highest average seen */
};

/* Call this function from your idle task/thread
*/
extern void cl_IdleLoop(void);

/* Provide this function to report new values
*/
extern void cl_Callout(cpuload_t *);

/* The following functions are provided by the environment in which you use the program
*/
extern u64_t cl_ReadTime(void);
extern void cl_Disable(void);
extern void cl_Enable(void);
extern void cl_Barrier(void);

#endif
