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
	string command;
	cout << "$ "; //ready prompt
	cin >> command; //take in command from user

	char_separator<char> delim(" ||");
    	tokenizer< char_separator<char> > mytok(command, delim);	

	for(tokenizer<char_separator<char> >::iterator it = mytok.begin(); it != mytok.end(); ++it)
       		cout << *it << endl;
	
	cout << "listed the arguements" << endl;
//	for(int i = 1; i < argc; i++)
//	{
//		if(-1 == execvp(argv[i], argv))
//      			perror("There was an error in execvp. ");
//		execvp(argv[i], argv);
//	}
	return 0;
}
