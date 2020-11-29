Assignment 3
CMPUT 379 Fall 2020

NAME: Cyrus Diego
CCID: cdiego

INSTRUCTIONS:

** Compiling **
- run "make" to compile server and client programs and their corresponding man pages (.pdf) 
    - man pages (client.pdf and server.pdf) will be produced in the root directory
    - server and client executables will be in the generated bin/ directory 
- run "make clean" to delete the obj/ and bin/ directories AND the man pages 
- run "make debug" to compile with the -g flag 

** Running **
- client:
    - run ./bin/client port_number ip_addres [<file_name]
    - port_number is the port the server is listening to
    - ip_address is the address of the machine hosting the server
    - file_name is optional, specifies the path to an input file
- server:
    - run ./bin/server port_number
    - port_number is the port the server is listening to

** Notes ** 
- server must be running first then clients can connect

FILES INCLUDED:

- README.txt
- Makefile
- /src/client/main.cpp
- /src/client/client.cpp
- /src/server/main.cpp
- /src/server/server.cpp
- /src/logger.cpp
- /src/tands.cpp
- /src/helper.cpp
- /include/client.cpp
- /include/logger.cpp
- /include/tands.cpp
- /include/helper.cpp

GENERATED DIRECTORIES: 
- obj/ houses .o files and .d (dependency) files
- bin/ houses executable

MAKEFILE CREDITS:
https://github.com/KRMisha/Makefile/blob/master/Makefile
https://stackoverflow.com/questions/2394609/makefile-header-dependencies
https://stackoverflow.com/questions/2214575/passing-arguments-to-make-run

SERVER CODE CREDITS:
- I referenced the following link on how to use poll()
https://www.ibm.com/support/knowledgecenter/ssw_ibm_i_71/rzab6/poll.htm

