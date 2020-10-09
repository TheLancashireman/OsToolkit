/* cpuload.c - a "background task" that measures CPU load
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
#include "cpuload.h"

/* calc_rolling() - calculate the interval average and rolling average
 *
 * This function is called whenever an interval gets filled up.
 * Actions performed:
 * 	- Adjust the rolling sum downwards by the interval that's about to be evicted from the array.
 *  - Calculate the average for the interval and store it, overwriting the average from the evicted interval.
 *	- Add the new average to the rolling sum.
 *	- Increment the rolling index modulo n_intervals
 *	- Set the busy time to zero.
 *	- Calculate a new window average and update the peak hold if necessary.
*/
static void calc_rolling(cpuload_t *cl)
{
	cl->rolling_sum -= cl->rolling[cl->rolling_i];
	cl->rolling[cl->rolling_i] = (u32_t)((cl->t_busy * CL_SCALE) / cl->t_interval);
	cl->rolling_sum += cl->rolling[cl->rolling_i];
	cl->rolling_i++;
	if ( cl->rolling_i >= CL_N_INTERVALS )
		cl->rolling_i = 0;
	cl->t_busy = 0;

	cl->average = cl->rolling_sum / CL_N_INTERVALS;
	if ( cl->peak < cl->average )
		cl->peak = cl->average;

	cl_Callout(cl);
}

/* cl_LogLoad() - at given timestamp, logs given amount of busy time
*/
static void cl_LogLoad(cpuload_t *cl, u64_t t, u64_t busy)
{
	CL_DEBUG(printf("cl_LogLoad(%u, %u)\n", t & 0xffffffff, busy & 0xffffffff))
	u64_t interval_left = cl->t_next - (t - busy);

	while ( t >= cl->t_next )
	{
		/* interval elapsed */
		if (interval_left <= busy)
		{
			cl->t_busy += interval_left;
			busy -= interval_left;
		}
		else
		{
			cl->t_busy += busy;
			busy = 0;
		}
		calc_rolling(cl);
		cl->t_next += cl->t_interval;
		interval_left = cl->t_interval;
		cl->t_busy = 0;
	}

	cl->t_busy += busy;
}

/* cl_Init() - initialize the cpuload_t structure with given initial timestamp
 *
*/
static void cl_Init(cpuload_t *cl, u64_t t)
{
	cl->t_window	= (u64_t)CL_INTERVAL * (u64_t)CL_N_INTERVALS;	/* Length of a window, in ticks. */
	cl->t_interval	= (u64_t)CL_INTERVAL;							/* Length of an interval, in ticks */
	cl->t_threshold	= (u64_t)CL_THRESHOLD;							/* Boundary value for decision: busy or idle */
	cl->t_next		= t + (u64_t)CL_INTERVAL;						/* Start of next interval */

	cl->t_busy = 0;			/* Amount of current interval that was busy */
	cl->rolling_sum = 0;	/* Sum of the rolling array */
	cl->rolling_i = 0;		/* Next position in the rolling array */
	cl->average = CL_SCALE;	/* Most recent average. = rolling_sum/CL_N_INTERVAL */
	cl->peak = 0;			/* Highest average seen */

	for ( int i=0; i<CL_N_INTERVALS; i++ )
	{
		cl->rolling[i] = 0;	/* Percent busy in each interval */
	}
}

/* cl_IdleLoop() - idle-loop function that calculates CPU load
 *
 * This function runs continuously at the lowest priority
 *
 * CPU load is defined as the percentage of time that is spent *not* executing this function
 * To achieve this, we use two times:
 *		t1 = start of a potentially busy interval
 *		t2 = start of an idle interval. The calculation time is counted as idle.
 * t2-t1 is the length of the potentially busy interval. If the interval is short, it is ignored.
 *
*/
void cl_IdleLoop(void)
{
	cpuload_t cl;
	u64_t t1, t2;	/* Time markers */
	u64_t te;		/* Elapsed time */

	cl_Disable();
	t2 = cl_ReadTime();

	cl_Init(&cl, t2);

	for (;;)
	{
		t1 = cl_ReadTime();
		cl_Enable();
		cl_Barrier();
		cl_Disable();
		t2 = cl_ReadTime();

		te = t2 - t1;				/* Time spent while interruptions possible */

		if ( te > cl.t_threshold )	/* Log measured time as idle or busy, depending on length */
			cl_LogLoad(&cl, t2, te);
		else
			cl_LogLoad(&cl, t2, 0U);
	}
}
