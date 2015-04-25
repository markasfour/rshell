#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

int main(int argc, char* argv[])
{

	//cout << "argc = " << argc << endl;	

	vector <string> arguments;
	//put all passed in arguments into a vector
	for(int i = 0; i < (argc - 1); i++)
	{
		arguments.push_back(argv[i + 1]);
	}
	//for(unsigned int i = 0; i < arguments.size(); i++)
	//{
	//	cout << arguments.at(i) << endl;
	//}


	//check for directory argument
	vector <string> directories;
	vector <string> flags;
	for(unsigned int i = 0; i < arguments.size(); i++)
	{
		if(arguments.at(i).at(0) == '-') //if argument starts with a -, then it is a flag
			flags.push_back(arguments.at(i));
		else
			directories.push_back(arguments.at(i));
	}
	//cout << "flags: " << endl;
	//for(unsigned int i = 0; i < flags.size(); i++)
	//{
	//	cout << flags.at(i) << endl;
	//}
	//cout << " directories: "<< endl;
	//for(unsigned int i = 0; i < directories.size(); i++)
	//{
	//	cout << directories.at(i);
	//}

	
	//flag checking
	bool aFlag = false;
	bool lFlag = false;
	bool RFlag = false;
	for(unsigned int i = 0; i < flags.size(); i++)
	{
		for(unsigned int j = 0; j < flags.at(i).size(); j++) //loop through each letter of the flag
		{
			//cout << i << " " << j << " ";
			//cout << flags.at(i).at(j) << endl;
			if(flags.at(i).at(j) == '-') //do nothing
				{}	
			else if(flags.at(i).at(j) == 'a')
				aFlag = true;
			else if(flags.at(i).at(j) == 'l')
				lFlag = true;
			else if(flags.at(i).at(j) == 'R')
				RFlag = true;
			else
			{
				perror("invalid flag");
				exit(1);
			}
		}
	}
	
	if(aFlag || lFlag || RFlag)
		cout << "yay" << endl;

	//address directories first
	if(directories.size() > 0)
	{
		//check if valid directory
		struct stat name;
		for(unsigned int i = 0; i < directories.size(); i++)
		{
			if(stat(directories.at(i).c_str(), &name) == -1)
			{
				perror("stat error");
				continue;	
			}
			else
				cout << "yay" << endl;
		}
	}
	else
	{
		cout << "yolo" << endl;
	}
	return 0;
}
