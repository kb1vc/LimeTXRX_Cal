/*
Copyright (c) 2017, Matthew H. Reilly (kb1vc)
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

#include "Params.hxx"
#include <iostream>

Params::Params(int argc, char * argv[])
{
  namespace po = boost::program_options;
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "help message")
    ("radioargs", po::value<std::string>(&radio_args)->default_value("driver=lime"),
     "extra parameters to pass to device creator (e.g. device id, address, type)")
    ("freq_start", po::value<double>(&freq_start)->default_value(10e6),
     "First frequency for search")
    ("freq_stop", po::value<double>(&freq_stop)->default_value(1e9),
     "Last frequency for search")
    ("freq_step", po::value<double>(&freq_step)->default_value(1e6),
     "Frequency increment between measurements")

    ("tx_offset", po::value<double>(&tx_offset)->default_value(1e3), 
     "TX frequency is <tx_offset> above RX frequency")

    ("sample_rate", po::value<double>(&sample_rate)->default_value(625000), 
     "Sample rate from SDR to host")

    ("clock_rate", po::value<double>(&clock_rate)->default_value(40e6), 
     "Master clock rate")
    

    ("tx_g_pad", po::value<double>(&tx_g_pad)->default_value(-30.0), 
     "TX Gain for output amplifier")

    ("rx_g_lna", po::value<double>(&rx_g_lna)->default_value(0.0), 
     "RX Gain for front-end  amplifier")
    ("rx_g_bb", po::value<double>(&rx_g_tia)->default_value(0.0), 
     "RX Gain for amplifier at mixer output")
    ("rx_g_pga", po::value<double>(&rx_g_pga)->default_value(0.0), 
     "RX Gain for amplifier at ADC input")


    ;

  po::store(po::parse_command_line(argc, argv, desc), pmap);
  po::notify(pmap);

  // do we need a help message?
  if(pmap.count("help")) {
    std::cout << "SoapySDR_TXRX_Cal Calculate IQ and DC balance settings\n"
	      << "and store them in the calibration database.\n"
	      << desc << std::endl;
    exit(-1); 
  }
}

