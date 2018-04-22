/* -*- c++ -*- */
/* 
 * Copyright 2015 <+YOU OR YOUR COMPANY+>.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */


#ifndef INCLUDED_ADAPTIVE_LMS_FILTER_CC_H
#define INCLUDED_ADAPTIVE_LMS_FILTER_CC_H

#include <adaptive/api.h>
#include <gnuradio/sync_decimator.h>

namespace gr {
namespace adaptive {

/*!
 * \brief <+description of block+>
 * \ingroup adaptive
 *
 */
class ADAPTIVE_API lms_filter_cc : virtual public sync_decimator
{
    protected:
    virtual gr_complex error(const gr_complex& decision, const gr_complex& estimate) = 0;
    virtual void update_tap(gr_complex &tap, const gr_complex &in) = 0;

    public:
    typedef boost::shared_ptr<lms_filter_cc> sptr;

    /*!
     * @brief Create a new instance of adaptive::lms_filter_cc
     *
     * This block does adaptive filtering for co-channel interference
     * cancellation. The two inputs of this block are "SNOI" (Signal Not Of
     * Interest) and SOI-SNOI (The sum of SNOI and the Signal Of Interest). The
     * algorithm is a Least Means Squares filter, which is capable of
     * subtracting out the SNOI from SOI-SNOI even if the interfering signal
     * has a different phase, amplitude, or group delay. In fact, the group
     * delay need not even be an integral sample delay--the algorithm will
     * construct a subsample linear-phase delay filter if necessary to correct
     * out sub-sample group delays.
     *
     * Note that the block cannot correct for group delays longer than
     * (num_taps) / (sample_rate)
     *
     * The gradient descent parameter (mu) must be chosen carefully. If mu is
     * too small, the filter will take a long time to converge on a solution,
     * and will not be able to keep up with a changing channel. If mu is too
     * large, the solution may never converge. Additionally, runaway is
     * possible. If mu is too large, the FIR filter taps may grow unbounded.
     * This will eventually cause floating point overflow, which will likely
     * cause your gnuradio flowgraph to seem to halt. Consider using
     * normalized_lms_filter_ccc if this is a strong concern.
     *
     *
     * @param num_taps Number of taps for internal FIR filter
     * @param mu Step-size for gradient descent
     * @param sps Samples per second (currently unused, but may be an input to the FIR filter in the future)
     */
    static sptr make(int num_taps, float mu, int sps);
    virtual std::vector<gr_complex> get_taps() const = 0;
    virtual void set_taps(const std::vector<gr_complex> &taps) = 0;
    virtual float mu() const = 0;
    virtual void set_mu(float mu) = 0;
};

} // namespace adaptive
} // namespace gr

#endif /* INCLUDED_ADAPTIVE_LMS_FILTER_CC_H */

