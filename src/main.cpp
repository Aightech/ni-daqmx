#include <stdio.h>	// printf
#include <stdlib.h> // malloc, exit, free
#include <NIDAQmx.h>
#include <time.h>
#include <string>
#include <iostream>

#include <lsl_cpp.h>


// nidaqmxconfig --self-test
// nidaqmxconfig --self-test /dev/nipalk
// g++ main.cpp /usr/lib/x86_64-linux-gnu/libnidaqmx.so

class NIcard
{
public:
	NIcard(int n) : m_n(n)
	{
		std::string task_name = "Task";
		std::string ch_path = "Dev1/ai";
		std::string ch_name = "Channel";
		m_err = DAQmxCreateTask(task_name.c_str(), &m_taskHandle);
		check_error(m_err);
		for (int i = 0; i < m_n; i++)
		{
			m_err = DAQmxCreateAIVoltageChan(m_taskHandle, (ch_path + std::to_string(i)).c_str(), (ch_name + std::to_string(i)).c_str(), DAQmx_Val_Cfg_Default, -10.0, 10.0, DAQmx_Val_Volts, NULL);
			check_error(m_err);
		}

		// m_err = DAQmxCreateAIVoltageChan(m_taskHandle, "Dev1/ai1", "ChannelName2", DAQmx_Val_RSE, -10.0, 10.0, DAQmx_Val_Volts, NULL);

		m_err = DAQmxStartTask(m_taskHandle);
		check_error(m_err);
	};
	~NIcard(){};

	void read(float64 *data)
	{
		m_err = DAQmxReadAnalogF64(m_taskHandle, 1, 10.0, DAQmx_Val_GroupByChannel, data, m_n, &m_nb_read, NULL);
		check_error(m_err);
	};

	void check_error(int error)
	{
		if (DAQmxFailed(error))
		{
			DAQmxGetExtendedErrorInfo(m_errBuff, 2048);
			if (m_taskHandle != 0)
			{
				DAQmxStopTask(m_taskHandle);
				DAQmxClearTask(m_taskHandle);
			}
			if (DAQmxFailed(error))
				printf("DAQmx Error: %s\n", m_errBuff);
			printf("End of program, press Enter key to quit\n");
			getchar();
			exit(0);
		}
	}

	void convert(float64 *data_from, float64 *data_to)
	{//Tz,Ty,Tx,Fz,Fy,Fx
		for (int i = 0; i < 6; i++)
		{
			data_to[i] = 0;
			for (int j = 0; j < 6; j++)
			{
				data_to[i] += data_from[j] * m_calcoef[i][j];
			}
		}
	};

	int32 m_err = 0;
	TaskHandle m_taskHandle = 0;
	int32 m_nb_read;
	char m_errBuff[2048] = {'\0'};
	int m_n;
	float64 m_calcoef[6][6] = {
		{-0.00758, -0.00595, 0.17292,-3.31572,-0.10515, 3.04384},
		{-0.06741,  3.73876, 0.06286,-1.92243, 0.06722,-1.73453},
		{ 3.59265,  0.12395, 3.72948, 0.10916, 3.47531, 0.17706},
		{-0.37381, 22.57870, 21.6181,-10.8513,-18.7227,-11.5072},
		{-23.5661, -0.97300, 10.7964, 20.7230, 11.9170,-11.9916},
		{-0.51999, 14.04470,-0.63699, 15.2440,-0.38282, 13.2183}};
};

int main(void)
{
	int n = 6;
	float64 data[n];
	float64 conv[n];
	NIcard card(n);

   lsl::stream_info info_sample("pressure", "sample", n, 0,
                                     lsl::cf_double64);
        lsl::stream_outlet outlet_sample(info_sample);
        std::vector<int32_t> sample(n);
        std::cout << "[INFOS] Now sending data... " << std::endl;

	for (;;)
	{
		card.read(data);
		card.convert(data, conv);
		for (int i = 0; i < n; i++)
      {
        sample[i] = conv[i];
			printf("%.3f\t", conv[i]);
      }
		printf("\n");
      outlet_sample.push_sample(sample);
	}
}
