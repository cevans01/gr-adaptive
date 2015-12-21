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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "lms_filter_cc_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/misc.h>
#include <volk/volk.h>

namespace gr {
namespace adaptive {

using namespace filter::kernel;

lms_filter_cc::sptr lms_filter_cc::make(int num_taps, float mu, int sps)
{
    return gnuradio::get_initial_sptr (new lms_filter_cc_impl(num_taps, mu, sps));
}

lms_filter_cc_impl::lms_filter_cc_impl(int num_taps, float mu, int sps):
    sync_decimator("lms_filter_cc",
        gr::io_signature::make(2, 2, sizeof(gr_complex)),
        gr::io_signature::make(1, 1, sizeof(gr_complex)),
        sps),
    fir_filter_ccc(sps, std::vector<gr_complex>(num_taps, gr_complex(0,0))),
	_new_taps(num_taps, gr_complex(0,0)),
	_updated(false)
{
    set_mu(mu);
    if(num_taps > 0) {
	    _new_taps[num_taps-1] = 0.95;
    }
    fir_filter_ccc::set_taps(_new_taps);

    const int alignment_multiple = volk_get_alignment() / sizeof(gr_complex);
    set_alignment(std::max(1,alignment_multiple));

    set_history(num_taps);
}

lms_filter_cc_impl::~lms_filter_cc_impl()
{
}

gr_complex lms_filter_cc_impl::error(const gr_complex& decision, const gr_complex& estimate)
{
    gr_complex error = decision - estimate;
    return error;
}

void lms_filter_cc_impl::update_tap(gr_complex &tap, const gr_complex &in)
{
    //std::cout << "in update_tap" << std::endl;
    gr_complex neg_one = -1.0;
    //tap = neg_one * conj(conj(tap) - _mu*conj(in)*(_error));
    tap = conj(conj(tap) + _mu*in*conj(_error));
    //tap += _mu*in*conj(_error);
    //tap += _mu*in*conj(_error);
}

std::vector<gr_complex> lms_filter_cc_impl::get_taps() const
{
    return d_taps;
}

void lms_filter_cc_impl::set_taps(const std::vector<gr_complex> &taps)
{
    _new_taps = taps;
    _updated = true;
}

void lms_filter_cc_impl::print_taps(const std::vector<gr_complex> &taps)
{
    std::cout << "Taps= [";
    for(int i=0; i<taps.size()-2; i++) {
        std::cout << taps[i] << ", ";
    }
    std::cout << taps[taps.size()-1] << "]" << std::endl;
}

float lms_filter_cc_impl::mu() const
{
    return _mu;
}

void lms_filter_cc_impl::set_mu(float mu)
{
    std::cout << "setting mu=" << mu << std::endl;
    _mu = mu;
}



int lms_filter_cc_impl::work (int noutput_items,
       gr_vector_const_void_star &input_items,
       gr_vector_void_star &output_items)
{
    const gr_complex *soi_snoi = (const gr_complex *) input_items[0];
    const gr_complex *snoi = (const gr_complex *) input_items[1];
    gr_complex *out = (gr_complex *) output_items[0];
    gr_complex estimate;

    if(_updated) {
        // History requirements may have changed.
        d_taps = _new_taps;
        set_history(d_taps.size());
        _updated = false;
        return 0;
    }


    // TODO: make decimation work
    //int j = 0;
    size_t k, l = d_taps.size();
    for(int i = 0; i < noutput_items; i++) {
        estimate = filter(&snoi[i]);

        // Adjust taps
        _error = error(soi_snoi[i], estimate);
        for(k = 0; k < l; k++) {
            // Update tap locally from error.
            update_tap(d_taps[k], snoi[i+k]);

            // Update aligned taps in filter object.
            fir_filter_ccc::update_tap(d_taps[k], k);
        }
        out[i] = _error;
        //j += decimation();
    }
    if (((_print_counter++)%1000) == 0) {
        print_taps(d_taps);
    }

    // Tell runtime system how many output items we produced.
    return noutput_items;
}

} /* namespace adaptive */
} /* namespace gr */

