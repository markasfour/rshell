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
		

		string temp;
		//command list formatting
		for(tokenizer<char_separator<char> >::iterator it = mytok.begin(); it != mytok.end(); it++)
		{
			//cout << "; = " << semicolon << ", & = " << ampersand << ", | =" << pipe << endl;
			//cout << *it << endl;
			
			if(*it == ""); //do nothing
			
			else if(*it == ";") //semicolon handling
			{
				semicolon++;
				if(semicolon > 1)
				{
					perror("Syntax error. Too many ; characters.");
					exit(1);
				}
				else if(semicolon == 1)
				{
					if(ampersand == 0 && pipe == 0)
					{
						if(temp == "")
						{
							perror("No arguments before connector");
							exit(1);
						}
						commands.push_back(temp);
						temp = "";
						commands.push_back(";");
						semicolon = 0;
					}
					else
					{
						perror("Syntax error. Improper use of connectors.");
						exit(1);
					}
				}
				
			}
			else if(*it == "&") //ampersand handling
			{
				ampersand++;
				if(ampersand > 2)
				{
					perror("Syntax error. Too many & characters");
					exit(1);
				}
				else if(ampersand == 2)
				{
					if(semicolon == 0 && pipe == 0)
					{
						if(temp == "")
						{
							perror("No arguments before connector");
							exit(1);
						}
						commands.push_back(temp);
						temp = "";
						commands.push_back("&&");
						ampersand = 0;
					}
					else
					{
						perror("Syntax error. Improper use of connectors.");
						exit(1);
					}
				}
				
			}
			else if(*it == "|") //pipe handling
			{
				pipe++;
				if(pipe > 2)
				{
					perror("Syntax error. Too many | characters.");
					exit(1);
				}
				else if(pipe == 2)
				{
					if(semicolon == 0 && ampersand == 0)
					{
						if(temp == "")
						{
							perror("No arguments before connector");
							exit(1);
						}
						commands.push_back(temp);
						temp = "";
						commands.push_back("||");
						pipe = 0;
					}
					else
					{
						perror("Syntax error. Improper use of connectors.");
						exit(1);
					}
				}
				
			}		
			else
			{
				if(semicolon != 0 || ampersand != 0 || pipe != 0)
				{
					perror("Syntax error. Improper use of connectors.");
					//cout << semicolon << " " << ampersand << " " << pipe << endl;
					return -1;
				}
				if(temp != "")
					temp += ' ';
				temp += *it;
			}
		}
		commands.push_back(temp.c_str());
		temp = "";

		
		//check commands
		for(int i = 0; i < commands.size(); i++)
		{
			cout << "(" << commands.at(i) << ") ";
		}
		cout << "combined arguements into groups" << endl;

		char *input[999];	
		//exec commands
		for(int i = 0; i < commands.size(); i++)
		{
			const char *arg = commands.at(i).c_str();
		
			input[i] = new char[commands.at(i).size() + 1];
			strcpy(input[i], commands.at(i).c_str() + '\0');
			cout << commands.at(i).size() << endl;
			for(int j = 0; j < commands.at(i).size() + 1; j++)
			{
				cout << j << " " << input[i][j] << endl;
				if(input[i][j] == 0)
					cout << "NULL!" << endl;
			}
	
			if(commands.at(i) ==  "exit")
			{
				finish = true;
				cout << "ending session...";
				break;
			}
			cout << "commands = " << commands.at(i) << " arg = " << arg << endl;
			//execute command. based off of in-lecture notes
			int pid = fork();
			if(pid == -1)
			{
				perror("There was an error with fork(). ");
				exit(1);
			}
			else if(pid == 0) //in child
			{
				cout << "CHILD IS RUNNING :D" << endl;
				cout << input << endl;
				if(-1 == execvp(input[0], input))
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
