#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>	
#include<pthread.h>
#include<semaphore.h>
#include<vector>
#include<iostream>
#include "tands.h"
#include<poll.h>
#include<unordered_map>
#include<chrono>

using namespace std;

unordered_map<string, int> summary;
int count = 1;
sem_t mutex;
double startTime;
double endTime;

double epochTime() {
	double now = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
	return now / 1000;
}

void *tconnect(void *fd) {
	int sock = *(int*)fd;
	int rsize; 
	vector<char> buffer(2000);

	while ((rsize = recv(sock,buffer.data(), buffer.size(),0)) > 0) {

		string s(buffer.data());
		string e = s.substr(0,s.find("-"));
		string hostName = s.substr(s.find("-") + 1, s.length() - 1);
		int n = atoi(e.substr(1,e.length() - 1).c_str());
		
		if (summary.size() == 0) {
			startTime = epochTime();
		}

		if (summary.find(hostName) == summary.end()) {
			summary[hostName] = 0;
		}
		sem_wait(&mutex);
		endTime = epochTime();
		printf("%.2f: #%3d (T%3d) from %s\n",endTime,count,n,hostName.c_str());
		Trans(n);
		endTime = epochTime();
		printf("%.2f: #%3d (Done) from %s\n",endTime,count,hostName.c_str());
		string msg = "D" + to_string(count);

		if ((write(sock,msg.c_str(),msg.length()) < 0)) {
			perror("write() failed");
			exit(-1);
		}

		count++;
		summary[hostName]++;
		sem_post(&mutex);	
		buffer.clear();
		buffer.resize(2000);
	}
	free(fd);
	if (rsize < 0) {
		perror("Receive failed");
		exit(-1);
	}

	else if (rsize == 0) {
		fflush(stdout);	
	}

	return 0;
}

int main(int argc, char *argv[]) {

	sem_init(&mutex,0,1);
	int fd, cfd, *sock, p;
	struct sockaddr_in server, client;

	int port = atoi(argv[1]);
	if (port > 64000 || port < 5000) {
		cout<<"Please enter a valid port number!"<<endl;
		return 0;
	}

	struct pollfd pfd[1];
	int timer;

	fd = socket(AF_INET,SOCK_STREAM,0);
	if (fd < 0) {
		perror("Could not create a socket");
		exit(-1);
	}

	printf("Using port %d\n",port);

	memset(pfd,0,sizeof(pfd));
	pfd[0].fd = fd;
	pfd[0].events = POLLIN;
	timer = 30000;

	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = INADDR_ANY;

	if (bind(fd,(struct sockaddr *) &server, sizeof(server)) < 0) {
		perror("Server could not bind to socket");
		exit(-1);
	}

	if (listen(fd, 20) < 0) {
		perror("listen() failed");
		exit(-1);
	}
	
	while (true) {
		p = poll(pfd,1,timer);

		if (p < 0) {
			perror("Could not poll");
			exit(-1);
		}

		if (p == 0) {
			break;
		}
		int csize = sizeof(struct sockaddr_in);
		cfd = accept(fd, (struct sockaddr *) &client, (socklen_t*)&csize);
		if (cfd < 0) {
			perror("Client could not be accepted");
			exit(-1);
		}

		pthread_t thread;
		sock = new int;
		*sock = cfd;
		if (pthread_create(&thread, NULL, tconnect, (void *) sock) < 0) {
			perror("Thread could not be created");
			exit(-1);
		}
	}

	if(close(fd) < 0){
        perror("Server Failed to close");
        exit(-1);
    }

	count = 0;
	printf("\nSUMMARY\n");
    for (auto pairs : summary){
        printf("%4d transactions from %s\n",pairs.second,pairs.first.c_str());
		count += pairs.second;
    }

	double difference = endTime - startTime;
    printf("%4.1f transactions/sec  (%d,%.2f)\n",(double)count/difference,count,difference);
    return 0;

}

