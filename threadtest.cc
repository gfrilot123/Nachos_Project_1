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
#include "thread.h"
#include "list.h"
#include <string>
#include <iostream>
#include "synch.h"
#include <ctype.h>
using namespace std;

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

//---------------------Begin changes by Chau Cao--------------------
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
  bool * deadlockMonitor;
  int deadlockCount;
  Semaphore * deadlock;
  Semaphore * writer;
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
  mailboxSemaphore[identifier]->P();
  printf("--Person %d claims mailboxSemaphore[%d]\n", identifier, identifier);
  poss.messagesRead++;
  printf("---Person %d reads the %dth message: %s sent by Person %d --- %d messages have been read\n", identifier, poss.mailCount[identifier]- 1, poss.mailbox[poss.mailCount[identifier]-1][identifier].message, poss.mailbox[poss.mailCount[identifier]-1][identifier].sender, poss.messagesRead);
  poss.mailCount[identifier]--;
  freeSpaceSemaphore[identifier]->V();
  printf("----Person %d releases and increases freeSpaceSemaphore[%d] by 1\n", identifier, identifier);
  mailboxSemaphore[identifier]->V();
  printf("-----Person %d releases mailboxSemaphore[%d]\n", identifier, identifier);
}

//possCheckMail
//checks mailbox for mail
//if mail present, attempt to access critical resource, call read function and free resources
//yield after reading
//if maile is still present in the mailbox, continue reading until otherwise
void possCheckMail(int identifier) {
  printf("-Person %d checks - has %d letters in mailbox\n", identifier, poss.mailCount[identifier]);
  while(poss.mailCount[identifier] > 0) {
    possRead(identifier);
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
  poss.deadlock->P();
  poss.deadlockMonitor[identifier] = true;
  poss.deadlockCount++;
  poss.deadlock->V();
  if((poss.deadlockMonitor[randomRecipient] == true) && (poss.deadlockCount >= (poss.totalPeople / 2)))
  {
    printf("--------------!!Deadlock Prevention!!--------------\n");
    if(poss.mailCount[identifier] == poss.sizeOfMailbox) {
      possRead(identifier);
    }
    currentThread->Yield();
  }
  freeSpaceSemaphore[randomRecipient]->P();
  poss.deadlock->P();
  poss.deadlockMonitor[identifier] = false;
  poss.deadlockCount--;
  poss.deadlock->V();
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
    printf("Enter the number of people (between 1 and 10000): ");
    fgets(P, 7, stdin);
    inputControl = possInputEval(P);
  }
  poss.totalPeople = inputControl;

  inputControl = 0;
  while(inputControl == 0) {
    printf("Enter the capacity of the mailbox (between 1 and 10000): ");
    fgets(S, 7, stdin);
    inputControl = possInputEval(S);
  }
  poss.sizeOfMailbox = inputControl;

  inputControl = 0;
  while(inputControl == 0) {
    printf("Enter total number of messages (between 1 and 10000): ");
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
    //mailbox[][]
    poss.mailbox = new mail*[poss.sizeOfMailbox];
    for(int y = 0; y < poss.sizeOfMailbox; y++) {
      poss.mailbox[y] = new mail[poss.totalPeople];
    }
    poss.messagesSent = 0;
    poss.messagesRead = 0;
    poss.deadlockCount = 0;
    poss.deadlockMonitor = new bool[poss.totalPeople];
    poss.deadlockCount = 0;
    poss.deadlock = new Semaphore("deadlockSemaphore", 1);
    poss.writer = new Semaphore("writerSemaphore", 1);
    poss.mailCount = new int[poss.totalPeople];
    freeSpaceSemaphore = new Semaphore *[poss.totalPeople];
    mailboxSemaphore = new Semaphore *[poss.totalPeople];
    for (int x = 0; x < poss.totalPeople; x++) {
      poss.deadlockMonitor[x] = false;
      poss.mailCount[x] = 0;
      freeSpaceSemaphore[x] = new Semaphore("freeSpaceSemaphore", poss.sizeOfMailbox);
      mailboxSemaphore[x] = new Semaphore("mailboxSemaphore", 1);
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

//*****************     AUTHOR: GERALD FRILOT - BEGIN CODE *********************************
//*************************MESSAGE ARRAY
char* randomMsg[100]={"Are you feeling ok?","I am ready for the weekend!",
"Obsessive Writing Disorder!","Want to be pen pals?"};
//*************GLOBAL VARIABLES
int people,mailboxes,Messages;
int messageSent=0;
int goal;
bool wait=false;
Thread *globalThread= new  Thread("Array Fork");
Thread **mb;
//************MAILBOX STRUCT DEFINED HERE
typedef struct Mailbox
{
int myName;
int from[11000];
int to=0;
int messageSentCount=0;
bool waiting=false;
int capacity;
char* arrays[11000];

Mailbox(int name)
{
myName=name;
int from[11000];
int to=0;
int messageSentCount=0;
bool waiting=false;
int capacity;
char* arrays[11000];
}
}Mailbox;

//**********PROTOTYPE METHOD DECLARATION
void postOffice_Simulation(int any);
void constructMailBox(int numberOfMailboxes);
void enterPostOffice(int howmany);
void actionTime(int forkEntrance);
void readingTime(int messagesToRead);

//****************MAILBOX POINTER
struct Mailbox **mailboxPointer;

//*****************GET USER INPUT

void postOffice_Simulation(int any)
{
int validCounter=1;
do{

cout<<"How many people entering the post office simulation? ";
cin>>people;
cin.clear();
}while(validCounter<1 &&people>0 && people<10001);
cout<<"You've entered "<<people<<"\n";

if (people>0 && people <10001)
goto next;
else
{
	cout<<"Invalid entry! \n";
	return;
}

 
next:
do{
cout<<"How many messages can each mailbox hold? ";
cin>>mailboxes;
cin.clear();
}while(validCounter<1 &&mailboxes>0 && mailboxes<10001);
cout<<"You've entered "<<mailboxes <<"\n";

if (mailboxes>0 && mailboxes <10001)
goto nextOne;
else
{
	cout<<"Invalid entry! \n";
	return;
}


nextOne:

do{
cout<<"How many messages are being delivered? ";
cin>>Messages;
cin.clear();

}while(validCounter<1 && Messages >0 && Messages <10001);
cout<<"You've entered "<<Messages <<"\n";


if (Messages>0 && Messages <10001)
goto nextOver;
else
{
	cout<<"Invalid entry! \n";
	return;
}


nextOver:
goal=people*Messages;
constructMailBox(people);
enterPostOffice(people);
}

//*******CREATES AN ARRAY OF MAILBOX POINTERS
void constructMailBox(int numberOfMailboxes)
{
mailboxPointer=new struct Mailbox*[numberOfMailboxes];
int count=0;
for(int i = 0; i<numberOfMailboxes;i++)
{
  mailboxPointer[i]=new struct Mailbox::Mailbox(count);
  count++;
}

}

//******METHOD THAT CREATES AN ARRAY OF POINTERS
void enterPostOffice(int howmany)
{
mb= new Thread*[howmany];
char id[howmany];
for(int i =0;i<howmany;i++)
{
  id[i]=(char)i;
  mb[i]=new Thread((id));
}
//*******FORK THE NEXT METHOD FOR GUEST ENTRY
globalThread->Fork(actionTime,howmany);
}

//*******BEGIN ENTERING THE POST OFFICE
void actionTime(int forkEntrance)
{
for(int i =0; i< forkEntrance;i++)
  {cout<<"Guest "<<(int)mb[i]<<" has entered the post office."<<"\n";}
  mb[0]->Fork(readingTime,people);
}

//******MAIN POSTOFFICE SIMULATION METHOD
void readingTime(int messagesToRead)
{
int i;
while(messageSent < goal)
{
  for(i=0; i<people;i++)
  {
    cout<<"Guest "<<(int)mb[i]<<" is checking his mailbox. \n";
    if(Mailbox(i).arrays[i]==NULL)
    {
      cout<<"Guest mailbox "<<(int)mb[i]<<" is empty."<<"\n";
    }
    else if (!Mailbox(i).arrays[i]==NULL)
    {
    cout<<"Guest "<<(int)mb[i]<<" has mail !: "<<Mailbox(i).arrays[i];
    cout<<"from: "<<Mailbox(i).from[i]<<"\n";
    Mailbox(i).arrays[i]=NULL;

    while(Mailbox(i).arrays[i]!=NULL)
    {
      cout<<Mailbox(i).arrays[i++]<<"\n";
      Mailbox(i).arrays[i++]=NULL;
    }
  }
}

for (int j=0;j<people;j++)
  {
    int ID=((int)mb[j]);
    if(Mailbox(1).capacity==Messages)
    {goto wait;}
        else
        {
        int randomPerson=Random()%people;
        int randomMessage=Random()%4;
        if(j==randomPerson && randomPerson < people)
        {randomPerson++;}
        cout<<"Guest "<<(int)mb[j]<<" writes to Guest => "<<(int)mb[randomPerson]<<
        ": "<<randomMsg[randomMessage]<<"\n\n";
        Mailbox(randomPerson).arrays[j]=randomMsg[randomMessage];
        int ID=((int)mb[j]);
        Mailbox(randomPerson).from[randomPerson]=ID;

cout<<"*************"<<(int)mb[j]<<" is leaving the post office.*********** \n\n";
messageSent++;
cout<<"-------------------------------------------------------------------\n";
cout<<" Total messages to be delivered: "<<goal<<" \n";
cout<<" Messages delivered: "<<messageSent<<" \n";
cout<<"--------------------------------------------------------------------\n\n";
        }
cout<<"               "<<(int)mb[j]<<" is entering the postoffice.\n";
    }
wait:
      for(int a =0; a<7; a++)
      {currentThread->Yield();}
      }
  }
//*****************     AUTHOR: GERALD FRILOT - END CODE *********************************

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
      default:
      break;
    }

    // Randomly picks the amount of time the currentThread will buffer (between 2-5).
    int num = (Random()%5)+1;
    if(num < 2)
    {
      num = (Random()%5)+1;
    }

    for(int b = 0; b < num; b++)
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
      printf("Enter the number of threads you wish to make (1 to 10,000 max): ");
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
// Begin code by Robert Knott
// Global for number of meals to be eaten
int numMealGlobal;
// Global for number of philosophers to make
int numPhilosGlobal;
// Global to determine if all philosophers are present
int waitGlobal;
// Global to yeild the number of philosophers to sit
int sitGlobal;
bool seated = FALSE;
// Global bool array to track which chopsticks have been placed
bool* chopsticksGlobal;
// Global integer used to track the total number of meals eaten
int mealsEaten = 0;
// Global bool value used to determine if all meals have been eaten
bool finished = 0;
// Global integer value used to track how many philosophers are thinking
int isThinking = 0;

void PhiloEat(int a)
{
  printf("Philosopher %d has entered the dining hall.\n", a);
  while (waitGlobal < numPhilosGlobal){
    printf("-Pholosopher %d is waiting to sit.\n", a);
    waitGlobal++;
    currentThread->Yield();
  }
  printf("Philosopher %d sits.\n", a);
  while (sitGlobal < numPhilosGlobal)
  {
    sitGlobal++;
    currentThread->Yield();
  }

  //printf("seated = %d\n", seated);
  if(seated == FALSE)
  {
    printf("All Philosophers have been seated.\n");
    seated = TRUE;
  }

  int modPhilos = (a+1) % numPhilosGlobal;
  //printf("modPhilos = %d\n", modPhilos);
  //printf("chopsticks[modPhilos] = %d\n", chopsticksGlobal[modPhilos]);

  int eatCycle = (Random() % 3) + 2;
  int thinkCycle = (Random() % 3) + 2;

  //Randomly decide eat order
  while(mealsEaten < numMealGlobal)
  {
    if(!finished)
    {
      printf("Philosopher %d would like to eat.\n", a);
      printf("Philosopher %d looks to their left...\n", a);
      printf("---Philosopher %d's Left: %d, Right: %d\n", a, chopsticksGlobal[a], chopsticksGlobal[modPhilos]);
      if(chopsticksGlobal[a] == TRUE)
      {
        printf("Philosopher %d's left chopstick is available.\n", a);
        if(chopsticksGlobal[modPhilos] == TRUE)
        {
          printf("Philosopher %d's right chopstick is available.\n", a);
          chopsticksGlobal[a] = FALSE;
          printf("Philosopher %d grabs the left chopstick\n", a);
          chopsticksGlobal[modPhilos] = FALSE;
          printf("Philosopher %d grabs the right chopstick\n", a);
          printf("Philosopher %d can eat.\n", a);
          if(finished == TRUE)
          {
            printf("There are no more meals to eat.\n");
            break;
          }
          if(mealsEaten < numMealGlobal)
          {
            printf("Philosopher %d begins eating.\n", a);
            mealsEaten++;
            printf("Total Meals Served: %d/%d\n", mealsEaten, numMealGlobal);
            for(int i = 0; i < eatCycle; i++)
            {
              printf("Philosopher %d is eating...\n", a);
              currentThread->Yield();
            }
            printf("Philosopher %d has finished eating.\n", a);
            if(mealsEaten == numMealGlobal)
            {
              finished = TRUE;
            }
            printf("Philosopher %d begins to set the chopsticks down.\n", a);
            chopsticksGlobal[a] = TRUE;
            chopsticksGlobal[modPhilos] = TRUE;
            printf("Philosopher %d has returned the chopsticsk to the table.\n", a);
            printf("Philosopher %d has begun to think.\n", a);
	    isThinking++;
            for(int j = 0; j < thinkCycle; j++)
            {
              printf("Philosopher %d is thinking...\n", a);
              currentThread->Yield();
            }
	    isThinking--;
            printf("Philosopher %d has finished thinking and returns to the table.\n", a);
          }
          else
          {
            printf("However there is no more food to eat.\n");
            break;
          }
        }
        else
        {
          printf("Philosopher %d cannot see a right chopstick.\n", a);
          currentThread->Yield();
        }
      }
      else
      {
        printf("Philosopher %d cannot see a left chopstick.\n", a);
        currentThread->Yield();
      }
    }
  }
  printf("Philosopher %d prepares to leave.\n", a);
  if(isThinking == 0)
  {
    if(finished)
    {
      while(sitGlobal > 0)
      {
        sitGlobal--;
        currentThread->Yield();
      }

      if(seated == TRUE)
      {
        printf("All philosophers get up and leave the table.\n");
        seated = FALSE;
      }
    }
  }
}
// End code made by Robert Knott

// Begin code made by Joseph Aucoin

// Track current philosophers left neighbor
int philoLeftGlobal;
// Track current philosophers right neighbor
int philoRightGlobal;
// Tracks philosiphor current state 0-Thinking 1-Hungry 2-Eating
int* statePhiloGlobal;
// Semaphore to block multiple philosophers from picking up chopsticks at the same time
Semaphore* mutexGlobal = new Semaphore("lock",1);
// Semaphore array for the chopsticks
Semaphore** semChopsticksGlobal;

void PhiloSem(int a)
{
  printf("Philosopher %d has entered the dining hall.\n", a);
  while (waitGlobal < numPhilosGlobal){
    printf("-Pholosopher %d is waiting to sit.\n", a);
    waitGlobal++;
    currentThread->Yield();
  }
  printf("Philosopher %d sits.\n", a);
  while (sitGlobal < numPhilosGlobal)
  {
    sitGlobal++;
    currentThread->Yield();
  }

  //printf("seated = %d\n", seated);
  if(seated == FALSE)
  {
    printf("All Philosophers have been seated.\n");
    seated = TRUE;
  }

  int modPhilos = (a+1) % numPhilosGlobal;
  //printf("modPhilos = %d\n", modPhilos);
  //printf("chopsticks[modPhilos] = %d\n", chopsticksGlobal[modPhilos]);

  int eatCycle = (Random() % 3) + 2;
  int thinkCycle = (Random() % 3) + 2;

  //Randomly decide eat order
  while(mealsEaten < numMealGlobal)
  {
    if(!finished)
    {
      //printf("---Philosopher %d's Left: %d, Right: %d\n", a, chopsticksGlobal[a], chopsticksGlobal[modPhilos]);
      if(finished == TRUE)
      {
        printf("There are no more meals to eat.\n");
        break;
      }
      /*
      else
      {
        printf("However there is no more food to eat.\n");
        break;
      }
      */
      printf("Philosopher %d would like to eat.\n", a);
      printf("Philosopher %d ask the waiter if they can pick up chopsticks.\n", a);
      mutexGlobal->P();
      if(mealsEaten < numMealGlobal)
      {
        // Wait for the left chopstick
        semChopsticksGlobal[a]->P();
        printf("Philosopher %d looks to their left...\n", a);
        printf("Philosopher %d's left chopstick is available.\n", a);
        // Wait for the right chopstick
        semChopsticksGlobal[modPhilos]->P();
        printf("Philosopher %d looks to their right...\n", a);
        printf("Philosopher %d's right chopstick is available.\n", a);
        printf("Philosopher %d grabs the left chopstick\n", a);
        printf("Philosopher %d grabs the right chopstick\n", a);
        printf("Philosopher %d begins eating.\n", a);

        // Signal the waiter to let the next philosopher pick up chopsticks
        mutexGlobal->V();
        // Increment the global number of meals available
        mealsEaten++;
        printf("Total Meals Served: %d/%d\n", mealsEaten, numMealGlobal);

        // Busy waiting loop for the philosopher to eat
        for(int i = 0; i < eatCycle; i++)
        {
          printf("Philosopher %d is eating...\n", a);
          currentThread->Yield();
        }
        printf("Philosopher %d has finished eating.\n", a);

        // Flags if there is no more food
        if(mealsEaten == numMealGlobal)
        {
          finished = TRUE;
        }

      }
      printf("Philosopher %d begins to set the chopsticks down.\n", a);
      // Signal the left chopstick is available
      semChopsticksGlobal[a]->V();
      // Signal if right shopstick is available
      semChopsticksGlobal[modPhilos]->V();

      // Increment the number of philosophers that are thinking
      isThinking++;
      // Loop to let the philosopher think
      for(int j = 0; j < thinkCycle; j++)
      {
        printf("Philosopher %d is thinking...\n", a);
        currentThread->Yield();
      }
      // Decrement the number of philosophers that are done thinking
      isThinking--;
      printf("Philosopher %d has finished thinking and returns to the table.\n", a);
    }
  }

// Loop to wait for all philosopher to leave
  printf("Philosopher %d prepares to leave.\n", a);
  if(isThinking == 0)
  {
    if(finished)
    {
      while(sitGlobal > 0)
      {
        sitGlobal--;
        currentThread->Yield();
      }

      if(seated == TRUE)
      {
        printf("All philosophers get up and leave the table.\n");
        seated = FALSE;
      }
    }
  }
}
// End code made by Joseph Aucoin

void ThreadPhilo(int numPhilo, int numMeal, int a)
{
    // Creates an array of chopsticks for both busy and semaphore versions
    Thread* t;
    char id[2];
    numMealGlobal = numMeal;
    numPhilosGlobal = numPhilo;
    bool chopsticks[numPhilo] = { TRUE };
    int philoState[numPhilo];
    Semaphore* semChopsticks[numPhilo] = {};
    for(int r = 0; r < numPhilo; r++)
    {
      id[0] = (char)r;
      chopsticks[r] = TRUE;
      semChopsticks[r] = new Semaphore(id, 1);
      philoState[r] = 0;
    }
    chopsticksGlobal = chopsticks;
    semChopsticksGlobal = semChopsticks;
    for(int r = 0; r < numPhilo; r++)
    {
      printf("%d\n",semChopsticksGlobal[(r+1)%numPhilo]);
    }

    // Creates a new thread for each philosopher then forks to the busy waiting or semaphore function
    if(a == 0)
    {
      for(int x = 0; x < numPhilo; x++)
      {
          id[0] = (char)x;
          t = new Thread(id);
          t->Fork(PhiloEat,x);
      }
    }else
    {
      for(int x = 0; x < numPhilo; x++)
      {
          id[0] = (char)x;
          t = new Thread(id);
          t->Fork(PhiloSem,x);
      }
    }

}

void PhilosophersInput(int a)
{
  printf("a = %d\n", a);
  char userInput[1025];
  int numPhilos = 0;
  int numMeals = 0;

  printf("Enter the number of philosophers (2 to 10000): ");
  fgets(userInput, 1025, stdin);
  numPhilos = atoi(userInput);
  // Begin code added by Joseph Aucoin
  int count = 0;
  while('\0' != userInput[count] && '\n' != userInput[count])
  {
    if(userInput[count] == '0' || userInput[count] == '1' || userInput[count] == '2' || userInput[count] == '3' || userInput[count] == '4' || userInput[count] == '5'
    || userInput[count] == '6' || userInput[count] == '7' || userInput[count] == '8' ||
    userInput[count] == '9')
    {
      count++;
    }else
    {
      printf("Invalid input\n");
      printf("Enter the number of philosophers (2 to 10000): ");
      fgets(userInput, 1025, stdin);
      numPhilos = atoi(userInput);
      count = 0;
    }

    if(numPhilos > 10000 || numPhilos < 2)
    {
      printf("Invalid input\n");
      printf("Enter the number of philosophers (2 to 10000): ");
      fgets(userInput, 1025, stdin);
      numPhilos = atoi(userInput);
      count = 0;
    }
  }
  // End code added by Joseph Aucoin
  // Number of meals is arbitrairy and only exit to stop the user from asking an unreasonable amount
  printf("\nEnter the total number of meals that must be eaten (Max 10000): ");
  fgets(userInput, 1025, stdin);
  numMeals = atoi(userInput);
  // Begin code added by Joseph Aucoin
  count = 0;
  while('\0' != userInput[count] && '\n' != userInput[count])
  {
    if(userInput[count] == '0' || userInput[count] == '1' || userInput[count] == '2' || userInput[count] == '3' || userInput[count] == '4' || userInput[count] == '5'
    || userInput[count] == '6' || userInput[count] == '7' || userInput[count] == '8' ||
    userInput[count] == '9')
    {
      count++;
    }else
    {
      printf("Invalid input\n");
      printf("Enter the number of meals (Max 10000): ");
      fgets(userInput, 1025, stdin);
      numMeals = atoi(userInput);
      count = 0;
    }

    if(numMeals > 10000 || numMeals < 1)
    {
      printf("Invalid input\n");
      printf("Enter the number of meals (Max 10000): ");
      fgets(userInput, 1025, stdin);
      numMeals = atoi(userInput);
      count = 0;
    }
  }
  // End code added by Joseph Aucoin

  printf("\nnumPhilos = %d, numMeals = %d\n", numPhilos, numMeals);

  ThreadPhilo(numPhilos,numMeals, a);

}

//----------------------------------------------------------------------
// ThreadTest
// 	Invoke a test routine.
//----------------------------------------------------------------------

void
ThreadTest()
{
    /*DEBUG('t', "Entering ThreadTest");

    Thread* t = new Thread("forked thread");
    t->Fork(SimpleThread, 1);
    SimpleThread(0);*/

    int num;
    if(myMenuOption == 1)
    {
      Thread* inputThread = new Thread("input thread");
      inputThread->Fork(InputTest, 1);
    }else if(myMenuOption == 2)
    {
      Thread* shoutInput = new Thread("shout input");
      shoutInput->Fork(ThreadShoutInput, 1);
    }else if(myMenuOption == 3 || myMenuOption == 4)
    {
      if(myMenuOption == 3)
      {
        num = 0;
      }else
        num = 1;
      Thread* philoBusy = new Thread("busy philo");
      philoBusy->Fork(PhilosophersInput, num);
    }else if(myMenuOption == 5)
    {
      Thread *postofficeThread;
      postofficeThread = new Thread("user input");
      postofficeThread->Fork(postOffice_Simulation,1);
      DebugInit("user input");
    }else if(myMenuOption == 6)
    {
      Thread* postOfficeSemThread;
      postOfficeSemThread = new Thread("Post Sem");
      postOfficeSemThread->Fork(postOfficeSimSem,1);
    }


}
