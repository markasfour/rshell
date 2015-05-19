#include <iostream>
#include <stdlib.h>
#include <vector>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
using namespace std;

int main(int argc, char **argv)
{
	if(argc != 3)
	{
		perror("number of arguments");
		exit(1);
	}

	vector <string> files;
	for(int i = 1; i < argc; i++)
	{
		files.push_back(argv[i]);
	}

//	bool dir = false;
	
	struct stat blah;
	if(-1 == stat(files.at(0).c_str(), &blah)) {
		perror("error stat file 1 don't exist");	
	}

	if(!(S_IFREG & blah.st_mode)) {
		//file 1 doesn't exist
		perror("file 1 not a file");
		exit(1);
	}

	if(-1 == stat(files.at(1).c_str(), &blah)) {
		//need to create the file
//		int fd = open(files.at(1).c_str(), O_WRONLY|O_CREAT);
//			if(fd == -1)
//			{
//				perror("cannot open file2");
//				exit(1);
//			}

			if(-1 == link(files.at(0).c_str(), files.at(1).c_str())) {
					perror("eorrror with link");
					exit(1);
			}
			if(-1 == unlink(files.at(0).c_str())) {
					perror("praalam unlink");
					exit(1);
			}
		
//		if(-1 == close(fd)) {
//			perror("error close");
//			exit(1);
//		}


	} else if(S_IFDIR & blah.st_mode) {
		//it's a directory, do that stuff here

			files.at(1) = files.at(1) + '/' + files.at(0);
//			int fd = open(files.at(1).c_str(), O_WRONLY|O_CREAT);
//			if(fd == -1)
//			{
//				cout << "file either exists in directory or problem" << endl;
//				perror("cannot open file2");
//				exit(1);
//			}
			
			if(-1 == link(files.at(0).c_str(), files.at(1).c_str())) {
				perror("eorrror with link");
				exit(1);
			}
			if(-1 == unlink(files.at(0).c_str())) {
				perror("praalam unlink");
				exit(1);
			}
		
//			if(-1 == close(fd)) {
//				perror("error clsoe");
//				exit(1);
//			}

	} else {
		//file already exists
		perror("file already exists");
		exit(1);
	}



/*

	struct stat name;
	if(-1 == stat(files.at(0).c_str(), &name))
	{
		perror("stat");
		exit(1);
	}
	if(S_IFDIR & name.st_mode)
	{
		perror("cannot use dir as 1st arg");
		exit(1);
	}



	if(-1 != stat(files.at(1).c_str(), &name)) //if file 2 does not exist
	{
		int fd = open(files.at(1).c_str(), O_WRONLY|O_CREAT);
		if(fd == -1)
		{
			perror("cannot open file2");
			exit(1);
		}

		if(-1 == link(files.at(0).c_str(), files.at(1).c_str())) {
				perror("eorrror with link");
				exit(1);
		}
		if(-1 == unlink(files.at(0).c_str())) {
				perror("praalam unlink");
				exit(1);
		}
	}
	else if(S_IFREG & name.st_mode) //if file 2 exists
	{
		perror("file exists");
		exit(1);
	}
	else if(S_IFDIR & name.st_mode) //if file 2 is dir
	{
		dir = true;
	}



	
	if(dir)
	{
		if(S_IFREG & name.st_mode) //if file 2 exists
		{
			perror("file exists in dir");
			exit(1);
		}
		else //if file 2 does not exist
		{
			files.at(1) = files.at(1) + '/' + files.at(0);
			int fd = open(files.at(1).c_str(), O_WRONLY|O_CREAT);
			if(fd == -1)
			{
				perror("cannot open file2");
				exit(1);
			}
			
			if(-1 == link(files.at(0).c_str(), files.at(1).c_str())) {
				perror("eorrror with link");
				exit(1);
			}
			if(-1 == unlink(files.at(0).c_str())) {
				perror("praalam unlink");
				exit(1);
			}

		}
	}

	*/

	return 0;
}
