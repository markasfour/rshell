FLAGS = -Wall -Werror -ansi -pedantic -o

all:
	mkdir bin; \
	g++ $(FLAGS) bin/rshell src/main.cpp; \
	g++ $(FLAGS) bin/cp src/cp.cpp; \

rshell: src/main.cpp
	g++ $(FLAGS) bin/rshell src/main.cpp; \

cp: src/cp.cpp src/Timer.h
	g++ $(FLAGS) bin/cp src/cp.cpp; \
	
clean:
	rm -rf bin
