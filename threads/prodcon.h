#ifndef PRODCON_H
#define PRODCON_H

#include "log.h"
#include "tands.h"

extern FILE * log_file;

class Consumer {
    
    static string popQueue(int id);

public:

    static void *runThreads(void *args);
};

class Producer {

    static void pushQueue();

public:
    static void *initConsumers(void *arg);
};
#endif
