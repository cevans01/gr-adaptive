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

#include "normalized_lms_filter_cc_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/misc.h>
#include <volk/volk.h>
#include <limits>

namespace gr {
namespace adaptive {

using namespace filter::kernel;

normalized_lms_filter_cc::sptr normalized_lms_filter_cc::make(int num_taps, int sps)
{
    return gnuradio::get_initial_sptr (new normalized_lms_filter_cc_impl(num_taps, sps));
}

normalized_lms_filter_cc_impl::normalized_lms_filter_cc_impl(int num_taps, int sps):
    sync_decimator("normalized_lms_filter_cc",
        gr::io_signature::make(2, 2, sizeof(gr_complex)),
        gr::io_signature::make(1, 1, sizeof(gr_complex)),
        sps),
    fir_filter_ccc(sps, std::vector<gr_complex>(num_taps, gr_complex(0,0))),
	_new_taps(num_taps, gr_complex(0,0)),
	_updated(false)
{
    set_mu(0.0);
    _snoi_pwr = std::numeric_limits<float>::max();
    _print_counter = 0;
    if(num_taps > 0) {
	    _new_taps[num_taps-1] = 0.95;
    }
    fir_filter_ccc::set_taps(_new_taps);

    const int alignment_multiple = volk_get_alignment() / sizeof(gr_complex);
    set_alignment(std::max(1,alignment_multiple));

    set_history(num_taps);
}

normalized_lms_filter_cc_impl::~normalized_lms_filter_cc_impl()
{
}

gr_complex normalized_lms_filter_cc_impl::error(const gr_complex& decision, const gr_complex& estimate)
{
    gr_complex error = decision - estimate;
    return error;
}

void normalized_lms_filter_cc_impl::update_tap(gr_complex &tap, const gr_complex &in, const float& snoi_pwr)
{
    //std::cout << "in update_tap" << std::endl;
    gr_complex neg_one = -1.0;
    //tap = neg_one * conj(conj(tap) - _mu*conj(in)*(_error));
    tap = conj(conj(tap) + ((in*conj(_error))/snoi_pwr));
    //tap += _mu*in*conj(_error);
    //tap += _mu*in*conj(_error);
}

std::vector<gr_complex> normalized_lms_filter_cc_impl::get_taps() const
{
    return d_taps;
}

void normalized_lms_filter_cc_impl::set_taps(const std::vector<gr_complex> &taps)
{
    _new_taps = taps;
    _updated = true;
}

void normalized_lms_filter_cc_impl::print_taps(const std::vector<gr_complex> &taps)
{
    std::cout << "Taps= [";
    for(int i=0; i<taps.size()-2; i++) {
        std::cout << taps[i] << ", ";
    }
    std::cout << taps[taps.size()-1] << "]" << std::endl;
}

float normalized_lms_filter_cc_impl::mu() const
{
    return _mu;
}

void normalized_lms_filter_cc_impl::set_mu(float mu)
{
    std::cout << "setting mu=" << mu << std::endl;
    _mu = mu;
}



int normalized_lms_filter_cc_impl::work (int noutput_items,
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

    if (((_print_counter++)<10)) {
        print_taps(d_taps);
        std::cout << "snoi_power=" << std::real(_snoi_pwr) << std::endl;
    }

    // TODO: make decimation work
    //int j = 0;
    size_t k, l = d_taps.size();
    for(int i = 0; i < noutput_items; i++) {
        estimate = filter(&snoi[i]);

        // Adjust taps
        _error = error(soi_snoi[i], estimate);
        
        if (noutput_items - i > k) {
            volk_32fc_x2_conjugate_dot_prod_32fc(&_snoi_pwr, &snoi[i], &snoi[i], k);
        }

        for(k = 0; k < l; k++) {
            // Update tap locally from error.
            update_tap(d_taps[k], snoi[i+k], std::real(_snoi_pwr));

            // Update aligned taps in filter object.
            fir_filter_ccc::update_tap(d_taps[k], k);
        }
        out[i] = _error;
        //j += decimation();
    }

    // Tell runtime system how many output items we produced.
    return noutput_items;
}

} /* namespace adaptive */
} /* namespace gr */

