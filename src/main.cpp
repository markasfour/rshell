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
#include <signal.h>
using namespace std;
using namespace boost;


void handle(int x)
{
	if(x == SIGINT)
	{
	}
	if(x == SIGTSTP)
	{
        int y = 0;
		if(-1 == (y = kill(getpid(), SIGSTOP)))
		{
			perror("kill");
			exit(1);
		}
	}
}


void placeholder(string &command, string connector, int length)
{
	while(command.find(connector) != string::npos)
    {
		int index =command.find(connector);
		command.replace(index, length, " , ");
	}
}


bool parseIO(string & command, int & ex, char & numChar)
{
	int in = 0;
	bool manyleft = false;
	
	int out = 0;
	bool manyright = false;
	for(int i=0;(size_t)i < command.size();i++)
	{
		//cout << "HERE WE HAVE: " << command.at(i) << endl;
		if(command.compare(i,1,"<")==0)
		{
			in++;
			if(in > 1)
			{
				manyleft = true;
			}
			command.replace(i,1," < ");
			i = i+3;
		}
		else if((command.compare(i,1,">")==0)&& (command.compare(i+1,1,">")!=0) && (command.compare(i-1,1,">")!=0)) 
		{
			out++;
			if(out > 1)
			{
				manyright = true;
			}
			if(isdigit(command.at(i-1)))
			{
				//cout << command.at(i-1) << endl;
				ex = command.at(i-1) - '0';//convert char to num value
				numChar = command.at(i-1);
				string temp="";
				temp.append(" ");
				temp.push_back(numChar);
				temp.append("> ");
				command.replace(i-1,2,temp);
				i=i+3;
			}
			else
			{
				command.replace (i, 1, " > ");
				i = i+3;
			}
		}
		else if(command.compare(i,2,">>")==0)
		{
			out++;
			if(out > 1)
			{
				manyright = true;
			}
			if(isdigit(command.at(i-1)))
			{
				//cout << command.at(i-1) << endl;
				ex = command.at(i-1) - '0'; //convert chart to num value
				numChar = command.at(i-1);
				string temp="";
				temp.append(" ");
				temp.push_back(numChar);
				temp.append(">> ");
				command.replace(i-1,3,temp);
				i=i+4;
			}
			else
			{
				command.replace (i, 2, " >> ");
				i = i+4;
			}
		}
		else if(command.compare(i,1,"|") ==0)
		{
			command.replace(i,1," | ");
			i = i+3;
		}
		if(manyleft || manyright)
		{
			//cout << "in:" << in << "out:" << out << endl;
			cout << "Error: Too many instances of redirection." << endl;
			
			return false;
		}
	}
	return true;
}


int findIORe(char* input[], int x, string IO) //returns index of > >> < or |
{
	for(int i = 0; i < x; i++)
	{
		if(0 ==(strcmp(input[i], IO.c_str())))
			return i;
	}
	return -1;
}

//FUNCTION TO HANDLE <
void inRe(char* arg[], int index)
{
	arg[index] = 0;
	int fd = open(arg[index + 1], O_RDONLY); 
	//cout << arg[index + 1] << endl;
	if(-1 == fd)
	{
		perror("open failed");
		
		exit(1);
	}
	if(dup2(fd, 0) == -1)
	{
		perror("dup2");
		cout << "" << endl;
		exit(1);
	}
	//cout << "wut" << endl;
}

//FUNCTION TO HANDLE > AND >>
void outRe(char* arg[], int index, bool second, int ex)//ex is the numused for the extra credit
{
	//cout << arg[index] << endl;
	arg[index] = NULL;
	int fd;
	if(!second)
	{
		//cout << "EXECUTING: " << endl;
		//for(int i = 0; arg[i] != NULL; i++)
		//	cout << arg[i] << endl;

		if(-1 == (fd = open(arg[index + 1], O_WRONLY|O_CREAT|O_TRUNC, 0777)))
		{
			perror("open failed");

			exit(1);
		}
		//cout << "WOW IM HERE" << endl;
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
		//cout << "IM HERE NOW" << endl;
		if(dup2(fd,1) == -1)
		{
			perror("error with dup2");

			exit(1);
		}
		//cout << "WHOA" << endl;
	}
	else
	{
		if(dup2(fd,ex) == -1)
		{
			perror("error with dup2");

			exit(1);
		}
	}
}


void ExecuteCommand(char* Arg[], int size, int num, char numChar)
{
	//find all the things for HW2
	
	string numOut = "";
	numOut.push_back(numChar);
	numOut.append(">");
	//cout << numOut << endl;
	string numOut2 = "";
	numOut2.push_back(numChar);
	numOut2.append(">>");
	//cout << numOut2 << endl;
	int in = findIORe(Arg,size,"<");
	int out = findIORe(Arg,size,">");
	int out2 = findIORe(Arg,size,">>");
	int out3 = findIORe(Arg,size,numOut);
	int out4 = findIORe(Arg,size,numOut2);
	
	//RUN LIKE HW0
	if(-1 == in && -1 == out && -1 == out2 && -1 == out3 && -1 == out4)
	{
		if(execvp(Arg[0],Arg) == -1)
		{
			perror("execvp");
			exit(0);
		}
		return;
	}

	//HW2 stuff
	if(in != -1)
	{
		inRe(Arg,in);
	}
	if(out != -1)
	{
		outRe(Arg,out,false,num);
	}
	else if(out2 != -1)
	{
		outRe(Arg,out2,true,num);
	}
	else if(out3 != -1)
	{
		outRe(Arg,out3,false,num);
	}
	else if(out4 != -1)
	{
		outRe(Arg,out4,true,num);
	}
	//EXECUTE
	if(execvp(Arg[0],Arg) == -1)
	{
		perror("execvp");
		exit(0);
	}
	
}

void magic(char* input[], int x, int ex, char numChar)
{
	int piping;
	if(-1 == (piping = findIORe(input, x, "|")))//if there is no piping, handle like HW0
	{
		ExecuteCommand(input, x, ex, numChar);
	}
	else //piping
	{
		//cout << "LOC OF PIPE: " << piping << endl;
		int afterpiping = 0; //used to fill in array of right side of pipe
		char* left[999]; //left of the pipe
		char* right[999]; //right of the pipe;
		
		for(int i = 0; i < piping; i++)
		{
			left[i] = input[i]; //fill first half of input into left
		}
		left[piping] = '\0';

		for(int i = piping + 1; input[i] != '\0'; i++)
		{
			right[afterpiping] = input[i];
			afterpiping++;
		}
		right[afterpiping] = '\0';


		//based off of Izbicki's lecture......hope this works
		int fd[2];
		if(pipe(fd) == -1)
		{
			perror("error with pipe");
			exit(1);
		}
		int pid;
		if(-1 == (pid = fork()))
		{
			perror("error with fork");
			exit(1);
		}
		else if(pid == 0) //child
		{
			if(-1 == dup2(fd[1],1))
			{
				perror("error with dup2");
				exit(1);
			}
			if(-1 == close(fd[0]))
			{
				perror("error with close");
				exit(1);
			}

			ExecuteCommand(left, piping, ex, numChar); //run left of pipe
		}

		else //parent
		{
			int in = 0; //standard in

			if(-1 == close(fd[1]))
			{
				perror("error with close");
				exit(1);
			}

			//in = dup(0);
			if(-1 == (in = dup(0)))
			{
				perror("error with dup");
				exit(1);
			}
			if(-1 == dup2(fd[0],0))
			{
				perror("error with dup2");
				exit(1);
			}
			//wait for child
			if(wait(0) == -1) //wait(0) is wait for child
			{
				perror("error with wait");
				exit(1);
			}

			//RECURSION FOR MULTIPLE PIPES :D
			magic(right, afterpiping, ex, numChar);

			if(-1 == dup2(in, 0))
			{
				perror("error with dup2");
				exit(1);
			}

			
		}
	}
}

bool rshell(char hostarray[64], bool finish, string login)
{
	int status = 0;
	char numChar = '\0';
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
		int comment = command.find("#");
		if(comment == 0)
			return false;
		command.erase(comment, command.size() - comment);
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
	int ex = -1; //used for extra credit

	//find ;
	if(command.find(';') != string::npos)
	{
		semicolon = true;
		length = 1;
		connector = ";";
	}

	if(semicolon)
	{
		while(command.find(connector) != string::npos)
		{
			int find = command.find(connector);
			command.replace(find, length, " % ");
		}
		for(unsigned int i = command.size() - 1; command.at(i) == ' '; i--)
		{
			command.erase(command.size() - 1, 1);
		}

	}

	//find &&
	if(command.find("&&") != string::npos)
	{
		ampersand = true;
		length = 2;
		connector = "&&";
	}

	if(ampersand)
	{
		while(command.find(connector) != string::npos)
		{
			int find = command.find(connector);
			command.replace(find, length, " % ");
		}
		for(unsigned int i = command.size() - 1; command.at(i) == ' '; i--)
		{
			command.erase(command.size() - 1, 1);
		}

	}

	//find ||
	if(command.find("||") != string::npos)
	{

		pipe = true;
		length = 2;
		connector = "||";
	}


	if(pipe)
	{
		while(command.find(connector) != string::npos)
		{
			int find = command.find(connector);
			command.replace(find, length, " % ");
		}
		for(unsigned int i = command.size() - 1; command.at(i) == ' '; i--)
		{
			command.erase(command.size() - 1, 1);
		}

	}

	bool worked = true;
	worked = parseIO(command, ex, numChar);
	if(!worked)
		return false;
	//for(unsigned int i = 0; i < command.size(); i++)
	//{
	//	cout << command.at(i) << endl;
	//}
	
	//tokenize
	vector<string> mytok;
	char_separator<char> spaces(" ");
	//tokenize by spaces
	tokenizer<char_separator<char> > toks(command, spaces);
	for(tokenizer<char_separator<char> >::iterator it = toks.begin(); it != toks.end(); it++)
	{
		mytok.push_back(*it);
	}
	//for(unsigned int i = 0; i < mytok.size(); i++)
	//{
	//	cout << "(" << mytok.at(i) << ") ";
	//}
	//cout << endl;
	
	//check if connector without prev argument
	if(mytok.at(0) == "%")
	{
		perror("syntax error");
		cout << "" << endl; //THIS HAS TO B HERE TO WORK APPARENTLY	
		return false;
	}
	//check for connectors without argument in between
	for(unsigned int i = 1; i < mytok.size(); i++)
	{
		if(mytok.at(i) == "%" && mytok.at(i - 1) == "%")
		{
			perror("syntax error");
			cout << "" << endl; //THIS HAS TO BE HERE TO WORK APPARENTLY...
			return false;
		}
	}

	//tokenize by placeholders
	char_separator<char> seps("%");
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
			input[j] = new char [a.size()];
			strcpy(input[j], a.c_str());

			j++;
		}

		//check if current arg is exit
		if(0 == (strcmp(input[0], "exit")))
		{
			cout << "Ending session ..." << endl;
			return true; //ONLY TIME TO RETURN TRUE!!!!!!!!!!!!!!!!!!!!!!!!
		}

		input[j] = NULL;


		int pid;
		if(-1 == (pid = fork()))
		{
			perror("fork");
			//FIXed MEMORY LEAK!!!!!!
			delete[] input[j];
			return false;
		}
		else if(pid == 0) //child
		{
			//cout << "EXECUTING: " << endl;
			//for(int i = 0; input[i] != NULL; i++)
			//	cout << input[i] << endl;
	

			magic(input, j, ex, numChar); //this is where the magic happens :)
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
	string login;// = getlogin();
	if(getlogin() == NULL)
	{
		login = "";
		perror("get login failed");
	}
	else
	{
		login = getlogin();
	}
	char hostarray[64];
	//gethostname(hostarray, 64);
	if(gethostname(hostarray, 64) == -1)
		perror("get host name failed");
	
	
    //signals	
	struct sigaction a;
	a.sa_handler = &handle;
	int b;
	if(-1 == (b = sigaction(SIGINT, &a , NULL)))
	{
		perror("sigaction");
		exit(1);
	}
	if(-1 == (b = sigaction(SIGQUIT, &a , NULL)))
	{
		perror("sigaction");
		exit(1);
	}
	if(-1 == (b = sigaction(SIGTSTP, &a , NULL)))
	{
		perror("sigaction");
		exit(1);
	}
	
	
	//rshell loop
	while(!finish)
	{
		cin.clear();
		finish = rshell(hostarray, finish, login);
	}
	cout << "goodbye!" << endl;
	
	return 0;
}
