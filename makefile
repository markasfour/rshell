all: 
	if [ ! -d "./bin" ];\
	then mkdir bin;\
	fi
	g++ -Wall -Werror -ansi -pedantic src/main.cpp -o bin/rshell
	g++ -Wall -Werror -ansi -pedantic src/ls.cpp -o bin/ls
	g++ -Wall -Werror -ansi -pedantic src/cp.cpp -o bin/cp

rshell:
	if [ ! -d "./bin" ];\
	then mkdir bin;\
	fi
	g++ -Wall -Werror -ansi -pedantic src/main.cpp -o bin/rshell

ls:
	if [ ! -d "./bin" ];\
	then mkdir bin;\
	fi
	g++ -Wall -Werror -ansi -pedantic src/ls.cpp -o bin/ls   

cp: 
	if [ ! -d "./bin" ];\
	then mkdir bin;\
	fi
	g++ -Wall -Werror -ansi -pedantic src/cs.cpp -o bin/cp

clean:
	rm -rf bin/
