#include <iostream>
#include <string.h>
#include <unistd.h> 
#include <stdio.h>
#include <errno.h>
#include <sys/wait.h>
#include <vector>
#include <boost/tokenizer.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
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
		int right = 0;
		int left = 0;
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
		//	cout << command << endl;
		}

		//account for empty command
		if(command == "")
			continue;

		//tokenizer init
		char_separator<char> delim(" ",";&|#<>");
		tokenizer< char_separator<char> > mytok(command, delim);	
		
		//token check
		//for(tokenizer<char_separator<char> >::iterator it = mytok.begin(); it != mytok.end(); it++)
		//{
		//	cout << "(" << *it << ")" << " ";
		//}
		//cout << "listed the arguements" << endl;
		

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
			//if(*it == ""); //do nothing
			
			if(*it == ";") //semicolon handling
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
				//cout << "PIPE!!!!!!!!!!!" << endl;
				pipe++;
				if(pipe > 2)
				{
					perror("Syntax error. Too many | characters.");
					syntaxerror = true;
					break;
				}
				else if(pipe == 1)
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
						commands.push_back("|");
					}
					else
					{
						perror("Syntax error. Improper use of connectors.");
						syntaxerror = true;
						break;
					}
				
				}
				else if(pipe == 2)
				{
					if(semicolon == 0 && ampersand == 0)
					{
						commands.push_back("|");
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
			else if(*it == ">") //> handling
			{
				right++;
				if(right > 2)
				{
					perror("Syntax error. Too many > characters.");
					syntaxerror = true;
					break;
				}
				else if(right == 1)
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
						commands.push_back(">");
					}
					else
					{
						perror("Syntax error. Improper use of connectors.");
						syntaxerror = true;
						break;
					}
				
				}
				else if(right == 2)
				{
					if(semicolon == 0 && ampersand == 0)
					{
						commands.push_back(">");
						right = 0;
					}
					else
					{
						perror("Syntax error. Improper use of connectors.");
						syntaxerror = true;
						break;
					}
				}
				
			}
			else if(*it == "<") //< handling
			{
				left++;
				if(left > 2)
				{
					perror("Syntax error. Too many < characters.");
					syntaxerror = true;
					break;
				}
				else if(left == 1)
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
						commands.push_back("<");
					}
					else
					{
						perror("Syntax error. Improper use of connectors.");
						syntaxerror = true;
						break;
					}
				
				}
				else if(left == 2)
				{
					if(semicolon == 0 && ampersand == 0)
					{
						commands.push_back("<");
						left = 0;
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
				if(semicolon != 0 || ampersand != 0)
				{
					perror("Syntax error. Improper use of connectors.");
					//cout << semicolon << " " << ampersand << " " << pipe << endl;
					syntaxerror = true;
					break;
				}
				if(temp != "")
				{
					temp += ' ';
				}
				temp += *it;
				if(pipe == 1)
					pipe = 0;
				if(right == 1)
					right = 0;
				if(left == 1)
					left = 0;
			}
		}
		commands.push_back(temp.c_str());
		temp = "";
			
		//check commands
		//for(unsigned int i = 0; i < commands.size(); i++)
		//{
		//	cout << "(" << commands.at(i) << ") ";
		//}
		//cout << endl;
		//combine two pipes, two >, and two < together
		bool prevpipe = false;
		bool prevright = false;
		bool prevleft = false;
		for(unsigned int i = 1; i < commands.size(); i++)
		{
			if(commands.at(i - 1) == "|")
				prevpipe = true;
			else
				prevpipe = false;
			
			if(commands.at(i - 1) == ">")
				prevright = true;
			else
				prevright = false;
			
			if(commands.at(i - 1) == "<")
				prevleft = true;
			else
				prevleft = false;

			if(prevpipe)
			{
				if(commands.at(i) == "|")
				{
					commands.at(i - 1) = "||";
					commands.erase(commands.begin() + i);
					i--;
				}
			}
			if(prevright)
			{
				if(commands.at(i) == ">")
				{
					commands.at(i - 1) = ">>";
					commands.erase(commands.begin() + i);
					i--;
				}
			}
			if(prevleft)
			{
				if(commands.at(i) == "<")
				{
					commands.at(i - 1) = "<<";
					commands.erase(commands.begin() + i);
					i--;
				}
			}
		}

		//check commands
		//for(unsigned int i = 0; i < commands.size(); i++)
		//{
		//	cout << "(" << commands.at(i) << ") ";
		//}


		//cout << "combined arguements into groups" << endl;
		if(!syntaxerror)
		{
			char *input[999];
			//exec commands
			for(unsigned int i = 0; i < commands.size(); i++)
			{
				string current = "";
				string word = "";
				int k = 0;
				for(unsigned int j = 0; j < commands.at(i).size(); j++) //itterate through letters
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

				else if(commands.at(i) == "||") //double pipe case
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
				bool outputRedir1 = false;
				bool outputRedir2 = false;
				bool inputRedir = false;

				if(i + 1 < commands.size())
				{
					if(commands.at(i + 1) == ">") //HANDLE >
					{
						outputRedir1 = true;
					}
					if(commands.at(i + 1) == ">>")
					{
						outputRedir2 = true;
					}
					if(commands.at(i + 1) == "<") //HANDLE <
					{
						inputRedir = true;
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
					int fd = 0;
					if(outputRedir1 || outputRedir2)
					{
						if(i + 2 < commands.size())
						{
							if(-1 == close(1))
								perror("close");
							if(outputRedir1) //for >
							{
								if((fd=open(commands.at(i + 2).c_str(),O_CREAT|O_WRONLY|O_TRUNC,0777))==-1)
									perror("open");
							}
							else if(outputRedir2) //for >>
							{
								if((fd=open(commands.at(i + 2).c_str(),O_CREAT|O_WRONLY|O_APPEND,0777))==-1)
									perror("open");
							}
							if((dup2(fd,1))==-1)
							    perror("dup2");
							//if(-1 == close(fd))
							//	perror("close");
						}
					}	
					
					if(inputRedir)
					{
						if(i + 2 < commands.size())
						{
							if(-1 == close(0))
								perror("close");
							if((fd=open(commands.at(i + 2).c_str(),O_RDONLY, 0777)) == -1)
								perror("open");
							if((dup2(fd,0))==-1)
								perror("dup2");
							//if(-1 == close(fd))
							//	perror("close");

						}

					}
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
					if(outputRedir1 || outputRedir2 || inputRedir)
						i = i + 2;
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
