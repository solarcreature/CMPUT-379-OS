CC = g++
FLAGS = -Wall -std=c++11
SRC = main.cpp

shell379:
	$(CC) $(FLAGS) $(SRC) -o shell379

clean:
	rm shell379
