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


