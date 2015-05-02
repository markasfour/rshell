#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <deque>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>
#include <algorithm>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <iomanip>
using namespace std;


//Globalized flags
bool aFlag = false;
bool lFlag = false;
bool RFlag = false;



void organize(deque <string> &files)
{
	sort(files.begin(), files.end(), locale("en_US.UTF-8"));
}

void printme(string file, struct stat name)
{
	if(file.at(0) == '.') //if hidden
	{
		printf("%c[%dm", 0x1B, 100);
		if(name.st_mode & S_IFDIR) //hidden dir
		{
			printf("%c[%dm", 0x1B, 94);
			cout << file;
			printf("%c[%dm", 0x1B, 0);
		}
		else if(name.st_mode & S_IXUSR) //hidden exec
		{
			printf("%c[%dm", 0x1B, 92);
			cout << file;
			printf("%c[%dm", 0x1B, 0);
		}
		else //hidden reg
		{
			cout << file;
			printf("%c[%dm", 0x1B, 0);
		}
	}
	else //not hidden
	{
		if(name.st_mode & S_IFDIR) //dir
		{
			printf("%c[%dm", 0x1B, 94);
			cout << file;
			printf("%c[%dm", 0x1B, 0);
		}
		else if(name.st_mode & S_IXUSR) //exec
		{
			printf("%c[%dm", 0x1B, 92);
			cout << file;
			printf("%c[%dm", 0x1B, 0);
		}
		else //reg
		{
			cout << file;
		}
	}
}

void printlFlag(struct stat name)
{
	if(name.st_mode & S_IFDIR)
		cout << 'd';
	else if(!(name.st_mode & S_IFLNK)) //! because prints otherwise 
		cout << 'l';
	else
		cout << '-';
	

	cout << ((name.st_mode & S_IRUSR)? "r" : "-" );
	cout << ((name.st_mode & S_IWUSR)? "w" : "-" );
	cout << ((name.st_mode & S_IXUSR)? "x" : "-" );
	cout << ((name.st_mode & S_IRGRP)? "r" : "-" );
	cout << ((name.st_mode & S_IWGRP)? "w" : "-" );
	cout << ((name.st_mode & S_IXGRP)? "x" : "-" );
	cout << ((name.st_mode & S_IROTH)? "r" : "-" );
	cout << ((name.st_mode & S_IWOTH)? "w" : "-" );
	cout << ((name.st_mode & S_IXOTH)? "x" : "-" );
	cout << " " << name.st_nlink << " ";

	struct passwd* p;
	if((p = getpwuid(name.st_uid)) == NULL)
	{
		perror("getpwuid error");
		exit(1);
	}
	else
		cout << p->pw_name << " ";
	

	struct group* g;
	if((g = getgrgid(name.st_gid)) == NULL)
	{
		perror("getgrgid error");
		exit(1);
	}
	else
		cout << g->gr_name << " ";
	
	cout << setw(5) << right;
	cout << name.st_size << " ";

	string time = ctime(&name.st_mtime);
	if(time.at(time.size() - 1) == '\n')
		time.at(time.size() - 1) = '\0';
	cout << time << " ";
}



void lsDir(deque <string> files, deque <string> directories, struct stat name, bool otherOutput)
{
	deque <string> recurrence;

	organize(directories);
	for(unsigned int i = 0; i < directories.size(); i++)
	{
	
		//cout << "curr dir: " << directories.at(i) << endl;
		DIR* dp;
		if(!(dp = opendir(directories.at(i).c_str())))
		{
			perror("cannot open dir");
			exit(1);
		}
		dirent* direntp;
		while((direntp = readdir(dp)) != 0) //extract files from directory
		{
			files.push_back(direntp->d_name);


			//for recurrence case
			string path = directories.at(i) + "/" + direntp->d_name;
			if(-1 == (stat(path.c_str(), &name)))
			{
				perror("stat error 1");
				exit(1);
			}
			else if(name.st_mode & S_IFDIR) 
			{	
				if(!aFlag)
				{
					string here = direntp->d_name;
					if(here.at(0) == '.')
						continue;
				}
				
				string cur = ".";
				string pre = "..";
				if(direntp->d_name != cur && direntp->d_name != pre)
					recurrence.push_back(directories.at(i) + "/" + direntp->d_name);
			}
		}
		//for(unsigned int w = 0; w < recurrence.size(); w++)
		//	cout << "r: " << recurrence.at(w) << endl;

		
		
		organize(files); //sort extracted files
		organize(recurrence); //sort extracted directories
		
		if(!aFlag) //remove hidden files
		{
			for(unsigned int k = 0; k < files.size(); k++)
			{
				if(files.at(k).at(0) == '.')
				{
					files.erase(files.begin() + k); //erase hidden files
					k--;
				}
			}
		}

		//for output formatting
		unsigned int largest = 0;
		for(unsigned int q = 0; q < files.size(); q++)
		{
			if(files.at(q).size() > largest)
				largest = files.at(q).size();
		//	cout << q << "largest: " << largest << endl;
		}
		largest += 2; //regular ls has two spaces after each file
		//cout << largest << endl;
		unsigned int cols = 80/largest; //typical terminal holds 80 char


		if(lFlag)
		{
			int total = 0;
			for(unsigned int h = 0; h < files.size(); h++) //get total block size
			{
				string path = directories.at(i) + "/" + files.at(h);
				if(-1 == (stat(path.c_str(), &name)))
				{
					perror("stat error 1");
					exit(1);
				}
				else
					total += name.st_blocks;

				//cout << total << endl;
			}
			cout << "total " << total/2 << endl; //output total block size
		}

		if((directories.size() > 1) || otherOutput || RFlag) //output directory currently working on
		{	
			if(stat(directories.at(i).c_str(), &name) == -1) //assuming no error. just changing name
				perror("stat error");
			printme(directories.at(i), name);
			cout << ":" << endl;
		}	
		
		unsigned int currcol = 0;
		for(unsigned int j = 0; j < files.size(); j++)
		{
			string path = directories.at(i) + "/" + files.at(j);

			if(lFlag)
			{
					if(stat(path.c_str(), &name) == -1)
					perror("stat error 2");
				
				printlFlag(name);
			}

			if(stat(path.c_str(), &name) == -1) //assuming no error. just changing name
				perror("stat error");
		
			cout << setw(largest) << left; //format output
			printme(files.at(j), name);
			if(!lFlag)
			{
				currcol++;
				if(currcol == cols)
				{
					currcol = 0;
					cout << endl;
				}
			}
			if(lFlag) 
				cout << endl;
		}
		cout << endl << endl;
		files.clear();
	}
	if(RFlag) //recurrence flag
	{
		if(recurrence.size() > 0)
		{
			for(unsigned int k = 0; k < recurrence.size(); k++)
			{
				if(recurrence.at(k) == "." || recurrence.at(k) == "..")
				{
					recurrence.erase(recurrence.begin() + k); //erase current and previous dir
					k--;
				}
			}
			lsDir(files, recurrence, name, otherOutput); //recurrence statement
		}
	}

}



int main(int argc, char* argv[])
{

	//cout << "argc = " << argc << endl;	
	
	bool filesonly = false;

	deque <string> arguments;
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
	deque <string> directories;
	deque <string> flags;
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
	
	//if(aFlag || lFlag || RFlag)
	//	cout << "yay" << endl;

	
	struct stat name;
	
	if(directories.size() == 0) //case where no arguments are passed in
		directories.push_back("."); //add current directory as default


	//handle case if file names are passed in
	deque <string> files;
	if(directories.size() > 0)
	{
		for(unsigned int i = 0; i < directories.size(); i++)
		{
			//DIR *directory;
			
			//Check if argument is a file
			if(stat(directories.at(i).c_str(), &name) == -1)
			{
				perror("stat error");
				directories.erase(directories.begin() + i);
				i--;
			}
			else
			{
				if(!(name.st_mode & S_IFDIR))
				{
					files.push_back(directories.at(i));
					directories.erase(directories.begin() + i);
					i--;
				}
			}
		}
	}
	if(directories.size() == 0)
	{
		filesonly = true;
	}

	//for(unsigned int i = 0; i < directories.size(); i++)
	//	cout << "Listed as dir: " << directories.at(i) << endl;


	//cout << filesonly << endl;
	//for(unsigned int i = 0; i < files.size(); i++)
	//{
	//	cout << "Tracked files: " << files.at(i) << endl;
	//}

	organize(files); //sort files alphabetically

	//for(unsigned int i = 0; i < files.size(); i++)
	//{
	//	cout << "Sorted files: " << files.at(i) << endl;
	//}


	if(filesonly)
	{
		if(!lFlag) //print without -l descriptors
		{
			for(unsigned int i = 0; i < files.size(); i++)
				cout << files.at(i) << "  ";
			cout << endl;
			return 0; //end program
		}
		else //only l flag has effect when only file names a inputted
		{
			for(unsigned int i = 0; i < files.size(); i++)
			{
				if(stat(files.at(i).c_str(), &name) == -1)
					perror("stat error");
				
				printlFlag(name);
				printme(files.at(i), name);
				cout << endl;
			}
			return 0; //end program
		}
	}



	else //case with directories as args
	{
		bool otherOutput = false;
		
		for(unsigned int i = 0; i < files.size(); i++)
		{	
			if(stat(files.at(i).c_str(), &name) == -1) //assuming no error. just changing name
				perror("stat error");
			printme(files.at(i), name);
			cout << "  "; //print out file arguments first
		}
		if(files.size() > 0)
		{	
			cout << endl << endl;
			otherOutput = true;
		}
		files.clear();
		
		lsDir(files, directories, name, otherOutput);
	}
	return 0;
}
