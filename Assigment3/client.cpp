#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "tands.h"
#include <iostream>
#include <chrono>
#include <vector>

using namespace std;

double epochTime() {
	double now = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
	return now / 1000;
}

int main(int argc, char * argv[]) {

	string input;
	int numTransactions = 0;
	int count = 0;
	int n = 0;
	int cfd;
	int port = atol(argv[1]);
	if (port > 64000 || port < 5000) {
		cout<<"Please enter a valid port number!"<<endl;
		return 0;
	}

	char * address = argv[2];
	struct sockaddr_in server;

	char host[1024];
	string hostName;
	gethostname(host,1024);
	hostName = (string)host + '.' + to_string(getpid());

	FILE * log = fopen((hostName + ".log").c_str(),"w");

	fprintf(log,"Using port %d\n",port);
	fprintf(log,"Using server address %s\n",address);
	fprintf(log,"Host %s\n",hostName.c_str());

	server.sin_addr.s_addr = inet_addr(address);
	server.sin_family = AF_INET;
	server.sin_port = htons(port);

	cfd = socket(AF_INET,SOCK_STREAM,0);
	if (cfd < 0) {
		perror("Client Socket could not be made");
		fclose(log);
		exit(-1);
	}


	if (connect(cfd, (struct sockaddr *) &server,sizeof(server)) < 0) {
		perror("Client could not connect");
		fclose(log);
		exit(-1);
	}


	while(getline(cin,input)) {
		n = atoi(input.substr(1,input.length() - 1).c_str());
		if (input[0] == 'S') {
			fprintf(log,"Sleep %d units\n",n);
			Sleep(n);
		}

		else if (toupper(input[0]) == 'T') {

			string a = input + '-' + hostName;
			if (send(cfd,a.c_str(),a.length(),0) < 0) {
				perror("Send failed");
				fclose(log);
				exit(-1);
			}

			numTransactions++;

			fprintf(log,"%.2f: Send (T%3d)\n",epochTime(),n);


			int rsize;
			vector<char> buffer(2000);
			if ((rsize = recv(cfd,buffer.data(), buffer.size(),0)) <= 0){
				perror("Receive failed");
				fclose(log);
				exit(-1);
			}

			string s(buffer.begin(), buffer.end());
			count = atoi(s.substr(1, s.length() -1).c_str());
			fprintf(log,"%.2f: Recv (D%3d)\n",epochTime(),count);

			
		}

	}

	if (close(cfd) < 0){
		perror("Client could not be closed");
		fclose(log);
		exit(-1);
	}
	fprintf(log,"Sent %d transactions\n",numTransactions);
	fclose(log);

	return 0;
}
