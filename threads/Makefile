CC = g++
FLAGS = -Wall -pthread -O -std=c++14
OBJECTS = prodcon.o tands.o main.o  

prodcon: $(OBJECTS)
	$(CC) $(FLAGS) $(OBJECTS) -o prodcon

prodcon.o: prodcon.cpp prodcon.h sharedvars.h log.h tands.h
	$(CC) $(FLAGS) prodcon.cpp -c -o prodcon.o 

tands.o: tands.cpp tands.h
	$(CC) $(FLAGS) tands.cpp -c -o tands.o

main.o: main.cpp prodcon.h sharedvars.h log.h tands.h
	$(CC) $(FLAGS) main.cpp -c -o main.o

clean:
	rm *.o 
	rm prodcon