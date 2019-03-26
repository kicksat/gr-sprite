/* -*- c++ -*- */
/* MIT License
 * 
 * Copyright (c) 2019 KickSat
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "single_correlator_cf_impl.h"
#include <gnuradio/gr_complex.h>
#include <gnuradio/fft/fft.h>
#include <complex>
#include <cmath>

namespace gr {
  namespace sprite {

    single_correlator_cf::sptr
    single_correlator_cf::make(int prn_id)
    {
      return gnuradio::get_initial_sptr
        (new single_correlator_cf_impl(prn_id));
    }

    /*
     * The private constructor
     */
    single_correlator_cf_impl::single_correlator_cf_impl(int prn_id)
      : gr::sync_block("single_correlator_cf",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(float)))
    {
      set_history(SPRITE_PRN_LENGTH);

      generate_prn(prn_id);
  
      cc430_modulator(m_prn, m_template);
      for (int k = 0; k < SPRITE_PRN_LENGTH; k++)
      {
        m_template[k] = conj(m_template[k]);
      }
      
      m_fft = new fft::fft_complex(SPRITE_PRN_LENGTH, true, 1);
      m_fft_buffer_in = m_fft->get_inbuf();
      m_fft_buffer_out = m_fft->get_outbuf();
    }

    /*
     * Our virtual destructor.
     */
    single_correlator_cf_impl::~single_correlator_cf_impl()
    {
    }

    void single_correlator_cf_impl::cc430_modulator(int* prnBits, gr_complex* baseBand)
    {
      float* diffs = m_buffer_real1;
      float* iBB = m_buffer_real2;
      float* qBB = m_buffer_real3;
      
      //Differentially encode with +/-1 values
      diffs[0] = -2*prnBits[0] + 1;
      for (int k = 1; k < SPRITE_PRN_LENGTH; k++)
      {
        char diff = prnBits[k]-prnBits[k-1];
        if(diff == 0)
        {
          diffs[k] = 1;
        }
        else
        {
          diffs[k] = -1;
        }
      }
      
      //Initialize with offset between I and Q
      iBB[0] = 1;
      qBB[0] = diffs[0];
      qBB[1] = diffs[0];
      
      for(int k = 1; k < SPRITE_PRN_LENGTH-2; k+=2)
      {
        iBB[k] = diffs[k]*iBB[k-1];
        iBB[k+1] = iBB[k];
      }
      iBB[SPRITE_PRN_LENGTH-1] = diffs[SPRITE_PRN_LENGTH-1]*iBB[SPRITE_PRN_LENGTH-2];
      
      for(int k = 2; k < SPRITE_PRN_LENGTH; k+=2)
      {
        qBB[k] = diffs[k]*qBB[k-1];
        qBB[k+1] = qBB[k];
      }
      
      for(int k = 0; k < SPRITE_PRN_LENGTH; k++)
      {
        baseBand[k] = iBB[k]*cos(M_PI/2*k) + std::complex<double>{0, qBB[k]*sin(M_PI/2*k)};
      }
    }

    void single_correlator_cf_impl::generate_prn(int prn_id)
    {
      if(prn_id == -2)
      { 
        //Deep copy M-sequence
        for (int k = 0; k < M_SEQUENCE_LENGTH; k++)
        {
          m_prn[k] = mseq1[k];
        }
      }
      else if(prn_id == -1)
      { 
        //Deep copy M-sequence
        for (int k = 0; k < M_SEQUENCE_LENGTH; k++)
        {
          m_prn[k] = mseq2[k];
        }
      }
      else //if(prn_id >= 0 && prn_id < M_SEQUENCE_LENGTH)
      { 
        //Generate Gold Codes by xor'ing 2 M-sequences in different phases
        for (int k = 0; k < M_SEQUENCE_LENGTH-prn_id; k++)
        {
          m_prn[k] = mseq1[k] ^ mseq2[k+prn_id];
        }
        for (int k = M_SEQUENCE_LENGTH-prn_id; k < M_SEQUENCE_LENGTH; k++)
        {
          m_prn[k] = mseq1[k] ^ mseq2[k-M_SEQUENCE_LENGTH+prn_id];
        }
      }

      m_prn[SPRITE_PRN_LENGTH-1] = 0; //To pad out the last byte, add a zero to the end

    }

    

    int
    single_correlator_cf_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
        const gr_complex *in = (const gr_complex *) input_items[0];
        float *out = (float *) output_items[0];

        for(int k = 0; k < noutput_items; ++k) {
          
          //Pointwise multiply by baseband template and copy to fft input
          for (int j = 0; j < SPRITE_PRN_LENGTH; ++j)
          {
            m_fft_buffer_in[j] = m_template[j]*in[j+k];
          }
          
          //Take FFT
          m_fft->execute();
          
          //Sum power
          float pow = 0.0;
          for (int j = 0; j < SPRITE_PRN_LENGTH; ++j)
          {
            pow += real(m_fft_buffer_out[j]*conj(m_fft_buffer_out[j]));
          }
          
          out[k] = pow;
        }

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace sprite */
} /* namespace gr */

