# gr-adaptive

GNU Radio module with several blocks for doing co-channel interference cancellation.

This module exposes four blocks:
    * *lms_filter_ccc* Uses a least means squares algorithm
    * *normalized_lms_filter_cc* Uses the same least means squares algorithm as above but automatically adjusts the step size using the power of the input signals
    * *(Least squres filter not implemented yet)*
    * *(Recursive Least Squares not implemented yet)*

![lms filter](lms_filter.png "LMS Filter design")


The two inputs of each block are "SNOI" (Signal Not Of Interest) and SOI-SNOI
(The sum of SNOI and the Signal Of Interest). The algorithm is a Least Means
Squares filter, which is capable of subtracting out the SNOI from SOI-SNOI even
if the interfering signal has a different phase, amplitude, or group delay. In
fact, the group delay need not even be an integral sample delay--each algorithm
will construct a subsample linear-phase delay filter if necessary to correct
out sub-sample group delays.

Note that the block cannot correct for group delays longer than (num_taps) /
(sample_rate)

For the LMS block, the gradient descent parameter (mu) must be chosen
carefully. If mu is too small, the filter will take a long time to converge on
a solution, and will not be able to keep up with a changing channel. If mu is
too large, the solution may never converge. Additionally, runaway is possible.
If mu is too large, the FIR filter taps may grow unbounded.  This will
eventually cause floating point overflow, which will likely cause your gnuradio
flowgraph to seem to halt. Consider using the Normalized LMS Filter if this is
a strong concern.
 
