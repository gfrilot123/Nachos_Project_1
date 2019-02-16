# Nachos_Project_1
TASK 3 POST OFFICE BUSY WAITING IMPLEMENTED BY GERALD FRILOT

    Begin:               
1. Explain the method you used to resolve the deadlock problem. Why did you choose this particular method?

I decided to use a goto label that redirects a tread upon encountering the deadlock issue. When a mailbox is full, 
the thread enters a label and cycles for 3+  iterations and Yields() before checking the mailbox again. 
A boolean flag named wait is enabled and until that flag goes back to true,meaning mail was successfully delivered, 
everyone skips placing mail in the  inbox waiting because its a global flag. I used this method because it keeps 
all threads in motion without having to check something that can be alerted to everyone as a whole. 

                   
2. In your own words, explain how you implemented each task. Did you encounter any bugs? If so, how did you fix them? 
If you failed to complete any tasks, list them here and briefly explain why.

I avoided using 2D arrays and implemented a mailbox struct. The mailbox struct allowed assignment of each thread utilizing 
the thread name as its memory address. I forked a method that begin asking for user input and validated that it was all 
correct before moving forward. I gave the user one shot at entering the correct information for each input. If input validation 
received bad input. The program returns. User has to try again.
Once input was valid, global variables were initialized. I then created 2 arrays. 1 array was a pointer to a pointer 
of mailbox objects and the second array was a pointer to every thread provided by user input.  The five methods I constructed 
consisted of:

Post Office Simulation: Collects and validates user input. This method invokes Construct Mailbox and Enter Post Office.
Construct Mailbox: Creates an array of mailbox pointers.
Enter Post Office : Creates an array of threads and forks a function titled action time.
Action Time: Allows everyone to enter the post office at the beginning. Then the first thread that enters Forks Reading time.
Reading Time: This is where the action begins. A series of nested loops are allowing the sequence of events to take place 
and the main outer loop is checking to make sure everyone has submitted an equal amount of messages. If a mailbox is full, 
the boolean flag for the person entering the goto label waits is enabled to true. Every thread randomly goes through writing 
except to the guy waiting until he resets the flag. 

I underwent a few seg faults and had to bump the stacksize up to 10024 and the faults were gone. 
    
3. What sort of data structures and algorithms did you use for each task?
I used a mailbox struct and arrays.

End Task 3 Description by Gerald Frilot
