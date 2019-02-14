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

//---------------------Begin changes by Chau Cao--------------------
#include <string.h>
#include <ctype.h>
#include <synch.h>
//Global Variables to be used with Project 2 task 4: PO Sim w/ Semaphores
//Struct declaration to define mail and post office
//Semaphores declared to protect critical resources
struct mail {
  char * message;
  int sender;
};

struct postOffice {
  int totalPeople;
  int sizeOfMailbox;
  int totalMessages;
  int messagesSent;
  int messagesRead;
  Semaphore * writer;
  //Semaphore * reader;
  int * mailCount;
  mail ** mailbox;
  const char * messageArray[13] = {
    "Letter 1",
    "Letter 2",
    "Letter 3",
    "Letter 4",
    "Letter 5",
    "Letter 6",
    "Letter 7",
    "Letter 8",
    "Letter 9",
    "Letter 10",
    "Letter 11",
    "Letter 12",
    "Letter 13"
  };
};
Semaphore ** mailboxSemaphore;
Semaphore ** freeSpaceSemaphore;
postOffice poss;
//------------------------End Changes by Chau Cao------------------------

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

//----------------------Code by Chau Cao---------------------------
//possValidation
//helper function to evaluate user inputType
//returns true if valid false otherwise
//Valid input: Integers in the range of 0 and 10000
bool possValidation(char *input) {
  int length = strlen(input);
  int check;
  int scope;
  if(length > 6) {
    return false;
  }
  else {
    for(int i = 0; i < length; i = i + 1) {
      if((check = isdigit(input[i])) == 0) {
        return false;
      }
    }
  }
  scope = atoi(input);
  if((scope == 0) || (scope > 10000)) {
    return false;
  }
  else {
    return true;
  }
}

//possInputEval
//driver function for input evaluation
//calls possValidation if the newline character is found in the buffer
//if input exceeds buffer or is invalid, the 0 is returned.
//valid input is returned in integer format
int possInputEval(char *input) {
  char * check;
  bool validInput = false;
  int result;
  if (((check = strchr(input, '\n')) != NULL) && (input[0] != '\n') && (input[10] != '\n')) {
    *check = '\0';
    validInput = possValidation(input);
  }
  if(check == NULL){
    scanf("%*[^\n]"); //2 lines of code to clear stdin input buffer. Stack overflow possibility?
    scanf("%*c");
  }
  if(validInput == false) {
    printf("The input is invalid\n");
    return 0;
  }
  else {
    result = atoi(input);
    return result;
  }
}

//possRead
//reads next message
//increments total messages read for debugging purposes
//decrements current mail counter
void possRead(int identifier) {
  poss.messagesRead++;
  printf("---Person %d reads the %dth message: %s sent by Person %d --- %d messages have been read\n", identifier, poss.mailCount[identifier]- 1, poss.mailbox[poss.mailCount[identifier]-1][identifier].message, poss.mailbox[poss.mailCount[identifier]-1][identifier].sender, poss.messagesRead);
  poss.mailCount[identifier]--;
}

//possCheckMail
//checks mailbox for mail
//if mail present, attempt to access critical resource, call read function and free resources
//yield after reading
//if maile is still present in the mailbox, continue reading until otherwise
void possCheckMail(int identifier) {
  printf("-Person %d checks - has %d letters in mailbox\n", identifier, poss.mailCount[identifier]);
  while(poss.mailCount[identifier] > 0) {
    mailboxSemaphore[identifier]->P();
    printf("--Person %d claims mailboxSemaphore[%d]\n", identifier, identifier);
    possRead(identifier);
    freeSpaceSemaphore[identifier]->V();
    printf("----Person %d releases and increases freeSpaceSemaphore[%d] by 1\n", identifier, identifier);
    mailboxSemaphore[identifier]->V();
    printf("-----Person %d releases mailboxSemaphore[%d]\n", identifier, identifier);
    printf("------Person %d yields after a read\n", identifier);
    currentThread->Yield();
  }
  return;
}

//possSendMail
//send mail to random person
void possSendMail(int identifier) {

  //initialize variables to randomly choose recipient and message
  int randomRecipient = 0;
  int randMessageIndex = 0;
  bool validRandom = false;
  mail * randomMessage = new mail;
  randomMessage->message = new char[13];
  //loop until valid random input acquired (aka not self)
  while(validRandom == false) {
    randomRecipient = (Random() % ((poss.totalPeople - 1) + 1));
    if(randomRecipient != identifier) {
      validRandom = true;
    }
  }
  randMessageIndex = (Random() % (12 - 0 + 1)) + 0;
  //insert values into message and attempt to mail
  strcpy(randomMessage->message, poss.messageArray[randMessageIndex]);
  randomMessage->sender = identifier;

  printf("--------Person %d is trying to send: %s : to Person %d \n", identifier, randomMessage->message, randomRecipient);
  printf("---------Person %d is trying to claim freeSpaceSemaphore[%d]\n", identifier, randomRecipient);
  freeSpaceSemaphore[randomRecipient]->P();
  printf("----------Person %d gets and decreases freeSpaceSemaphore[%d] by 1\n", identifier, randomRecipient);
  mailboxSemaphore[randomRecipient]->P();
  printf("-----------Person %d claims mailboxSemaphore[%d]\n", identifier, randomRecipient);

  //only one person is allowed to write at a time to not exceed the total messages alloted for simulation
  poss.writer->P();
  if(poss.messagesSent == poss.totalMessages){
    freeSpaceSemaphore[randomRecipient]->V(); //undo action of taking resource
    mailboxSemaphore[randomRecipient]->V();
  }
  else {
    poss.mailbox[poss.mailCount[randomRecipient]][randomRecipient] = *randomMessage;
    poss.mailCount[randomRecipient]++;
    printf("------------Person %d successfully sends: %s : to Person %d\n", identifier, randomMessage->message, randomRecipient);
    mailboxSemaphore[randomRecipient]->V();
    poss.messagesSent++;
    printf("-------------Person %d successfully updates the overall message count to %d\n", identifier, poss.messagesSent);
  }
  poss.writer->V();
  return;

  delete randomMessage;
}

void possPerson(int identifier) {
  int randomYieldNumber; //randomly assigned number of yields
  int yieldCount; //control variable to determine how many yield executions have occured
  bool yieldController; // loop control. Thread conties to yield while false

  //loop while messages can still be sent or there is mail to be read
  while(poss.messagesSent < poss.totalMessages || poss.mailCount[identifier] > 0){
    printf("Person %d enters the post office\n", identifier);
    possCheckMail(identifier);
    printf("-------Person %d's mailbox is empty\n", identifier);
    //will not attempt to send mail if total has been met
    if(poss.messagesSent < poss.totalMessages) {
      possSendMail(identifier);
    }
    printf("--------------Person %d leaves the post office\n", identifier);

    //if nothing to do, will not return
    if((poss.messagesSent == poss.totalMessages) && (poss.mailCount[identifier] == 0)) {
      break;
    }

    //else busy waiting 2-5 cycles to return to post office
    else {
      yieldCount = 0; //initialize controls to default values
      yieldController = false;
      randomYieldNumber =(Random() % (5 - 2 + 1)) + 2;
      //yields until random yield number is met then breaks out of loop
      while(yieldController == false) {
        printf("---------------Person %d yields the %dth cycle\n", identifier, yieldCount + 1);
        currentThread->Yield();
        yieldCount++;
        if(yieldCount == randomYieldNumber) {
          yieldController = true;
        }
      }
    }
  }
}

//postOfficeSimSem
//driver function of task 6
void postOfficeSimSem(int identifier) {

  //define buffers and controller to receive user input
  char * P = new char[7];
  char * S = new char[7];
  char * M = new char[7];
  int inputControl = 0;

  printf("-----Starting Post Office Simulation w/ Semaphores-----\n");

  //loop each input until valid input is received
  //once validity determined assign values to global
  while(inputControl == 0){
    printf("Enter the number of people: ");
    fgets(P, 7, stdin);
    inputControl = possInputEval(P);
  }
  poss.totalPeople = inputControl;

  inputControl = 0;
  while(inputControl == 0) {
    printf("Enter the capacity of the mailbox: ");
    fgets(S, 7, stdin);
    inputControl = possInputEval(S);
  }
  poss.sizeOfMailbox = inputControl;

  inputControl = 0;
  while(inputControl == 0) {
    printf("Enter total number of messages: ");
    fgets(M, 7, stdin);
    inputControl = possInputEval(M);
  }
  poss.totalMessages = inputControl;

  //exception handler for condition of only 1 person
  if(poss.totalPeople == 1)
  {
    printf("Person 0 enters the post office\n");
    printf("Person 0 attempts to send mail to themselves \n");
    printf("The staff politely ask him to leave and not come back \n");
  }
  //dynamically allocate and initialize relevant globals
  else {
    poss.mailCount = new int[poss.totalPeople];
    for(int x = 0; x < poss.totalPeople; x++) {
      poss.mailCount[x] = 0;
    }
    poss.mailbox = new mail*[poss.sizeOfMailbox];
    for(int y = 0; y < poss.sizeOfMailbox; y++) {
      poss.mailbox[y] = new mail[poss.totalPeople];
    }
    poss.messagesSent = 0;
    poss.messagesRead = 0;
    poss.writer = new Semaphore("writerSemaphore", 1);
    //poss.reader = new Semaphore("readerSemaphore", poss.totalPeople);
    freeSpaceSemaphore = new Semaphore *[poss.totalPeople];
    for(int z = 0; z < poss.totalPeople; z++) {
      freeSpaceSemaphore[z] = new Semaphore("freeSpaceSemaphore", poss.sizeOfMailbox);
    }
    mailboxSemaphore = new Semaphore *[poss.totalPeople];
    for(int n = 0; n < poss.totalPeople; n++) {
      mailboxSemaphore[n] = new Semaphore("mailboxSemaphore", 1);
    }

    for(int i = 0; i < poss.totalPeople; i++) {
      Thread * t = new Thread("POSS Thread");
      t->Fork(possPerson, i);
    }
  }
  delete P;
  delete S;
  delete M;
}
//---------------------- End Code by Chau Cao---------------------------



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
