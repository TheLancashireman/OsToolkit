/* cpuload-linux.c
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
 * This file contains functions to test the cpuload measurement functions on a linux host
*/
#include "cpuload.h"
#include <stdio.h>
#include <stdlib.h>

static int limit = (CL_N_INTERVALS * 3) + 1;	/* When to stop the test */
static int load;								/* The required test load */
static cl_u64_t busy_time;							/* Busy time needed in one interval */

static int measure = 0;
static int busy = 1;
static cl_u64_t current_timer;

int main(int argc, char **argv)
{
	if ( argc < 2 )
	{
		printf("Usage: cpuload-linux <percent-load>\n");
		exit(1);
	}
	char *param = argv[1];
	if ( param[0] < '0' || param[0] > '9' )
	{
		printf("Usage: cpuload-linux <percent-load>\n");
		exit(1);
	}

	load = atoi(param);

	if ( (load < 0) || (load > 100) )
	{
		printf("Usage: cpuload-linux <percent-load>\n");
		exit(1);
	}

	busy_time = CL_INTERVAL;
	busy_time = (busy_time * load) / 100;

	cl_IdleLoop();

	return 0;
}

void cl_Callout(cpuload_t *cl)
{
	printf("curr: %d, peak: %d\n", cl->average, cl->peak);

	limit--;
	if ( limit <= 0 )
		exit(0);
	if ( limit < CL_N_INTERVALS )
	{
		busy_time = busy_time / 2;
		if ( busy_time <= CL_THRESHOLD )
			busy_time = CL_THRESHOLD + 1;
	}

	busy = 1;
}

/* cl_Disable() - nothing to do
*/
void cl_Disable(void)
{
}

/* cl_Barrier() - set the measure flag to request a timer increment
*/
void cl_Barrier(void)
{
	measure = 1;
}

/* cl_Enable() - nothing to do
*/
void cl_Enable(void)
{
}

/* cl_ReadTime() - provide a new time value
 *
 * The first call after every call to cl_Enable() is the end time of a busy/idle period.
 * If the busy flag is set, the entire busy time is added to the timer and the busy time is cleared
 * If the busy flag is clear, an idle time is added to the timer. This continues until the end of an interval.
 * At the end of the interval, the callout function sets the busy flag back to 1.
*/
cl_u64_t cl_ReadTime(void)
{
	if ( measure )
	{
		measure = 0;

		if ( busy )
		{
			current_timer += busy_time;
			busy = 0;
		}
		else
		{
			current_timer += CL_THRESHOLD - 1;
			CL_DEBUG(printf("time = %u\n", current_timer & 0xffffffff))
		}
	}

	return current_timer;
}

/* Editor settings; DO NOT DELETE
 * vi:set ts=4:
*/
