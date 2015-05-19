all: 
	if [ ! -d "./bin" ];\
	then mkdir bin;\
	fi
	g++ -Wall -Werror -ansi -pedantic src/main.cpp -o bin/rshell
	g++ -Wall -Werror -ansi -pedantic src/ls.cpp -o bin/ls
	g++ -Wall -Werror -ansi -pedantic src/cp.cpp -o bin/cp
	g++ -Wall -Werror -ansi -pedantic src/rm.cpp -o bin/rm
	g++ -Wall -Werror -ansi -pedantic src/mv.cpp -o bin/mv

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

mv: 
	if [ ! -d "./bin" ];\
	then mkdir bin;\
	fi
	g++ -Wall -Werror -ansi -pedantic src/mv.cpp -o bin/mv

rm: 
	if [ ! -d "./bin" ];\
	then mkdir bin;\
	fi
	g++ -Wall -Werror -ansi -pedantic src/rm.cpp -o bin/rm

clean:
	rm -rf bin
