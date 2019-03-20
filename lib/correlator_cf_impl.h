/* -*- c++ -*- */
/* 
 * Copyright 2014 <+YOU OR YOUR COMPANY+>.
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

#ifndef INCLUDED_SPRITE_CORRELATOR_CF_IMPL_H
#define INCLUDED_SPRITE_CORRELATOR_CF_IMPL_H

#include <sprite/correlator_cf.h>
#include <gnuradio/gr_complex.h>
#include <gnuradio/fft/fft.h>

#define M_SEQUENCE_LENGTH 511
#define SPRITE_PRN_LENGTH 512

namespace gr {
  namespace sprite {

    class correlator_cf_impl : public correlator_cf
    {
      private:
        static int mseq1[M_SEQUENCE_LENGTH];
        static int mseq2[M_SEQUENCE_LENGTH];

        int m_prn0[SPRITE_PRN_LENGTH];
	      int m_prn1[SPRITE_PRN_LENGTH];
        void generate_prns(int prn_id0, int prn_id1);
          
        gr_complex m_template0[SPRITE_PRN_LENGTH];
        gr_complex m_template1[SPRITE_PRN_LENGTH];
        void cc430_modulator(int* prnBits, gr_complex* baseBand);
          
        float m_buffer_real1[SPRITE_PRN_LENGTH];
        float m_buffer_real2[SPRITE_PRN_LENGTH];
        float m_buffer_real3[SPRITE_PRN_LENGTH];
          
        gr_complex* m_fft_buffer_in0;
        gr_complex* m_fft_buffer_out0;
          
        fft::fft_complex* m_fft0;

        gr_complex* m_fft_buffer_in1;
        gr_complex* m_fft_buffer_out1;
          
        fft::fft_complex* m_fft1;

      public:
        correlator_cf_impl(int prn_id0, int prn_id1);
        ~correlator_cf_impl();

        // Where all the action really happens
        int work(int noutput_items,
	        gr_vector_const_void_star &input_items,
	        gr_vector_void_star &output_items);
    };

    int correlator_cf_impl::mseq1[] = {
      1,0,1,0,1,0,1,0,1,0,0,0,0,0,0,1,0,1,0,0,1,0,1,0,1,1,1,1,0,0,1,0,
      1,1,1,0,1,1,1,0,0,0,0,0,0,1,1,1,0,0,1,1,1,0,1,0,0,1,0,0,1,1,1,1,
      0,1,0,1,1,1,0,1,0,1,0,0,0,1,0,0,1,0,0,0,0,1,1,0,0,1,1,1,0,0,0,0,
      1,0,1,1,1,1,0,1,1,0,1,1,0,0,1,1,0,1,0,0,0,0,1,1,1,0,1,1,1,1,0,0,
      0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,0,1,1,1,1,1,0,0,0,1,0,1,
      1,1,0,0,1,1,0,0,1,0,0,0,0,0,1,0,0,1,0,1,0,0,1,1,1,0,1,1,0,1,0,0,
      0,1,1,1,1,0,0,1,1,1,1,1,0,0,1,1,0,1,1,0,0,0,1,0,1,0,1,0,0,1,0,0,
      0,1,1,1,0,0,0,1,1,0,1,1,0,1,0,1,0,1,1,1,0,0,0,1,0,0,1,1,0,0,0,1,
      0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,1,1,0,0,0,0,1,0,0,1,1,
      1,0,0,1,0,1,0,1,0,1,1,0,0,0,0,1,1,0,1,1,1,1,0,1,0,0,1,1,0,1,1,1,
      0,0,1,0,0,0,1,0,1,0,0,0,0,1,0,1,0,1,1,0,1,0,0,1,1,1,1,1,1,0,1,1,
      0,0,1,0,0,1,0,0,1,0,1,1,0,1,1,1,1,1,1,0,0,1,0,0,1,1,0,1,0,1,0,0,
      1,1,0,0,1,1,0,0,0,0,0,0,0,1,1,0,0,0,1,1,0,0,1,0,1,0,0,0,1,1,0,1,
      0,0,1,0,1,1,1,1,1,1,1,0,1,0,0,0,1,0,1,1,0,0,0,1,1,1,0,1,0,1,1,0,
      0,1,0,1,1,0,0,1,1,1,1,0,0,0,1,1,1,1,1,0,1,1,1,0,1,0,0,0,0,0,1,1,
      0,1,0,1,1,0,1,1,0,1,1,1,0,1,1,0,0,0,0,0,1,0,1,1,0,1,0,1,1,1,1
    };
    
    int correlator_cf_impl::mseq2[] = {
      1,0,1,0,1,0,1,0,1,1,1,1,0,1,1,1,1,1,1,0,0,1,1,1,1,0,1,0,0,1,0,0,
      1,1,1,1,1,0,0,1,0,1,1,1,1,1,0,1,0,0,0,0,0,0,1,0,1,1,0,0,0,1,0,0,
      1,1,0,0,1,1,1,0,1,1,1,1,0,1,0,1,1,0,1,1,0,1,1,1,0,1,0,1,0,1,1,0,
      1,0,0,1,0,1,1,1,0,1,0,0,0,1,0,0,0,0,0,1,0,0,0,0,1,1,0,1,0,1,1,1,
      0,1,1,0,0,0,0,1,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,1,1,0,0,0,0,0,1,0,
      1,0,0,0,1,1,1,0,1,1,0,1,0,0,0,0,1,0,1,1,1,0,0,0,0,1,1,1,0,0,0,0,
      0,1,1,1,0,1,0,0,1,1,0,1,0,1,0,1,0,0,1,1,0,0,0,1,1,1,1,0,1,1,0,1,
      1,0,0,1,1,1,1,1,1,0,1,1,1,0,1,1,1,0,0,1,1,1,0,0,1,1,0,0,0,0,1,1,
      0,0,0,1,0,1,1,1,1,0,0,1,1,0,1,0,0,0,1,1,0,0,1,0,0,0,0,0,0,0,0,1,
      0,0,1,0,1,0,0,0,0,1,1,1,1,0,0,1,0,0,1,1,0,1,1,1,0,0,0,1,1,1,0,0,
      1,0,0,0,1,0,0,1,0,0,0,0,1,0,0,1,1,1,0,1,0,1,1,1,1,1,1,1,0,1,0,1,
      0,0,1,0,0,0,1,1,0,1,1,0,1,0,1,0,0,0,0,0,1,1,0,0,1,1,0,0,1,0,1,0,
      0,1,0,1,0,1,0,0,0,1,0,1,0,0,1,1,1,0,0,0,1,0,1,0,1,1,1,0,0,1,0,1,
      0,1,1,0,0,0,0,0,0,0,1,1,0,1,1,1,1,0,0,0,1,0,0,0,1,0,1,1,0,1,0,1,
      1,0,0,1,0,0,1,0,0,1,0,1,1,0,0,1,1,0,1,1,0,0,0,1,1,0,1,0,0,1,1,1,
      1,0,0,0,0,0,0,1,1,1,1,1,0,1,1,0,0,1,0,1,1,0,1,1,1,1,1,0,0,0,0
    };

  } // namespace sprite
} // namespace gr

#endif /* INCLUDED_SPRITE_CORRELATOR_CF_IMPL_H */

