#include <string.h>
#include <ctype.h>
#include <synch.h>

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
