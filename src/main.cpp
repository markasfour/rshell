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

//FUNCTION TO HANDLE <
void inRe(char* arg[], int index)
{
	arg[index] = 0;
	int fd; 
	if(-1 == (fd = open(arg[index + 1], O_RDONLY)))
	{
		perror("open failed");
		
		exit(1);
	}
	if(dup2(fd, 0) == -1)
	{
		perror("dup2");

		exit(1);
	}
}

//FUNCTION TO HANDLE > AND >>
void outRe(char* arg[], int index, bool second, int ex)//ex is the numused for the extra credit
{
	arg[index] = 0;
	int fd;
	if(!second)
	{
		if(-1 == (fd = open(arg[index + 1], O_WRONLY|O_CREAT|O_TRUNC, 0777)))
		{
			perror("open failed");

			exit(1);
		}
	}
	else //if >>
	{
		if(-1 == (fd = open(arg[index + 1], O_WRONLY|O_CREAT|O_APPEND, 0777)))
		{
			perror("open failed");

			exit(1);
		}
	}


	if(ex == -1)
	{
		if(dup2(fd,1) == -1)
		{
			perror("dup2");

			exit(1);
		}
	}
	else
	{
		if(dup2(fd,ex) == -1)
		{
			perror("dup2");

			exit(1);
		}
	}
}


void placeholder(string &command, string connector, int length)
{
	while(command.find(connector) != string::npos)
	{
		int index = command.find(connector);
		command.replace(index, length, " . ");
	}
}

int findIORe(char* input[], int x, string IO)
{
	for(int i = 0; i < x; i++)
	{
		if(0 ==(strcmp(input[i], IO.c_str())))
			return i;
	}
	return -1;
}


void ExecuteCommand(char* input[], int x)
{
	//find all the things for HW2
	
	int leftIO = findIORe(input, x, "<");
	int rightIO = findIORe(input, x, ">");
	int right2IO = findIORe(input, x, ">>");
}

void magic(char* input[], int x)
{
	ExecuteCommand(input, x);
}

bool rshell(char hostarray[64], bool finish, string login)
{
	//login name and host info prompt
	if(getlogin() != NULL)
		cout << login << "@" << hostarray;

	//ready prompt
	cout << "$ ";

	string command = "";
	//take in command from user
	getline(cin, command);
	//cout << command << endl;
	
	//account for empty command
	if(command == "")
		return false;
	
	//account for #
	if(command.find('#') != string::npos)
	{
		command = command.substr(0, command.find('#'));
		//cout << command << endl;
	}

	//account for spaces at start of command
	for(unsigned int i = 0; i < command.size(); i++)
	{
		if(command.at(i) != ' ')
			break;
		else if(command.at(i) == ' ')
		{
			command.erase(i, 1);
			i--;
		}
	}

	//account for spaces at end of command
	for(unsigned int i = command.size() - 1; command.at(i) == ' '; i--)
	{
		command.erase(command.size() - 1, 1);
	}

	bool semicolon = false;
	bool ampersand = false;
	bool pipe = false;
	int length = 0;
	string connector = "";

	//find ;
	if(command.find(';') != string::npos)
	{
		semicolon = true;
		length = 1;
		connector = ";";
	}
	//find &&
	else if(command.find("&&") != string::npos)
	{
		ampersand = true;
		length = 2;
		connector = "&&";
	}
	//find ||
	else if(command.find("||") != string::npos)
	{

		pipe = true;
		length = 2;
		connector = "||";
	}


	//Redir(command);
	
	if(semicolon || ampersand || pipe)
	{
		while(command.find(connector) != string::npos)
		{
			int find = command.find(connector);
			command.replace(find, length, " . ");
		}
		for(unsigned int i = command.size() - 1; command.at(i) == ' '; i--)
		{
			command.erase(command.size() - 1, 1);
		}

	}

	
	//tokenize
	vector<string> mytok;
	char_separator<char> spaces(" ");
	//tokenize by spaces
	tokenizer<char_separator<char> > toks(command, spaces);
	for(tokenizer<char_separator<char> >::iterator it = toks.begin(); it != toks.end(); it++)
	{
		mytok.push_back(*it);
	}
	//check if connector without prev argument
	if(mytok.at(0) == ",")
	{
		perror("syntax error");
		
		return false;
	}
	//check for connectors without argument in between
	for(unsigned int i = 1; i < mytok.size(); i++)
	{
		if(mytok.at(i) == "," && mytok.at(i - 1) == ",")
		{
			perror("syntax error");

			return false;
		}
	}

	//tokenize by placeholders
	char_separator<char> seps(",");
	tokenizer<char_separator<char> > toks2(command, seps);
	//main loop to iterate through
	for(tokenizer<char_separator<char> >::iterator it = toks2.begin(); it != toks2.end(); it++)
	{

		int count = 0;
		int j = 0;
		tokenizer<char_separator<char> > toks3(*it, spaces);
		for(tokenizer<char_separator<char> >::iterator it2 = toks3.begin(); it2 != toks3.end(); it2++)
		{
			count++; //count = size of toks3
		}
		
		char** input = new char*[count + 1];

		for(tokenizer<char_separator<char> >::iterator it2=toks3.begin(); it2 != toks3.end(); it2++)
		{
			//add arguments one token at a time
			string a = *it2;
			input[j] = new char[a.size()];
			strcpy(input[j], a.c_str());

			j++;
		}


		//check if current arg is exit
		if(!(strcmp(input[0], "exit")))
		{
			cout << "Ending session ..." << endl;
			return true; //ONLY TIME TO RETURN TRUE!!!!!!!!!!!!!!!!!!!!!!!!
		}

		input[j] = NULL;


		int pid;
		if(-1 == (pid = fork()))
		{
			perror("fork");
			//FIX MEMORY LEAK!!!!!!
			delete[] input;
			return false;
		}
		else if(pid == 0) //child
		{
			magic(input, j); //this is where the magic happens :)
		}
		else //parent
		{
			if(-1 == waitpid(pid, &status, 0)) //wait for child to finish
				perror("There was an error with wait().");
			
			if(semicolon)
				continue;
			if((status == 0) && ampersand)
				continue;
			if((status > 0) && pipe)
				continue;

			delete input;
			return false;
		}
	}
	return false;
}


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
		finish = rshell(hostarray, finish, login);
	}
	cout << "goodbye!" << endl;
	/*	bool syntaxerror = false;
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
		
		//remove spaces
		for(unsigned int i = 0; i < command.size(); i++)
		{
			if(command.at(i) == ' ')
			{
				command.erase(i,1);
				i--;
			}
		}

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
			char **input = new char*[999];
			//exec commands
			for(unsigned int i = 0; i < commands.size(); i++)
			{
				string current = "";
				string word = "";
				int k = 0;
				for(unsigned int j = 0; j < commands.at(i).size(); j++) //itterate through letters
				{
					current = commands.at(i);
					//cout << "current: " << current << endl;
					if(current[j] == ' ')
					{
						input[k] = new char[word.size() + 1];
						strcpy(input[k], word.c_str());
						k++;
						word = "";
					}
					else
						word += current[j]; //add letter
					//cout << "word: " << word << endl;
				}	
				input[k] = new char[word.size() + 1];
				strcpy(input[k], word.c_str());
				k++;

				//input[k] = new char[1]; //add the NULL char *
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
						delete[] input;	
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
				for(int z = 0; z <= k; z++)
				{
					delete[] input[z];
				}
				delete[] input;
			}

			//shell termination
			if(finish)
			{
				cout << "good-bye!" << endl;
				break;
			}
		}
	}*/
	return 0;
}
