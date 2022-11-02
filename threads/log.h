#ifndef LOG_H
#define LOG_H

#include "sharedvars.h"

class Log {

    int ask;
    int receive;
    int work;
    int sleep;
    int complete;
    int nthreads;
    float endTime;
    int *threadArr;

public:

    Log();
    void setAsk();
    void setReceive();
    void setWork();
    void setSleep();
    void setComplete();
    void setCount(int id);
    void setTime(float endTime);
    void setThreads(int nthreads);
    void printSummary();
};
#endif