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


int status;


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
		bool syntaxerror = false;
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
		//for(tokenizer<char_separator<char> >::iterator it = mytok.begin(); it != mytok.end(); it++)
		//{
		//	cout << "(" << *it << ")" << " ";
		//}
		//cout << "listed the arguements" << endl;
		

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
					syntaxerror = true;
					break;
				}
				else if(semicolon == 1)
				{
					if(ampersand == 0 && pipe == 0)
					{
						if(temp == "")
						{
							perror("No arguments before connector");
							syntaxerror = true;
							break;
						}
						commands.push_back(temp);
						temp = "";
						commands.push_back(";");
						semicolon = 0;
					}
					else
					{
						perror("Syntax error. Improper use of connectors.");
						syntaxerror = true;
						break;
					}
				}
				
			}
			else if(*it == "&") //ampersand handling
			{
				ampersand++;
				if(ampersand > 2)
				{
					perror("Syntax error. Too many & characters");
					syntaxerror = true;
					break;
				}
				else if(ampersand == 2)
				{
					if(semicolon == 0 && pipe == 0)
					{
						if(temp == "")
						{
							perror("No arguments before connector");
							syntaxerror = true;
						}
						commands.push_back(temp);
						temp = "";
						commands.push_back("&&");
						ampersand = 0;
					}
					else
					{
						perror("Syntax error. Improper use of connectors.");
						syntaxerror = true;
						break;
					}
				}
				
			}
			else if(*it == "|") //pipe handling
			{
				pipe++;
				if(pipe > 2)
				{
					perror("Syntax error. Too many | characters.");
					syntaxerror = true;
					break;
				}
				else if(pipe == 2)
				{
					if(semicolon == 0 && ampersand == 0)
					{
						if(temp == "")
						{
							perror("No arguments before connector");
							syntaxerror = true;
							break;
						}
						commands.push_back(temp);
						temp = "";
						commands.push_back("||");
						pipe = 0;
					}
					else
					{
						perror("Syntax error. Improper use of connectors.");
						syntaxerror = true;
						break;
					}
				}
				
			}		
			else
			{
				if(semicolon != 0 || ampersand != 0 || pipe != 0)
				{
					perror("Syntax error. Improper use of connectors.");
					//cout << semicolon << " " << ampersand << " " << pipe << endl;
					syntaxerror = true;
					break;
				}
				if(temp != "")
					temp += ' ';
				temp += *it;
			}
		}
		commands.push_back(temp.c_str());
		temp = "";


		//check commands
		//for(int i = 0; i < commands.size(); i++)
		//{
		//	cout << "(" << commands.at(i) << ") ";
		//}
		//cout << "combined arguements into groups" << endl;
		if(!syntaxerror)
		{
			char *input[999];
			//exec commands
			for(int i = 0; i < commands.size(); i++)
			{
				string current = "";
				string word = "";
				int k = 0;
				for(int j = 0; j < commands.at(i).size(); j++) //itterate through letters
				{
					current = commands.at(i);
					if(current[j] == ' ')
					{
						input[k] = new char[word.size() + 1];
						strcpy(input[k], word.c_str());
						k++;
						word = "";
					}
					else
						word += current[j]; //add letter		
				}	
				input[k] = new char[word.size() + 1];
				strcpy(input[k], word.c_str());
				k++;

				input[k] = new char[1]; //add the NULL char *
				input[k] = NULL;
		
				if(commands.at(i) ==  "exit") //exit command
				{
					finish = true;
					cout << "ending session...";
					break;
				}
				
				else if(commands.at(i) == ";") //semicolon case
				{
					continue;
				}

				else if(commands.at(i) == "||") //pipe case
				{
					if(status)
						continue;
					else
					{
						i++;
						continue;
					}
				}

				else if(commands.at(i) == "&&") //ampersand case
				{
					if(status == 0)
						continue;
					else
					{
						i++;
						continue;
					}
				}

				//execute command. based off of in-lecture notes
				int pid = fork();
				if(pid == -1)
				{
					perror("There was an error with fork(). ");
					exit(1);
				}
				else if(pid == 0) //in child
				{
					//cout << "CHILD IS RUNNING :D" << endl;
					//cout << input << endl;
					status = execvp(input[0], input);
					if(-1 == status) 
					{
						perror("There was an error in execvp.");
						
					}
					exit(1);
				}
				else if(pid > 0) //in parent
				{
					if(-1 == waitpid(pid, &status, 0)) //wait for child to finish
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
	}
	return 0;
}
