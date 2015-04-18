# rshell

Shell Terminal for UCR CS100 Software Construction Course
This program is a custom terminal that can run general bash commands.

##Install
To install and run rshell, enter the following into a terminal: 

'''
1. git clone http://www.github.com/markasfour/rshell.git
2. cd rshell
3. git checkout hw0
4. make
5. bin/rshell
'''

##Program Features
Rshell runs all bash commands
Rshell supports the use of connectors such as ```;```, ```&&```, and ```||```
Rshell supports left to right connector logic sequencing (i.e. ```ls && pwd || exit``` would result in ```ls``` being evaluated first, ```pwd``` being evaluated next, and ```exit``` being evaluated last) 
Rshell provides proper error messages for use of improper syntax and invalid commands
Rshell allows for the use of commenting through the use of ```#```
Rshell has a custom exit command allowing the user to exit by using ```exit``` as a command


##Bugs and Limitations
```echo "hello world"``` will include the quotation marks
Host name is limited to 64 characters
Commands such as ```cd``` are not supported in this make of Rshell
Colors are not supported in this make of Rshell
Text navigation (such as arrow keys) is not supported by this make of Rshell
Up arrow key does not run the previous command

