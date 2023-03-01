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
    int dj=10;
    double v=0;
    for(int j =0;;j++)
    {
		if(sensor.has_new_data())
		{
			for(unsigned i = 0; i < nb_channels; i++)
			{
				sample[i] = sensor.get_FT(i);
				// if(j%dj==0)
				//   printf("%.3f\t", sample[i]);
			}
			double a=0.99;
			v=a*v + (1-a)*sample[2];
			if(j%dj==0)
			  printf("%.3f\n", v);
			  //printf("\n");
			outlet_sample.push_sample(sample);
		}
    }
}
