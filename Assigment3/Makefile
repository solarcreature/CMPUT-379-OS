CC = g++
FLAGS = -Wall -O -std=c++14
OBJECTS_s = server.o tands.o 
OBJECTS_c = client.o tands.o 

all : server client client.pdf server.pdf
client: $(OBJECTS_c)
	$(CC) $(FLAGS) $(OBJECTS_c) -o client

server: $(OBJECTS_s)
	$(CC) $(FLAGS) $(OBJECTS_s) -o server -pthread

server.o: server.cpp 
	$(CC) $(FLAGS) -c server.cpp -o server.o

client.o: client.cpp 
	$(CC) $(FLAGS) -c client.cpp -o client.o

tands.o: tands.cpp tands.h
	$(CC) $(FLAGS) -c tands.cpp -o tands.o

client.pdf: client.man
	groff -man client.man | ps2pdf - client.pdf

server.pdf: server.man
	groff -man server.man | ps2pdf - server.pdf

clean:
	rm *.o 
	rm client
	rm server
	rm *.pdf
	rm *.log