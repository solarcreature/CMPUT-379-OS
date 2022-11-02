#include "prodcon.h"

extern Log summary;


//Critical section protected by semaphores for queue. Sleep is called when input is S, else work is added to queue. 
void Producer::pushQueue() {

    string input;
    while (getline(cin,input)) {
        int n = stoi(input.substr(1,input.length() - 1));
        if(input[0] == 'T') {
            
            sem_wait(&empty);
            sem_wait(&mutex);
            transactions.push(input);
            summary.setWork();

            sem_wait(&output);
            double ms = getTime();
            fprintf(log_file,"   %.3f ID= 0 Q=%2ld Work   \t%d\n",ms,transactions.size(),n);
            summary.setTime(ms);
            sem_post(&output);

            sem_post(&mutex);
            sem_post(&full);

            
        }
        else if(input[0] == 'S') {

            sem_wait(&output);
            double ms = getTime();
            fprintf(log_file,"   %.3f ID= 0      Sleep  \t%d\n",ms,n);
            summary.setTime(ms);
            sem_post(&output);

            Sleep(n);
            summary.setSleep();
        }
    }
    EOF_hit = true;

    sem_wait(&output);
    double ms = getTime();
    fprintf(log_file,"   %.3f ID= 0      End\n",ms);
    summary.setTime(ms);
    sem_post(&output);
}

//Starts n consumer threads and pushes work to the queue using the above function. Waits for all consumer threads to end. 
void * Producer::initConsumers(void *arg) {
    
    int nthreads = *static_cast<int *>(arg);
    int *id;
    pthread_t consumerThreads [nthreads];
    for (int i = 0; i < nthreads; i++) {
        id = static_cast<int *>(malloc(sizeof(int)));
        *id = i;
        pthread_create(&consumerThreads[i],NULL,&Consumer::runThreads, static_cast<void *>(id));
        free(id);
    }
    pushQueue();
    for (int i = 0; i < nthreads; i++) {
        pthread_join(consumerThreads[i],NULL);
    }  
    return 0;
}

//Consumer thread of id > 0 pops from critical section queue. Semaphores used to protect this queue. 
string Consumer::popQueue(int id) {

    sem_wait(&full);
    sem_wait(&mutex);

    string task = "";
    if (!no_tasks) {
        task = transactions.front();
        int n = stoi(task.substr(1,task.length() - 1));
        transactions.pop();

        sem_wait(&output);
        double ms = getTime();
        fprintf(log_file,"   %.3f ID=%2d Q=%2ld Receive \t%d\n",ms,id,transactions.size(),n);
        summary.setTime(ms);
        sem_post(&output);

        summary.setReceive();

        if(EOF_hit && transactions.size() == 0) {
            no_tasks = true;
            sem_post(&mutex);
            sem_post(&empty);
            sem_post(&full);
        }
    }
    else {
        sem_post(&mutex);
        sem_post(&empty);
        sem_post(&full);
    } 

    sem_post(&mutex);
    sem_post(&empty);
    return task;
}

//Consumer Thread Loop until EOF and queue is empty. 
void * Consumer::runThreads(void *args) {

    int id = *static_cast<int *>(args) + 1;
    while (true) {
        string task = "";

        sem_wait(&output);
        double ms = getTime();
        fprintf(log_file,"   %.3f ID=%2d      Ask  \t\n",ms,id);
        summary.setTime(ms);
        sem_post(&output);

        summary.setAsk();

        if (no_tasks) {
            break;
        }
        
        task = popQueue(id);

        if (task != "") {
            int n = stoi(task.substr(1,task.length() - 1));
            
            if (task[0] == 'T') {
                Trans(n);
            }

            sem_wait(&output);
            double ms = getTime();
            fprintf(log_file,"   %.3f ID=%2d      Complete  \t%d\n",ms,id,n);
            summary.setTime(ms);
            sem_post(&output);

            summary.setCount(id);
            summary.setComplete();
        }

        else {
            break;
        }    
    }

    return 0;
}
