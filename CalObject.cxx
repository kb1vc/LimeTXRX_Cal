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

#include "CalObject.hxx"
#include <iostream>
#include <fstream>
#include <SoapySDR/Device.hpp>
#include <SoapySDR/Types.hpp>
#include <SoapySDR/Logger.hpp>
#include <SoapySDR/Errors.hpp>


CalObject::CalObject(Params & params) {

  int stat; 

  dev = SoapySDR::Device::make(params.getRadioArgs()); 

  if(dev == NULL) {
    std::cerr << boost::format("Could not open SoapySDR device with identifier [%s]\n")
      % params.getRadioArgs(); 
    exit(-1);
  }

  // now set the stuff that we care about
  dev->setMasterClockRate(params.getClockRate()); 
  dev->setSampleRate(SOAPY_SDR_TX, 0, params.getSampleRate());
  dev->setSampleRate(SOAPY_SDR_RX, 0, params.getSampleRate());

  dev->setAntenna(SOAPY_SDR_RX, 0, "LB2");
  dev->setAntenna(SOAPY_SDR_TX, 0, "BAND2");

  // set the gains for all stages.  
  dev->setGain(SOAPY_SDR_RX, 0, "LNA", params.getGain(Params::RX_LNA));
  dev->setGain(SOAPY_SDR_RX, 0, "PGA", params.getGain(Params::RX_PGA));
  dev->setGain(SOAPY_SDR_RX, 0, "TIA", params.getGain(Params::RX_TIA)); 

  dev->setGain(SOAPY_SDR_TX, 0, "PAD", params.getGain(Params::TX_PAD));   

  std::vector<size_t> chans; 
  chans.push_back(0); 

  tx_stream = dev->setupStream(SOAPY_SDR_TX, "CF32", chans);
  rx_stream = dev->setupStream(SOAPY_SDR_RX, "CF32", chans);   

  stat = dev->activateStream(rx_stream, 0, 0, 0);
  if(stat < 0) {
    std::cerr << boost::format("Error activating rx stream : [%s]\n") 
      % SoapySDR::errToStr(stat);
  }
  stat = dev->activateStream(tx_stream, 0, 0, 0);   
  if(stat < 0) {
    std::cerr << boost::format("Error activating tx stream : [%s]\n") 
      % SoapySDR::errToStr(stat);
  }

  // turn off automatic DC offset correction
  dev->setDCOffsetMode(SOAPY_SDR_RX, 0, false);
  dev->setDCOffsetMode(SOAPY_SDR_TX, 0, false);
   
  tx_offset = params.getTXOffset(); 

  // setup the tx envelopes
  env_on.resize(100);
  env_off.resize(100);
  for(int i = 0; i < 100; i++) {
    env_on[i] = std::complex<float>(1.0, 0.0);
    env_off[i] = std::complex<float>(0.0, 0.0);     
  }
		
  // allocate an RX buffer that ensures 100 cycles of the reference signal. 
  double tsamp = 100.0 / tx_offset; 
  double dvlen = tsamp * params.getSampleRate(); 
  rx_buffer.resize((int) round(dvlen)); 
}

CalObject::~CalObject() {
  dev->deactivateStream(rx_stream); 
  dev->deactivateStream(tx_stream); 

  dev->closeStream(rx_stream); 
  dev->closeStream(tx_stream); 

  SoapySDR::Device::unmake(dev); 
}

bool CalObject::doExp(double freq) {

  std::cout << boost::format("Performing experiment at %g\n") % freq; 
  dev->setFrequency(SOAPY_SDR_RX, 0, "RF", freq);
  dev->setFrequency(SOAPY_SDR_TX, 0, "RF", freq + tx_offset);   
  dev->setFrequency(SOAPY_SDR_RX, 0, "BB", 0.0);
  dev->setFrequency(SOAPY_SDR_TX, 0, "BB", 0.0);
  
  // reset cal settings
  dev->setDCOffset(SOAPY_SDR_TX, 0, std::complex<float>(0.0, 0.0));
  dev->setDCOffset(SOAPY_SDR_RX, 0, std::complex<float>(0.0, 0.0)); 
  dev->setIQBalance(SOAPY_SDR_TX, 0, std::complex<float>(1.0, 0.0));
  dev->setIQBalance(SOAPY_SDR_RX, 0, std::complex<float>(1.0, 0.0)); 

  std::cout << boost::format("Sending envelope ON\n");
  sendEnvelope(env_on); 

  sleep(5); 

  std::cout << boost::format("Sending envelope OFF\n");  
  sendEnvelope(env_off); 

  sleep(5); 

  return true; 
  if(!doRXExp(freq)) {
    return false; 
  }
  
  return doTXExp(freq); 
}

bool CalObject::sendEnvelope(std::vector<std::complex<float > > & vec)
{
  std::complex<float> * buf[1]; 
  buf[0] = vec.data(); 

  // in general, we're letting the TX stream run dry
  int flags = 0; 
  int stat = dev->writeStream(tx_stream, (void**) buf, vec.size(), flags); 
  if(stat < 0) {
    std::cerr << boost::format("TX Error on stream : [%s]\n") 
      % SoapySDR::errToStr(stat);
    return false; 
  }
  return true; 
}

bool CalObject::doTXExp(double freq) {
  (void) freq; 
  return true; 
}

bool CalObject::doRXExp(double freq) {
  (void) freq; 
  return true; 
}

bool CalObject::dumpBuffer(std::string & ofname, std::vector<std::complex<float> > & vec) {
  std::ofstream ofs(ofname.c_str()); 
  for(unsigned int i = 0; i < vec.size(); i++) {
    ofs << boost::format("%d %g %g\n") % i % vec[i].real() % vec[i].imag(); 
  }
  ofs.close(); 
  return true; 
}

bool CalObject::getRXBuffer(std::vector<std::complex<float> > & vec) {
  int stat; 
  int flags = 0; 
  long long timeNs; 
  std::complex<float> * bp[1]; bp[0] = vec.data();

  long long cur_time = dev->getHardwareTime(); 
  
  while(timeNs < cur_time) {
    stat = dev->readStream(rx_stream, (void **) bp, vec.size(), flags, timeNs, 100000); 
    if(stat < 0) {
      std::cerr << boost::format("RX Error on stream : [%s]\n") 
	% SoapySDR::errToStr(stat);
      return false; 
    }
  }

  return true; 
}

std::complex<float> CalObject::vectorMean(std::vector<std::complex<float>> & vec)
{
  std::complex<float> ret = std::complex<float>(0.0); 
  for(unsigned int i = 0; i < vec.size(); i++) {
    ret = ret + vec[i]; 
  }

  float vs = (float) vec.size(); 
  ret = ret / std::complex<float>(vs, 0.0); 

  return ret; 
}

std::complex<float> CalObject::productMean(std::vector<std::complex<float> > & vec0, 
					   std::vector<std::complex<float> > & vec1)
{
  std::complex<float> ret = std::complex<float>(0.0); 
  for(unsigned int i = 0; i < vec0.size(); i++) {
    std::complex<float> prod(vec0[i].real() * vec1[i].real(), 
			     vec0[i].imag() * vec1[i].imag()); 
    ret = ret + prod; 
  }

  float vs = (float) vec0.size(); 
  ret = ret / std::complex<float>(vs, 0.0); 

  return ret; 
}
