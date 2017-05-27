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

#ifndef CAL_OBJ_HDR
#define CAL_OBJ_HDR

#include <SoapySDR/Device.hpp>
#include <SoapySDR/Types.hpp>
#include <SoapySDR/Logger.hpp>
#include <SoapySDR/Version.hpp>

#include "Params.hxx"

#include <boost/format.hpp>
#include <boost/program_options.hpp>
#include <string>
#include <iostream>

 /**
  * This class handles command line parameters and built-ins. 
  */
class CalObject {
public:
  /**
   * @brief Constructor
   *
   * @param argc count of command line arguments
   * @param argv pointer to list of parameter strings
   */
  CalObject(Params & params); 
  ~CalObject(); 

  bool doExp(double freq); 
private:

  bool doTXExp(double freq); 

  bool doRXExp(double freq); 

  SoapySDR::Device * dev;
  SoapySDR::Stream * rx_stream;
  SoapySDR::Stream * tx_stream;   

  std::vector<std::complex<float> > rx_buffer; 
  std::vector<std::complex<float> > env_on;
  std::vector<std::complex<float> > env_off; 
  double tx_offset; 

  std::complex<float> vectorMean(std::vector<std::complex<float> > & vec);
  std::complex<float> productMean(std::vector<std::complex<float> > & vec0, 
				  std::vector<std::complex<float> > & vec1); 

  std::complex<float> calcIQBalance(std::vector<std::complex<float> > & vec);

  bool getRXBuffer(std::vector<std::complex<float> > & vec); 

  bool sendEnvelope(std::vector<std::complex<float> > & vec);

  bool dumpBuffer(const std::string & ofname, std::vector<std::complex<float> > & vec);

  bool save_rx_result, save_tx_result; 
};

#endif
