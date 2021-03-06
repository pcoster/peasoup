#pragma once
#include <vector>
#include "cuda.h"
#include <thrust/copy.h>
#include <thrust/device_ptr.h>
#include "utils/exceptions.hpp"

//TEMP
#include <stdio.h>

//######################

template <class T> class TimeSeries {
protected:
  T* data_ptr;
  unsigned int nsamps;
  float tsamp;
  
public:  
  TimeSeries(T* data_ptr,unsigned int nsamps,float tsamp)
    :data_ptr(data_ptr), nsamps(nsamps), tsamp(tsamp){}

  TimeSeries(void)
    :data_ptr(0), nsamps(0.0), tsamp(0.0) {}

  TimeSeries(unsigned int nsamps)
    :data_ptr(0), nsamps(nsamps), tsamp(0.0){}
  
  T operator[](int idx){
    return data_ptr[idx];
  }
  
  T* get_data(void){return data_ptr;}
  void set_data(T* data_ptr){this->data_ptr = data_ptr;};
  unsigned int get_nsamps(void){return nsamps;}
  void set_nsamps(unsigned int nsamps){this->nsamps = nsamps;}
  float get_tsamp(void){return tsamp;}
  void set_tsamp(float tsamp){this->tsamp = tsamp;}
};

//#########################

template <class T>
class DedispersedTimeSeries: public TimeSeries<T> {
private:
  float dm;

public:
  DedispersedTimeSeries()
    :TimeSeries<T>(),dm(0.0){}

  DedispersedTimeSeries(T* data_ptr, unsigned int nsamps, float tsamp, float dm)
    :TimeSeries<T>(data_ptr,nsamps,tsamp),dm(dm){}
  
  float get_dm(void){return dm;}
  void set_dm(float dm){this->dm = dm;}
};

//###########################

template <class T>
class FilterbankChannel: public TimeSeries<T> {
private:
  float freq;
  
public:
  FilterbankChannel(T* data_ptr, unsigned int nsamps, float tsamp, float freq)
    :TimeSeries<T>(data_ptr,nsamps,tsamp),freq(freq){}
};


template <class OnDeviceType>
class DeviceTimeSeries: public TimeSeries<OnDeviceType> {
public:
  DeviceTimeSeries(unsigned int nsamps,bool reusable=false)
    :TimeSeries<OnDeviceType>(nsamps)
  {
    cudaError_t error = cudaMalloc((void**)&this->data_ptr, sizeof(OnDeviceType)*nsamps);
    ErrorChecker::check_cuda_error(error);
    
  }

  template <class OnHostType>
  DeviceTimeSeries(TimeSeries<OnHostType>& host_tim)
    :TimeSeries<OnDeviceType>(host_tim.get_nsamps())
  {
    cudaError_t error;
    OnHostType* copy_buffer;
    error = cudaMalloc((void**)&copy_buffer, sizeof(OnHostType)*this->nsamps);
    ErrorChecker::check_cuda_error(error);
    error = cudaMemcpy(copy_buffer, host_tim.get_data(), this->nsamps, cudaMemcpyHostToDevice);
    ErrorChecker::check_cuda_error(error);
    thrust::device_ptr<OnHostType> thrust_copy_ptr(copy_buffer);
    thrust::device_ptr<OnDeviceType> thrust_data_ptr(this->data_ptr);
    thrust::copy(thrust_copy_ptr, thrust_copy_ptr+this->nsamps, thrust_data_ptr);
    this->tsamp = host_tim.get_tsamp();
    cudaFree(copy_buffer);
  }
 
  ~DeviceTimeSeries()
  {
    cudaFree(this->data_ptr);
  }

};


template <class OnDeviceType,class OnHostType>
class ReusableDeviceTimeSeries: public DeviceTimeSeries<OnDeviceType> {
private:
  OnHostType* copy_buffer;
  
public:
  ReusableDeviceTimeSeries(unsigned int nsamps)
    :DeviceTimeSeries<OnDeviceType>(nsamps)
  {
    cudaError_t error = cudaMalloc((void**)&copy_buffer, sizeof(OnHostType)*this->nsamps);
    ErrorChecker::check_cuda_error(error);
  }
  
  void copy_from_host(TimeSeries<OnHostType>& host_tim)
  {
    this->tsamp = host_tim.get_tsamp();
    cudaError_t error = cudaMemcpy(copy_buffer, host_tim.get_data(), this->nsamps, cudaMemcpyHostToDevice);
    ErrorChecker::check_cuda_error(error);
    thrust::device_ptr<OnHostType> thrust_copy_ptr(copy_buffer);
    thrust::device_ptr<OnDeviceType> thrust_data_ptr(this->data_ptr);
    thrust::copy(thrust_copy_ptr, thrust_copy_ptr+this->nsamps, thrust_data_ptr);
  }

  ~ReusableDeviceTimeSeries()
  {
    cudaFree(copy_buffer);
    cudaFree(this->data_ptr);
  }
};

  
//#############################

template <class T>
class TimeSeriesContainer {
protected:
  T* data_ptr;
  unsigned int nsamps;
  float tsamp;
  unsigned int count;
  
  TimeSeriesContainer(T* data_ptr, unsigned int nsamps, float tsamp, unsigned int count)
    :data_ptr(data_ptr),nsamps(nsamps),tsamp(tsamp),count(count){}
  
public:
  unsigned int get_count(void){return count;}
  unsigned int get_nsamps(void){return nsamps;}
  void set_tsamp(float tsamp){this->tsamp = tsamp;}
};

//created through Dedisperser
template <class T>
class DispersionTrials: public TimeSeriesContainer<T> {
private:
  std::vector<float> dm_list;
  
public:
  DispersionTrials(T* data_ptr, unsigned int nsamps, float tsamp, std::vector<float> dm_list_in)
    :TimeSeriesContainer<T>(data_ptr,nsamps,tsamp, (unsigned int)dm_list_in.size())
  {
    dm_list.swap(dm_list_in);
  }
  
  DedispersedTimeSeries<T> operator[](int idx)
  {
    T* ptr = this->data_ptr+idx*this->nsamps;
    return DedispersedTimeSeries<T>(ptr, this->nsamps, this->tsamp, dm_list[idx]);
  }
  //DedispersedTimeSeries<T> nearest_dm(float dm){}
};



//created through Channeliser
template <class T>
class FilterbankChannels: public TimeSeriesContainer<T> {

public:
  FilterbankChannel<T> operator[](int idx);
  FilterbankChannel<T> nearest_chan(float freq);
  
};
