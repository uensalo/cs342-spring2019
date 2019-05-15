- Unsal Ozturk, 21601565, CS342 Project 1, Section 1, Project submission.
- Contents: Code(bilshell.c, producer.c, consumer.c), Makefile(Makefile and make_2),
	    MATLAB script for plots (exp_results.m), report (report.pdf), README.txt.
- Information about the program:
	- Bilshell is a very simple shell program which supports piping 
	with only two subprograms.
	- The user may invoke "exit" to exit the program.
	- The shell program outputs the current path and a "<<" symbol
	when it's awaiting input.
	- The program may be invoked in the following way:
		bilshell N input_filename
		bilshell N
		(1st one for batch mode, 2nd one for interactive mode)	
- Since producer and consumer programs can not take N as a parameter
(only M) according to the project description, I had to define them as 
preprocessor directives. If one wishes to read/write N characters
at a time, one has to modifiy the preprocessor directives in producer.c 
and consumer.c and recompile the program. By default, they read 1 byte 
at a time, which should not cause any SIGPIPES, because there is no 
race condition to read/write from/to the buffers. To change this, open 
producer.c and consumer.c and change the preprocessor directives, and 
recompile. Make sure that the #define N .. is the same for both programs
and the bilshell read parameter.

- By default, the program compiles into usr/bin. This requires admin
access, and thus a password.

- Compiling without moving executables into the bin: use make_2 to
compile instead of the usual Makefile.

- To compile, just invoke make and provide admin password. If admin password
is not known/can't be provided, use make_2 instead of Makefile.
