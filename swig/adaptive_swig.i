/* -*- c++ -*- */

#define ADAPTIVE_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "adaptive_swig_doc.i"

%{
#include "adaptive/lms_filter_cc.h"
#include "adaptive/normalized_lms_filter_cc.h"
%}


%include "adaptive/lms_filter_cc.h"
GR_SWIG_BLOCK_MAGIC2(adaptive, lms_filter_cc);

%include "adaptive/normalized_lms_filter_cc.h"
GR_SWIG_BLOCK_MAGIC2(adaptive, normalized_lms_filter_cc);
