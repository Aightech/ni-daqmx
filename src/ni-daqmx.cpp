#include "ni-daqmx.hpp"
#include <unistd.h> // for usleep()

namespace NI
{

DAQcard::DAQcard(int n) : m_n(n)
{
    std::string task_name = "Task";
    std::string ch_path = "Dev1/ai";
    std::string ch_name = "Channel";
    m_err = DAQmxCreateTask(task_name.c_str(), &m_taskHandle);
    check_error(m_err);
    for(int i = 0; i < m_n; i++)
    {
        m_err = DAQmxCreateAIVoltageChan(
            m_taskHandle, (ch_path + std::to_string(i)).c_str(),
            (ch_name + std::to_string(i)).c_str(), DAQmx_Val_Cfg_Default, -10.0,
            10.0, DAQmx_Val_Volts, NULL);
        check_error(m_err);
    }
    m_err = DAQmxStartTask(m_taskHandle);
    check_error(m_err);
};
DAQcard::~DAQcard(){};

void DAQcard::read(float64 *data)
{
    m_err = DAQmxReadAnalogF64(m_taskHandle, 1, 10.0, DAQmx_Val_GroupByChannel,
                               data, m_n, &m_nb_read, NULL);
    check_error(m_err);
};

void DAQcard::check_error(int error)
{
    if(DAQmxFailed(error))
    {
        DAQmxGetExtendedErrorInfo(m_errBuff, 2048);
        if(m_taskHandle != 0)
        {
            DAQmxStopTask(m_taskHandle);
            DAQmxClearTask(m_taskHandle);
        }
        if(DAQmxFailed(error))
            printf("DAQmx Error: %s\n", m_errBuff);
        printf("End of program, press Enter key to quit\n");
        getchar();
        exit(0);
    }
}

namespace ATI
{

FT6_sensor::FT6_sensor() : m_card(6)
{
    m_active = true;
    m_thread = new std::thread(&FT6_sensor::loop, this);
};

FT6_sensor::~FT6_sensor()
{
    m_active = false;
    m_thread->join();
    delete m_thread;
};

double FT6_sensor::get_FT(unsigned i) //Tz,Ty,Tx,Fz,Fy,Fx
{
    std::lock_guard<std::mutex> lck(*m_mutex);
    i = (i < 6) ? i : 6;
    m_new_data[i] = false;
    return m_data_conv[i];
}

void *FT6_sensor::loop(void *obj)
{
    while(reinterpret_cast<FT6_sensor *>(obj)->m_active)
        reinterpret_cast<FT6_sensor *>(obj)->read_FT();
    return nullptr;
};

void FT6_sensor::read_FT()
{
    m_card.read(m_data);
    std::lock_guard<std::mutex> lck(*m_mutex);
    this->convert();
    
};

void FT6_sensor::convert()
{
    for(int i = 0; i < 6; i++)
    {
        m_data_conv[i] = 0;
        for(int j = 0; j < 6; j++)
            m_data_conv[i] += m_data[j] * m_calcoef[i][j];
        m_new_data[i] = true;
    }
};
} // namespace ATI
} // namespace NI
