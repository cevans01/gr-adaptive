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
    * \brief Return a shared_ptr to a new instance of adaptive::lms_filter_cc.
    *
    * To avoid accidental use of raw pointers, adaptive::lms_filter_cc's
    * constructor is in a private implementation
    * class. adaptive::lms_filter_cc::make is the public interface for
    * creating new instances.
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

