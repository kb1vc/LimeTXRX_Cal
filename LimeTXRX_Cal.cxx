#include <SoapySDR/Device.hpp>
#include <SoapySDR/Types.hpp>
#include <SoapySDR/Logger.hpp>
#include <SoapySDR/Version.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <iostream>
#include <vector>

#include "Params.hxx"
#include "CalObject.hxx"




int main(int argc, char * argv[])
{
  Params params(argc, argv); 
  
  CalObject dev(params); 

  for(double freq = params.getStart(); 
      freq <= params.getStop(); 
      freq += params.getStep()) {
    if(!dev.doExp(freq)) {
      exit(-1); 
    }
  }
  exit(0); 
}
