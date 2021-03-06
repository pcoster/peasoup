#pragma once
#include "cuda.h"
#include "cufft.h"
#include "data_types/timeseries.hpp"
#include "utils/exceptions.hpp"

class CuFFTer {
protected:
  cufftHandle fft_plan;
  unsigned int size; 
  CuFFTer(void):fft_plan(0),size(0){}
  unsigned int get_size(void){return size;}

public:
  double get_resolution(float tsamp){
    return (double) 1.0/(size * tsamp);
  }
};


class CuFFTerR2C: public CuFFTer {

public:
  CuFFTerR2C(unsigned int size, unsigned int batch=1)
    :CuFFTer()
  {
    this->size = size;
    cufftResult error = cufftPlan1d(&fft_plan, size, CUFFT_R2C, 1);
    ErrorChecker::check_cufft_error(error);
  }
  
  unsigned int get_output_size(void){
    return size/2+1;
  }
  
  void execute(float* tim, cufftComplex* fseries)
  {
    cufftResult error = cufftExecR2C(fft_plan, (cufftReal*) tim, fseries);
    ErrorChecker::check_cufft_error(error);
  }
  
  
};

