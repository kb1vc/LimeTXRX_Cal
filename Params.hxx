/*
Copyright (c) 2017 Matthew H. Reilly (kb1vc)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

    Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in
    the documentation and/or other materials provided with the
    distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef PARAMS_HDR
#define PARAMS_HDR

#include <boost/format.hpp>
#include <boost/program_options.hpp>
#include <string>
#include <algorithm>
#include <iostream>


 /**
  * This class handles command line parameters and built-ins. 
  */
class Params {
public:
  /**
   * @brief Constructor
   *
   * @param argc count of command line arguments
   * @param argv pointer to list of parameter strings
   */
  Params(int argc, char * argv[]);

  double getStart() { return freq_start; }
  double getStop() { return freq_stop; }
  double getStep() { return freq_step; }

  double getTXOffset() { return tx_offset; }

  double getSampleRate() { return sample_rate; }

  double getClockRate() { return clock_rate; }

  enum Gain { RX_LNA, RX_TIA, RX_PGA, TX_PAD } ;

  double getGain(Gain sel)  {
    switch (sel) {
    case RX_LNA: return rx_g_lna;
    case RX_TIA: return rx_g_tia;
    case RX_PGA: return rx_g_pga;
    case TX_PAD: return tx_g_pad; 
    default: return 0.0; 
    }
  }

  std::string getRadioArgs() { return radio_args; }
private:
    
  boost::program_options::variables_map pmap;

  double sample_rate, clock_rate; 

  std::string radio_args; 
  double freq_start, freq_stop, freq_step; 
  double tx_offset; 

  double tx_g_pad, rx_g_lna, rx_g_tia, rx_g_pga; 
};

#endif
