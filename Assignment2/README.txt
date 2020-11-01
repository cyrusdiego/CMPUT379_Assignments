Assignment 2
CMPUT 379 Fall 2020

NAME: Cyrus Diego
CCID: cdiego

HOW-TO-RUN:
- Run "make" in the root directory then run "make run nthreads <id> <path_to_test_file" 
  to run the executable
    - nthreads is the number of threads to create 
    - <id> is an optional parameter to specify an ID for the log file
    - <path_to_test_file indicates piping an input file to the program 

- Alternatively, the executable can be found in the bin/ directory and the program can be ran 
  via "prodcon nthreads <id> <path_to_test_file"
    - NOTE: if a log file already exists with a given ID, running the program will append to the file

- The output file (prodcon.<id.>log) will be in the directory in which the program is executed
    - If "make run ..." is used, the log files will be in the root directory 
    - If the executable is ran directly in bin/ then it will be 
    created in the bin/ directory (careful, running "make clean" will delete the bin/ directory)

- Makefile commands:
    - "make" will compile *without* any compiler flags 
    - "make clean" will remove obj/ and bin/ directories 
    - "make debug" will compile with a -g flag 
    - "make optimize" will compile with a -O flag
    - "make optimize3" will compile with a -O3 flag
    - "make run nthreads <id> <path_to_test_file" will run the executable (./bin/prodcon) 
       with the passed arguments
    - "make help" will show available commands 


FILES INCLUDED:
- src/cqueue.cpp
- src/printer.cpp
- src/prodcon.cpp
- src/tands.cpp

- include/constants.hpp
- include/cqueue.hpp
- include/printer.hpp
- include/tands.hpp

- Makefile
- README.txt

GENERATED DIRECTORIES: 
- obj/ houses .o files and .d (dependency) files
- bin/ houses executable

MAKEFILE CREDITS:
https://github.com/KRMisha/Makefile/blob/master/Makefile
https://stackoverflow.com/questions/2394609/makefile-header-dependencies
https://stackoverflow.com/questions/2214575/passing-arguments-to-make-run
