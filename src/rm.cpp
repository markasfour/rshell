#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <vector>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
using namespace std;

bool rFlag = false;	


void rmFunct(vector<string> files)
{
	vector<string> dirs;

	struct stat name;

	for(unsigned int i = 0; i < files.size(); i++)
	{
		if(-1 == stat(files.at(i).c_str(), &name))
		{
			perror("no");
			exit(1);
		}

		if(S_IFDIR & name.st_mode)
		{
			if(rFlag)
			{
				//cout << "YOLO" << endl;
				//cout << "files: " << files.at(i) << endl;
				if(files.at(i) != "." && files.at(i) != "..") {
					dirs.push_back(files.at(i) + "/");
				}
			} else { 
				cout << "Can't remove directory without -r flag" << endl;
				exit(1);
			}
		}
		else if(S_IFREG & name.st_mode)
		{

			if(files.at(i) != "." && files.at(i) != "..") {
				if(-1 == unlink(files.at(i).c_str()))
				{
					perror("praalam with unlink");
					exit(1);
				}
			}
		
		}
	}

	if(rFlag) {
		//cout << "SWAG" << endl;
		files.clear();
		DIR* dp;

		for(unsigned int i = 0; i < dirs.size(); i++)
		{
			if(!(dp = opendir(dirs.at(i).c_str())))
			{
				perror("cannot open dir");
				exit(1);
			}
			dirent* direntp;
			while((direntp = readdir(dp)) != 0) //extract files from directory
			{
				//cout << "asdfsf " << dirs.at(i) + direntp->d_name << endl;
				string cur = ".";
				string pre = "..";
				if(direntp->d_name != cur && direntp->d_name != pre)
					files.push_back(dirs.at(i) + direntp->d_name);
				
			}
			rmFunct(files);
			if(-1 == rmdir(dirs.at(i).c_str()))
			{
				perror("praalam removing directory");
				exit(1);
			}
		}
	}


}




int main(int argc, char **argv)
{
	if(argc < 2)
	{
		perror("no arguments passed in");
		exit(1);
	}


	vector <string> arguments;
	vector <string> files;

	for(int i = 1; i < argc; i++)
	{
		arguments.push_back(argv[i]);
	}

	//for(unsigned int i = 0; i < arguments.size(); i++)
	//{
	//	cout << arguments.at(i) << endl;
	//}


	for(unsigned int i = 0; i < arguments.size(); i++)
	{
		if(arguments.at(i).at(0) == '-')
		{
			if(arguments.at(i).size() > 1)
			{
				for(unsigned int j = 1; j < arguments.at(i).size(); j++)
				{
					if(arguments.at(i).at(j) == 'r')
					{
						//cout << "rFlag " << rFlag << endl;
						rFlag = true;
					}
					else
					{
						perror("unsupported flag");
						exit(1);
					}
				}
			}
		}

		else
		{
			files.push_back(arguments.at(i));
		}
	}

	rmFunct(files);
	//cout << "YEAH" << endl;
	return 0;
}
