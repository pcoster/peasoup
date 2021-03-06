#include <data_types/fourierseries.hpp>
#include <kernels/kernels.h>

class SpectrumFormer {
public:
  void form_interpolated(DeviceFourierSeries<cufftComplex>& input,
			 DevicePowerSpectrum<float>& output)
  {
    device_form_power_series(input.get_data(), output.get_data(),
			     input.get_nbins(), 1);
  }
  
  void form(DeviceFourierSeries<cufftComplex>& input,
	    DevicePowerSpectrum<float>& output)
  {
    device_form_power_series(input.get_data(), output.get_data(),
                             input.get_nbins(), 0);
  }

};
