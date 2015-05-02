#include <iostream>
#include <vector>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <fstream>
#include <stdlib.h> 

#include "Timer.h"
using namespace std;
 
int main(int argc, char** argv)
{
	bool no_time = false;
	if (argc == 3)
		no_time = true;

	string arg1 = argv[1];
    string arg2 = argv[2];
     
    //check if files exist
    int fd = open(arg1.c_str(),O_RDONLY);
    if (fd==-1) 
    {
		perror("source file not found");
        exit(1);
    }
    if ( -1 == close(fd))
    {
            perror("close failed");
            exit(1);
    }
     
    fd = open(arg2.c_str(),O_RDWR);
    if (fd!=-1) {
		perror("destination file already exists");
        exit(1);
    }


	if (no_time)
		goto HERE;

	if (true)
	{
		Timer t;
		double eTime;
		t.start();
	    //METHOD 1
	    ifstream in(arg1.c_str());
	    ofstream outfile(arg2.c_str());
	    char c;
		
		while(in.get(c))
		{
			outfile.put(c);
		}
		in.close();

		t.elapsedUserTime(eTime);
		cout << eTime << endl;
		t.elapsedSystemTime(eTime);
		cout << eTime << endl;
		t.elapsedWallclockTime(eTime);
		cout << eTime << endl;
	}	
	
	if (true)
	{
		//MED 2
		Timer t;
		double eTime;
		t.start();
		
		int fdnew;
		int fdold;
		if(-1 == (fdnew = open(arg2.c_str(), O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR))) 
		{
		    	perror("There was an error with open(). ");
		        	exit(1);
		}   
		if(-1 == (fdold = open(arg1.c_str(), O_RDONLY))) 
		{
		    	perror("There was an error with open(). ");
		        	exit(1); 
		}
		int size = 1;
		char cc[1];
		while (size != 0)
		{
			if (-1 == (size = read(fdold, cc, sizeof(cc)))) 
			{
		    		perror("There was an error with read(). ");
		        		exit(1); 
			}
			if (-1 == write(fdnew, cc, size))
			{
	    	    	perror("There was an error with write(). ");
	    	    	exit(1);
			}
		}

		if (-1 == close(fdnew))
		{
		    	perror("There was an error with close(). ");
		        	exit(1);
		}
		if (-1 == close(fdold))
		{
		    	perror("There was an error with close(). ");
		        	exit(1);
		}

		t.elapsedUserTime(eTime);
		cout << eTime << endl;
		t.elapsedSystemTime(eTime);
		cout << eTime << endl;
		t.elapsedWallclockTime(eTime);
		cout << eTime << endl;

	}

	if (true)
	{
		//MED 3
		Timer t;
		double eTime;
		t.start();


		HERE:

		int fdnew1;
		int fdold1;
		if(-1 == (fdnew1 = open(arg2.c_str(), O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR))) 
		{
			    perror("There was an error with open(). ");
			        exit(1);
		}   
		if(-1 == (fdold1 = open(arg1.c_str(), O_RDONLY))) 
		{
			    perror("There was an error with open(). ");
			        exit(1); 
		}
		int size1;
		char c1[BUFSIZ];
		if(-1 == (size1 = read(fdold1, c1, sizeof(c1)))) 
		{
			    perror("There was an error with read(). ");
			        exit(1); 
		}
		if(-1 == write(fdnew1, c1, size1))
		{
		    	perror("There was an error with write(). ");
		    	exit(1);
		}

		if(-1 == close(fdnew1))
		{
			    perror("There was an error with close(). ");
			        exit(1);
		}
		if(-1 == close(fdold1))
		{
			    perror("There was an error with close(). ");
			        exit(1);
		}
	

		if(no_time)
			return 0;
		t.elapsedUserTime(eTime);
		cout << eTime << endl;
		t.elapsedSystemTime(eTime);
		cout << eTime << endl;
		t.elapsedWallclockTime(eTime);
		cout << eTime << endl;

	}
	return 0;
}
