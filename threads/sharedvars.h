#ifndef SHAREDVARS_H
#define SHAREDVARS_H

#include <semaphore.h>
#include <queue>
#include <iostream>

using namespace std;

extern sem_t mutex;
extern sem_t empty;
extern sem_t full;
extern sem_t log_sem;
extern sem_t output;
extern queue<string> transactions;
extern bool EOF_hit;
extern bool no_tasks;  

double getTime();
#endif