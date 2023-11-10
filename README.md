# memory-cpu-simulation
This project aims to simulate the communication between a processor and a simple memory. This simulation used two processes, one for the CPU and one for the Memory.

List of files:

cpu.c

This is the parent file and should be executed in the command line
It implements the logic of the cpu, communicating with another 
process using the exec() function to execute the process on another
program (memory.c). This file implements all the instruction found in 
the project description

memory.c

This file implements the memory and it saves all the information from
a file.txt inside an array. it provides the infomation the cpu process needs
all the time an instructuon is requested. 

Makefile


There are two ways to run my program. I included the makefile I used to compile my programs.
For both cases extract the folder and follow one of the step below.

1)
- open the folder where all my file are included in cs1.utdallas.edu
- once you are inside the directory where all my files are stored type make
- you should be able to see 2 files compiling without errors or warnings
- if this does not happens proceed with the second way.
- if it works you can run my code the following: cpu <filename.txt> 20
- notice the number 20 is just a placeholder you can use all the numbers you want (making sure it is possible)

2) 
- if for some reason the Makefile is not working issue these commands:
- gcc -o cpu cpu.c
- gcc -0 memory memory.c
- from here refer to the 1 way second last point
