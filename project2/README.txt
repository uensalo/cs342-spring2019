***********
* CHANGES *
***********
- New function hash_increment in hash.h which ensures that an increment
operation is executed atomically.
- integer_count.c now invokes this function to avoid race conditions.



This is the readme file for the CS342 Spring 2019 project implementation.
We did this project in a group of two people. The authors are

    Name-Surname   Student ID   Section
1 - Unsal Ozturk   21601565     Section 1
2 - Faruk Oruc     21601844     Section 3

******************************************************************
* HOW TO COMPILE, RUN, AND TEST THE PROGRAMS AND OTHER NOTES     *
******************************************************************
- Invoke make from the terminal. This will compile the program. It will 
generate a thread-safe hash table library along with two programs: a test
program and the integer-count program.
- The hash table makes use of a linked-list, which is used to represent 
buckets for separate chaining.
- The hash table does not allow duplicate key value pairs.
- The hash table is thread safe.
- hash_destroy() should not be called before all threads using the 
hash table finish their execution. The application programmer has 
to enforce the behaviour.
- The values inserted into the hash table are not deallocated when the 
hash table is destroyed. It is up to the application programmer to 
deallocate the values.
