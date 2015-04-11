#include <iostream>
#include <string>
#include <unistd.h> 
#include <perror>
#include <vector>
#include <boost/tokenizer.hpp>
using namespace std;
using namespace boost;

int main(int argc, char **argv)
{
	//string command;
	//cout << "$ ";
	//cin >> command;
	
	execvp(command, argv);
	return 0;
}
