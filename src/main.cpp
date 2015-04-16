#include <iostream>
#include <string.h>
#include <unistd.h> 
#include <stdio.h>
#include <errno.h>
#include <sys/wait.h>
#include <vector>
#include <boost/tokenizer.hpp>
using namespace std;
using namespace boost;

int main(int argc, char **argv)
{
	bool finish = false;
	string login = getlogin();
	if(getlogin() == NULL)
	{
		login = "";
		perror("get login failed");
	}
	char hostarray[64];
	gethostname(hostarray, 64);
	if(gethostname(hostarray, 64) == -1)
		perror("get host name failed");
	
	//rshell loop
	while(!finish)
	{
		int semicolon= 0;
		int ampersand = 0;
		int pipe = 0;
		string command = "";
		vector <string> commands;
		vector <char *> args;
		vector <vector <char *> > execArgs;
		vector <char *> com;

		//login name and host info prompt
		if(getlogin() != NULL)
			cout << login << "@" << hostarray;
		
		//ready prompt
		cout << "$ ";

		//take in command from user
		getline(cin, command);
	//	cout << command << endl;

		//account for #
		if(command.find("#") != string::npos)
		{
			command = command.substr(0, command.find("#"));
			cout << command << endl;
		}

		//tokenizer init
		char_separator<char> delim(" ;&|#",";&|#", keep_empty_tokens);
		tokenizer< char_separator<char> > mytok(command, delim);	
		
		//token check
		for(tokenizer<char_separator<char> >::iterator it = mytok.begin(); it != mytok.end(); it++)
		{
			cout << "(" << *it << ")" << " ";
		}
		cout << "listed the arguements" << endl;

		//command list formatting
		for(tokenizer<char_separator<char> >::iterator it = mytok.begin(); it != mytok.end(); it++)
		{
			//cout << "; = " << semicolon << ", & = " << ampersand << ", | =" << pipe << endl;
			//cout << *it << endl;
			if(*it == "#")
			{
				break;
			}
			
			else if(*it == ""); //do nothing
			
			else if(*it == ";")
			{
				semicolon++;
				if(semicolon > 1)
				{
					cout << "syntax error 1";
					return -1;
				}
				else if(semicolon == 1)
				{
					if(ampersand == 0 && pipe == 0)
					{
						commands.push_back(";");
						semicolon = 0;
					}
					else
					{
						cout << "syntax error 5";
						return -1;
					}
				}
				
			}
			else if(*it == "&")
			{
				ampersand++;
				if(ampersand > 2)
				{
					cout << "syntax error 2";
					return -1;
				}
				else if(ampersand == 2)
				{
					if(semicolon == 0 && pipe == 0)
					{
						commands.push_back("&&");
						ampersand = 0;
					}
					else
					{
						cout << "syntax error 6";
						return -1;
					}
				}
				
			}
			else if(*it == "|")	
			{
				pipe++;
				if(pipe > 2)
				{
					cout << "syntax error 3";
					return -1;
				}
				else if(pipe == 2)
				{
					if(semicolon == 0 && ampersand == 0)
					{
						commands.push_back("||");
						pipe = 0;
					}
					else
					{
						cout << "syntax error 7";
						return -1;
					}
				}
				
			}		
			else
			{
				if(semicolon != 0 || ampersand != 0 || pipe != 0)
				{
					cout << "syntax error 4";
					cout << semicolon << " " << ampersand << " " << pipe << endl;
					return -1;
				}
				else
					commands.push_back(*it);
			}
		}
		
		//check commands
		for(int i = 0; i < commands.size(); i++)
		{
			cout << "(" << commands.at(i) << ") ";
		}
		cout << "combined arguements into groups" << endl;

		
		//add commands to command line arguements
		for(int i = 0; i < commands.size(); i++)
		{
			args.push_back(const_cast<char*>(commands.at(i).c_str()));
			execArgs.push_back(args);
			args.clear();
		}		
		for(int i = 0; i < execArgs.size(); i++)
		{
			com = execArgs.at(i);
		}


		//exec commands
		for(int i = 0; i < com.size(); i++)
		{
			//check for exit command. use strncmp because of char * type
			if(strncmp(com[i], "exit", 4) == 0)
			{
				finish = true;
				cout << "ending session...";
				break;
			}
			cout << com[i] << endl;
			//execute command. based off of in-lecture notes
			int pid = fork();
			if(pid == -1)
			{
				perror("There was an error with fork(). ");
				exit(1);
			}
			else if(pid == 0) //in child
			{
				if(-1 == execvp(com[i], &com[i]))
					perror("There was an error in execvp.");
				exit(1);
			}
			else if(pid > 0) //in parent
			{
				if(-1 == wait(0)) //wait for child to finish
					perror("There was an error with wait().");
			}

		}

		//shell termination
		if(finish)
		{
			cout << "good-bye!" << endl;
			break;
		}
	}
	return 0;
}
