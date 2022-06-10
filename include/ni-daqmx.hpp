#ifndef __NI_DAQMX_HPP
#define __NI_DAQMX_HPP

classs NI_DAQ
{
    public:
    NI_DAQ(){};

    private:
    int32_t m_error = 0;
    TaskHandle m_taskHandle = 0;
    int32_t m_read;
    double m_data[1000];
    char m_errBuff[2048] = {'\0'};
}

#endif
