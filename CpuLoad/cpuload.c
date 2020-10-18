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

/* cl_LogLoad() - advances measurement time, logs load as busy or idle
*/
static void cl_LogLoad(cpuload_t *cl, u64_t t, bool_t busy)
{
	CL_DEBUG(printf("cl_LogBusy(%u, %s)\n", t & 0xffffffff, (b ? "busy" : "idle")))
	u64_t interval_left = cl->t_interval - cl->t_used;

	while ( t >= interval_left )
	{
		cl->t_used += interval_left;
		if (busy)
		{
			cl->t_busy += interval_left;
		}
		calc_rolling(cl);
		t -= interval_left;
		interval_left = cl->t_interval;
		cl->t_used = 0;
		cl->t_busy = 0;
	}

	cl->t_used += t;
	if (busy)
	{
		cl->t_busy += t;
	}
}

/* cl_Init() - initialize the cpuload_t structure
 *
*/
static void cl_Init(cpuload_t *cl)
{
	cl->t_window	= (u64_t)CL_INTERVAL * (u64_t)CL_N_INTERVALS;	/* Length of a window, in ticks. */
	cl->t_interval	= (u64_t)CL_INTERVAL;							/* Length of an interval, in ticks */
	cl->t_threshold	= (u64_t)CL_THRESHOLD;							/* Boundary value for decision: busy or idle */

	cl->t_used = 0;			/* Amount of current interval accounted for. */
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
 *		ti = start of an idle interval
 *		t1 = start of a potentially busy interval
 *		t2 = end of a potentially busy interval
 *
 * The time from ti to t1 (t1-ti) is always counted as idle.
 * The time from t1 to t2 (t2-t1) may be busy or idle depending on how long.
 * -> If shorter than the threshold it is counted as idle. If the idle time (t2-ti) is longer than an interval,
 *    it is logged.
 * -> If longer than the threshold, it is counted as busy and logged. But first, the preceding idle interval is logged.
 * After logging some time (either idle/busy or just idle) ti is set to t2.
*/
void cl_IdleLoop(void)
{
	cpuload_t cl;
	u64_t t1, t2;	/* Time markers */
	u64_t ti;		/* Start of idle period */

	cl_Init(&cl);

	cl_Disable();

	/* Start of first idle interval.
	*/
	ti = cl_ReadTime();

	for (;;)
	{
		t1 = cl_ReadTime();
		cl_Enable();
		cl_Barrier();
		cl_Disable();
		t2 = cl_ReadTime();

		/* Busy or idle?
		*/
		if ( (t2 - t1) > cl.t_threshold )
		{
			/* Busy from t1 to t2, but time from ti to t1 was idle.
			 * Log the busy and idle intervals separately.
			*/
			cl_LogLoad(&cl, (t1 - ti), 0);
			cl_LogLoad(&cl, (t2 - t1), 1);

			/* Start a new idle interval
			*/
			ti = t2;
		}
		else
		if ( (ti - t2) >= cl.t_interval )
		{
			/* Total time from ti to t2 was idle.
			*/
			cl_LogLoad(&cl, (ti-t2), 0);

			/* Start a new idle interval
			*/
			ti = t2;
		}
	}
}
