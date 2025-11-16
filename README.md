# mutexsemaphore
for my assignment3 in OS class

This is a simulation program written in C on the Producer/Consumer problem, with the goal to simulate process synchronization
that includes pthreads, mutex, and semaphores.

Steps to run this program:
---------------------------
Assuming you have git cloned or downloaded this repository onto your system:
1. Compile the program
  -> in the current directory where the source code is located run "make" to compile the program (This uses the Makefile provided)
  -> if that is not possible or if you are running a different C compiler do: ex) "gcc -o proj3 proj3.c -lpthread"
2. Run the program
  -> run the executable (in this case proj3) in the current directory: "./proj3 [argument] [argument] [argument] [argument] [argument]"
  * The program executable expects 5 arguments when running the executable in this order:
    -> "the length of time the main thread is to sleep before terminating" (integer)
    -> "the maximum length of time the producer and consumer threads will sleep prior to producing or consuming a buffer_item" (integer)
    -> "the number of producer threads" (integer)
    -> "the number of consumer threads" (integer)
    -> "a yes or no to output the individual buffer snapshots for each item produced and consumed" (yes/no)
      // An example run of this program would be: " ./proj3 30 3 2 2 yes"
3. Clean up the binary via "make clean" or "rm proj3"

Thank you for trying this program!
- Austin
