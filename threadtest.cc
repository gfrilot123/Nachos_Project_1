// threadtest.cc
//	Simple test case for the threads assignment.
//
//	Create two threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield,
//	to illustratethe inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"


//----------------------------------------------------------------------
// SimpleThread
// 	Loop 5 times, yielding the CPU to another ready thread
//	each iteration.
//
//	"which" is simply a number identifying the thread, for debugging
//	purposes.
//----------------------------------------------------------------------

void
SimpleThread(int which)
{
    int num;

    for (num = 0; num < 5; num++) {
	      printf("*** thread %d looped %d times\n", which, num);

          currentThread->Yield();
    }

}

/*
InputTest
Is a void function that takes input from the user and determines the type of input.
For example an input of "5" would be an integer. While an input of "abc" is an array of characters.
The max input size is 1024 characters.
To check this a flag is set if certain parameters are met.
Is there is a number it is flagged as an integer.
If there is a negative sign at the from it is flagged negative.
If there is only one decimal point it is flagged as a decimal. If there is two it is flagged as a string.
All other cases default to a character string.
*/
void
InputTest(int a)
{
    printf("Enter an input with a max size of 1024: ");

    char userInput[1025];
    fgets(userInput, 1025, stdin);

    printf("Input is: %s\n", userInput);

    int count = 0;

    // Flags if input can be negative
    bool isNegative = FALSE;
    // Flags if input can be an integer
    bool isInt = FALSE;
    // Flags if input can be a decimal
    bool isDec = FALSE;
    // checks if a period char has bee used already
    bool decPeriod = FALSE;
    // Flags if input is a String
    bool isString = FALSE;

    if(userInput[0] == '-')
    {
      isNegative = TRUE;
      count++;
    }

    while('\0' != userInput[count])
    {
      //Checks if current character is a digit
      if(userInput[count] == '0' || userInput[count] == '1' || userInput[count] == '2' || userInput[count] == '3' || userInput[count] == '4' || userInput[count] == '5'
      || userInput[count] == '6' || userInput[count] == '7' || userInput[count] == '8' || userInput[count] == '9')
      {
        isInt = TRUE;
      }
      // Checks if there has been a digit used adn if there has already been a period
      else if(userInput[count] == '.' && isInt && decPeriod != TRUE)
      {

        decPeriod = TRUE;
        isDec = TRUE;

        // Handles the edge case of a sequence of number folllowed by a period
        if(userInput[count+1] == '\n')
        {
          isString = TRUE;
        }

      //Removes the new line from being counted and marking all other symbols as a string
      }else if(userInput[count] != '\n')
      {
        isString = TRUE;
      }

      count++;
    }

    //Prints to console what type of input the user typed depending on flags set
    if(isInt && isNegative != TRUE && isDec != TRUE && isString != TRUE)
    {
      printf("This is a integer\n");
    }else if(isInt && isNegative && isDec != TRUE && isString != TRUE)
    {
      printf("This is a negative integer\n");
    }else if(isInt && isNegative != TRUE && isDec && isString != TRUE)
    {
      printf("This is a decimal\n");
    }else if(isInt && isNegative && isDec && isString != TRUE)
    {
      printf("This is a negative decimal\n");
    }else
    {
      printf("This is a character string\n");
    }

    //Debugger code to see if flags set properly
    //printf("%d%d%d%d\n", isInt, isNegative, isDec, isString);

}

// Global variable to track the number of shouts per thread
int numShoutsGlobal;

/*
Shout
When called by a thread it randomly shouts based on the number of threads ands how many shouts.
Then it yields that thread in a buffer 2-5 times to allow other threads to shout.
*/
void Shout(int a)
{
  for(int x = 0; x < numShoutsGlobal; x++){

    //Randomly picks one of five shouts to use
    int shout = (Random()%5);
    switch(shout)
    {
      case 0:
      printf("%d Shout\n", a);
      break;
      case 1:
      printf("%d Let it all out\n", a);
      break;
      case 2:
      printf("%d These are the things I can do without\n", a);
      break;
      case 3:
      printf("%d I'm talking to you\n", a);
      break;
      case 4:
      printf("%d Come on\n", a);
      break;
    }

    // Randomly picks the amount of time the currentThread will buffer (between 2-5).
    int num = (Random()%5)+1;
    if(num < 2)
    {
      num = (Random()%5)+1;
    }

    for(int x = 0; x < num; x++)
    {
      currentThread->Yield();
    }

  }
}

/*
ThreadShout
Based on user input passed to it this function creates that many threads and set the global number of shouts.
Then Forks each thread to the shout function and passes its name as an int.
*/
void ThreadShout(int numThread, int numShout)
{
    Thread* t;
    char id[2];
    numShoutsGlobal = numShout;

    // Creates a new thread based on user input and Forks it to shout
    for(int x = 0; x < numThread; x++)
    {
        id[0] = (char)x;
        t = new Thread(id);
        t->Fork(Shout,x);
    }

}

/*
ThreadShoutInput
A function that is called by a thread and verifies user input to create a thread shouting match.
This verifies user input is between numbers 1-10,000.
This then call a function passing the users values and creates threads that shout that many times.
*/
void ThreadShoutInput(int a)
{
  // Ask for user inputs
  printf("Enter the numeber of threads you wish to make (1 to 10,000 max): ");

  // Stores user input and stores it in memory as a character array and an integer value
  // Gets the input for the number of Threads
  char userInput[1025];
  fgets(userInput, 1025, stdin);
  int count = 0;
  int userNumThread = atoi(userInput);

  // Checks if the user is input a valid input
  while('\0' != userInput[count] && '\n' != userInput[count])
  {
    if(userInput[count] == '0' || userInput[count] == '1' || userInput[count] == '2' || userInput[count] == '3' || userInput[count] == '4' || userInput[count] == '5'
    || userInput[count] == '6' || userInput[count] == '7' || userInput[count] == '8' || userInput[count] == '9')
    {
      count++;
    }else
    {
      printf("Not a valid input.\n");
      printf("Enter the numeber of threads you wish to make (1 to 10,000 max): ");
      fgets(userInput, 1025, stdin);
      userNumThread = atoi(userInput);
      count = 0;
    }

    if(userNumThread > 10000 || userNumThread < 1)
    {
      printf("Not a valid input.\n");
      printf("Enter the numeber of threads you wish to make (1 to 10,000 max): ");
      fgets(userInput, 1025, stdin);
      userNumThread = atoi(userInput);
      count = 0;
    }
  }

  // Gets the input for the number of shouts
    printf("Enter the number of shouts per thread (1 to 10,000): ");
    fgets(userInput, 1025, stdin);
    int userNumShout = atoi(userInput);
    count = 0;

      while('\0' != userInput[count] && '\n' != userInput[count])
      {
        if(userInput[count] == '0' || userInput[count] == '1' || userInput[count] == '2' || userInput[count] == '3' || userInput[count] == '4' || userInput[count] == '5'
        || userInput[count] == '6' || userInput[count] == '7' || userInput[count] == '8' || userInput[count] == '9')
        {
          count++;
        }else
        {
          printf("Not a valid input.\n");
          printf("Enter the number of shouts per thread (1 to 10,000): ");
          fgets(userInput, 1025, stdin);
          userNumShout = atoi(userInput);
          count = 0;
        }

        if(userNumShout > 10000 || userNumShout < 1)
        {
          printf("Not a valid input.\n");
          printf("Enter the number of shouts per thread (1 to 10,000): ");
          fgets(userInput, 1025, stdin);
          userNumShout = atoi(userInput);
          count = 0;
        }
      }
      
      ThreadShout(userNumThread,userNumShout);
}

void ThreadPick(int a)
{

}

//----------------------------------------------------------------------
// ThreadTest
// 	Invoke a test routine.
//----------------------------------------------------------------------

void
ThreadTest()
{
    DEBUG('t', "Entering ThreadTest");

    Thread* t = new Thread("forked thread");
    t->Fork(SimpleThread, 1);
    SimpleThread(0);



    if(myMenuOption == 1)
    {
      Thread* inputThread = new Thread("input thread");
      inputThread->Fork(InputTest, 1);
    }else if(myMenuOption == 2)
    {
      Thread* shoutInput = new Thread("shout input");
      shoutInput->Fork(ThreadShoutInput, 1);
    }

}
