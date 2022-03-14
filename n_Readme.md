
Name: Neelesh Palaparthi
Blazer ID: palaparn
project#: Homework 3

## 1. -e "<unix-command with arguments>"
For each file that matches the search criteria the UNIX command specified with
arguments must be executed.

## 2. -E "<unix-command with arguments>" 
The list of files that matches the search criteria must be provided as an argument to the
UNIX command specified.

## Compiling the code.
 gcc -Wall -o n_hw3.c n_hw3

## To Run
./n_hw3 -f jpg -E "tar cvf jpg.tar"
./n_hw3 -s 1024 -e "wc -l"
./n_hw3 -s 1024 -e "ls -l"
