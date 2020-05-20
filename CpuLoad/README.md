# CPU load measurement

For some embedded systems it is important to measure how much CPU time is used and, by extension,
how much is available for new features etc.

This set of source files can help.

Assumptions: your system uses real-time scheduling or something simlar, where tasks or threads of
higher priority completely block a task or thread of lower priority. Systems with scheduling algorithms
that provide a percentage of available CPU time based on priority cannot be measured using
this method.

CPU load is measured over intervals of configurable duration. At the end of each interval
the load over the whole interval is calculated as a fraction with a configurable scaling
(100ths, 1000ths etc.).

The average load over a configurable number of intervals is computed. This is a rolling average -
the average is always taken over the most recent set of intervals.

Finally, the peak load is stored. This is the highest average load ever seen.

Whenever an interval load is computed, a callout function is called to report the load.

## How it works

The idle function executes in a background task or thread. It has two phases:

* Calculation phase (interrupts are disabled)
* Load phase (interrupts are enabled)

The time spent in each phase is measured and fed in to the calculation functions that run during
the calculation phase. The time spent in the calculation phase is always recorded as "idle time".
The time spend in the load phase is recorded as "idle time" if it is less than a configurable
threshold; otherwise it is recorded as "busy time".


## License, disclaimer etc.

Copyright David Haworth

This file is part of Dave's OS toolkit.

Dave's OS toolkit is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Dave's OS toolkit is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Dave's OS toolkit.  If not, see <http://www.gnu.org/licenses/>.


