#include <iostream>
#include <string>
#include <unistd.h> 
#include <stdio.h>
#include <vector>
#include <boost/tokenizer.hpp>
using namespace std;
using namespace boost;

int main(int argc, char **argv)
{
	for(int i = 0; i < argc; i++)
	{
		cout << "argv[" << i << "] = " << argv[i] << endl;
	}
	cout << "listed the arguements" << endl;
	for(int i = 1; i < argc; i++)
	{
		if(-1 == execvp(argv[i], argv))
       			perror("There was an error in execvp. ");
		execvp(argv[i], argv);
	}
	return 0;
}
