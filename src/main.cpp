#include "ni-daqmx.hpp"
#include <lsl_cpp.h>

int main(void)
{
    NI::ATI::FT6_sensor sensor;
	sensor.start_thread();

    int nb_channels = 6;
    lsl::stream_info info_sample("FT_sensor", "sample", nb_channels, 0,
                                 lsl::cf_double64);
    lsl::stream_outlet outlet_sample(info_sample);
    std::vector<double> sample(nb_channels);
    std::cout << "[INFOS] Now sending data... " << std::endl;

    for(int i =0;;i++)
    {
		if(sensor.has_new_data())
		{
			for(unsigned i = 0; i < nb_channels; i++)
			{
				sample[i] = sensor.get_FT(i);
				if(i%1000==0)
				  printf("%.3f\t", sample[i]);
			}
			if(i%1000==0)
			  printf("\n");
			outlet_sample.push_sample(sample);
		}
    }
}
