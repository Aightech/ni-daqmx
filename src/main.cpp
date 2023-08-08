#include "ni-daqmx.hpp"
#include <lsl_cpp.h>

void cb(int nb_channels, int samplingSize, double* data, uint64_t timestamp_sec, uint32_t timestamp_nsec, void* obj)
{
	lsl::stream_outlet* outlet = reinterpret_cast<lsl::stream_outlet*>(obj);
	outlet->push_sample(data, timestamp_sec+ timestamp_nsec*1e-9);
	//print only once every 1000 samples
	static int j=0;
	if(j++%1000==0)
	{
		printf("\xd \bFx=%.3f Fy=%.3f Fz=%.3f Tx=%.3f Ty=%.3f Tz=%.3f", data[0], data[1], data[2], data[3], data[4], data[5]);
		fflush(stdout);
	}
		
}

int main(void)
{
    NI::ATI::FT6_sensor sensor(2);
	sensor.start_thread();

    int nb_channels = 6;
    lsl::stream_info info_sample("FT_sensor", "sample", nb_channels, 0,
                                 lsl::cf_double64);
    lsl::stream_outlet outlet_sample(info_sample);
    std::vector<double> sample(nb_channels);
    std::cout << "[INFOS] Now sending data... " << std::endl;
	sensor.set_callback(cb, &outlet_sample);

	std::cout << "\n## Press enter to stop streaming ##\n" << std::endl;
    char c = getchar();
	sensor.pause_thread();

    // int dj=10;
    // double v=0;
    // for(int j =0;;j++)
    // {
	// 	if(sensor.has_new_data())
	// 	{
	// 		for(unsigned i = 0; i < nb_channels; i++)
	// 		{
	// 			sample[i] = sensor.get_FT(i);
	// 			if(j%dj==0)
	// 			  printf("%.3f\t", sample[i]);
	// 		}
	// 		double a=0.99;
	// 		v=sample[2];
	// 		if(j%dj==0)
	// 		//printf("%.3f\n", v);
	// 		  printf("\n");
	// 		outlet_sample.push_sample(sample);
	// 	}
    // }
}
