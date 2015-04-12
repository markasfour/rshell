#include <iostream>
#include <string>
#include <unistd.h> 
#include <stdio.h>
#include <errno.h>
#include <vector>
#include <boost/tokenizer.hpp>
using namespace std;
using namespace boost;

int main(int argc, char **argv)
{
	bool finish = false;
	int empty = 0;
	string command = "";
	vector <string> commands;
	
	//ready prompt
	cout << "$ ";
	//take in command from user
	getline(cin, command);
//	cout << command << endl;

	//tokenizer init
	char_separator<char> delim(" |;&","", keep_empty_tokens);
    	tokenizer< char_separator<char> > mytok(command, delim);	
	
	//token check
	for(tokenizer<char_separator<char> >::iterator it = mytok.begin(); it != mytok.end(); it++)
	{
       		cout << "(" << *it << ")" << " ";
	}
	cout << "listed the arguements" << endl;

	//command list formatting
	string temp = "";
	for(tokenizer<char_separator<char> >::iterator it = mytok.begin(); it != mytok.end(); it++)
	{
		if(*it == "")
		{
			empty++;
			if(temp != "")
				commands.push_back(temp);
			temp = "";
			if(empty > 3)
				cout << "syntax error";
		}
		temp += *it;
		temp += " ";
	}
	if(temp != "")
		commands.push_back(temp);
	
	//add commands to command line arguements
//	int j = 0;
//	for(int i = 0; i< commands.size(); i++)
//	{
//		argv[j] = commands.at(i);
//		j++;
//	}

	//exec commands
	for(int i = 0; i < commands.size(); i++)
	{
		if(commands.at(i) == "exit ");
		{
			finish = true;
			cout << "ending session...";
			break;
		}
		command = commands.at(i);
		execvp(command, argv);
	}

	//shell termination
	if(finish)
	{
		cout << "good-bye!" << endl;
		//break;
	}
	
	return 0;
}
