#include <pthread.h>
#include <iomanip>
#include <chrono>
#include <fstream>
#include "prodcon.h"
#include "log.h"

sem_t mutex;
sem_t empty;
sem_t full;
sem_t log_sem;
sem_t output;
queue<string> transactions;
bool EOF_hit;
bool no_tasks;
FILE * log_file;
Log summary;

chrono::time_point<std::chrono::steady_clock> startTime = chrono::steady_clock::now(); //Start Time Now

Log::Log() {
    Log::ask = 0;
    Log::receive = 0;
    Log::work = 0;
    Log::sleep = 0;
    Log::complete = 0;
    Log::nthreads = 0;
}

//Incrementors for high-level events

void Log::setAsk() {
    sem_wait(&log_sem);
    Log::ask += 1;
    sem_post(&log_sem);
}

void Log::setReceive() {
    sem_wait(&log_sem);
    Log::receive += 1;
    sem_post(&log_sem);
}

void Log::setWork() {
    sem_wait(&log_sem);
    Log::work += 1;
    sem_post(&log_sem);
}

void Log::setSleep() {
    sem_wait(&log_sem);
    Log::sleep += 1;
    sem_post(&log_sem);
}

void Log::setComplete() {
    sem_wait(&log_sem);
    Log::complete += 1;
    sem_post(&log_sem);
}

void Log::setCount(int id) {
    sem_wait(&log_sem);
    Log::threadArr[id] = Log::threadArr[id] + 1;
    sem_post(&log_sem);
}

void Log::setTime(float endTime) {
    Log::endTime = endTime;
}

void Log::setThreads(int nthreads) {
    Log::nthreads = nthreads;
    Log::threadArr = new int [nthreads + 1];

    for(int i = 0; i < nthreads + 2; i++) {
        Log::threadArr[i] = 0;
    }
}

//Helper function to Initialize all my semaphores
void initSemaphores(int queueSize) {
    sem_init(&output,0,1);
    sem_init(&mutex,0,1);
    sem_init(&empty,0,queueSize);
    sem_init(&full,0,0);
    sem_init(&log_sem,0,1);
}

//Time function - https://stackoverflow.com/a/27739925
double getTime() {
    chrono::steady_clock::time_point result = chrono::steady_clock::now();
    double ms = (double) (chrono::duration_cast<chrono::milliseconds>(result - startTime).count())/1000;
    return ms;
}

void Log::printSummary() {
    fprintf(log_file,"Summary:\n");
    fprintf(log_file,"    Work%11d\n",Log::work);
    fprintf(log_file,"    Ask%12d\n",Log::ask);
    fprintf(log_file,"    Receive%8d\n",Log::receive);
    fprintf(log_file,"    Complete%7d\n",Log::complete);
    fprintf(log_file,"    Sleep%10d\n",Log::sleep);

    for (int i = 0; i < Log::nthreads; i++) {
        fprintf(log_file,"    Thread %2d%6d\n",i + 1,Log::threadArr[i+1]);
    }

    //Prevent divide by zero error
    float tps;
    if (Log::endTime == 0) {
        tps = -1;    
    }

    else {
        tps = Log::complete / Log::endTime;
    }
    fprintf(log_file,"Transactions per second: %.2f\n",tps);
    fclose(log_file);
}

int main(int argc, char *argv[]) {

    int *threadCount = (int *)malloc(sizeof(int));
    int nthreads = atoi(argv[1]);
    *threadCount = nthreads;
    int queueSize = nthreads * 2;
    int id = 0;
    EOF_hit = false;
    no_tasks = false;  
    string filename;
    pthread_t thread;
    summary = Log();

    if (argc == 3) {
        id = atoi(argv[2]);
    }

    if (id == 0) {
        filename = "prodcon.log";
    }

    else {
        filename = (string)"prodcon." + argv[2] + (string)".log";
    }

    log_file = fopen(filename.c_str(),"w");
    initSemaphores(queueSize);
    summary.setThreads(nthreads);
    pthread_create(&thread,NULL,&Producer::initConsumers,(void *)threadCount);
    pthread_join(thread,NULL);
    free(threadCount);
    summary.printSummary();
    return 0;
}