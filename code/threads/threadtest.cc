// threadtest.cc 
//	Simple test case for the threads assignment.
//
//	Create two threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield, 
//	to illustrate the inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

//测试文件，创建两个线程并且不断yield切换

#include "copyright.h"
#include "system.h"

// testnum is set in main.cc
int testnum = 1;

//----------------------------------------------------------------------
// SimpleThread
// 	Loop 5 times, yielding the CPU to another ready thread 
//	each iteration.
//
//	"which" is simply a number identifying the thread, for debugging
//	purposes.
//----------------------------------------------------------------------


//SimpleThread函数简单地yield出五次，并且打印信息
void
SimpleThread(int which)
{
    int num;
    
    for (num = 0; num < 5; num++) {
	printf("*** thread %d looped %d times\n", currentThread->getTid(), num);
        currentThread->Yield();
    }
}

//----------------------------------------------------------------------
// ThreadTest1
// 	Set up a ping-pong between two threads, by forking a thread 
//	to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------

//  modified by Oscar
void
TS()
{
    printf("---------------Call TS---------------\n");
    for (int i = 0; i < 128; ++i)
    {
        if (tidList[i])
        {
            printf("tid: %d, thread name: %s.\n", tid_pointer[i]->getTid(), tid_pointer[i]->getName());
        }
    }
    printf("-------------------------------------\n");
}

void
ThreadTest1()
{
    DEBUG('t', "Entering ThreadTest1");

    Thread *t = Thread::createThread("forked thread");

    t->Fork(SimpleThread, 1);
    SimpleThread(0);
}


//----------------------------------------------------------------------
// ThreadTest
// 	Invoke a test routine.
//----------------------------------------------------------------------

void
ThreadTest()
{
    switch (testnum) {
    case 1:
	ThreadTest1();
	break;
    default:
    for (int i = 0; i < testnum; ++i)
    {
            Thread *t = Thread::createThread("forked thread");
            if(t)
            t->Fork(SimpleThread, 1);
    }
	printf("No test specified.\n");
	break;
    }
}
