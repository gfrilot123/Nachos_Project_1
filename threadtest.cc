#include "copyright.h"
#include "system.h"
#include "thread.h"
#include "list.h"
#include <string>
#include <iostream>
using namespace std;
//*****************     AUTHOR: GERALD FRILOT  *********************************
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
int from[100];
int to=0;
int messageSentCount=0;
bool waiting=false;
int capacity;
char* arrays[4000];

Mailbox(int name)
{
myName=name;
int from[100];
int to=0;
int messageSentCount=0;
bool waiting=false;
int capacity;
char* arrays[4000];
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
cout<<"How many people entering the post office simulation? ";
cin>>people;
cout<<"How many messages can each mailbox hold? ";
cin>>mailboxes;
cout<<"How many messages are being delivered? ";
cin>>Messages;
cout<<"\n";
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

//**********MAIN METHOD

void
ThreadTest()
{
Thread *postofficeThread;
postofficeThread = new Thread("user input");
postofficeThread->Fork(postOffice_Simulation,1);
DebugInit("user input");
}
