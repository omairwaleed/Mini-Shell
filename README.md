# Mini Shell Program
## Overview
This program runs on a linux os,uses lex and yacc for parsing and you can run many commands as in basic terminal as:
* ls 
* ls -al 
* ls -al aaa bbb cc 
* ls -al aaa bbb cc > outfile
* ls -al aaa bbb cc >> outfile 
* cat file | grep text 
* ls | cat | grep > out < inp 
* ls aaaa | grep cccc | grep jjjj ssss dfdffd httpd &
* exit : this command exit the shell

## Installation "prerequisits"
* You need to install make if it isn't installed on your Ubuntu
```
sudo apt install make -y
```
* You need to install Lex package
```
sudo apt-get install flex
```
* You need to install Yacc package
```
sudo apt-get install bison
```

## Instructions
You have to run Build the shell program by typing :
```
make
```
To run it type:
```
./shell
```