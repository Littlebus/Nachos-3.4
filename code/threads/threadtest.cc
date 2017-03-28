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
extern Semaphore *have,*rest,*mutex;
extern SynchList *item_list;
extern Barrier *barrier;
extern RWLock *buffer;
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
    
    for (num = 0; num < 4; num++) {
	   printf("*** thread %d looped %d times\n", currentThread->getTid(), num);
       for (int i = 0; i < 10; ++i)
       {
           interrupt->OneTick();
       }
       
    }
}
void
ticks()
{
   for (int i = 0; i < 10; ++i)
   {
       interrupt->OneTick();
   }
}
void producer_cond(int arg)
{
    char *a = new char;
    void* s=(void*) a;
    for (int i = 0; i < 10; ++i)
    {
        item_list->Append(s);
        printf("add an item from %s!\n", currentThread->getName());
        ticks();
    }
}
void consumer_cond(int arg)
{
    for (int i = 0; i < 5; ++i)
    {
        item_list->Remove();
        printf("%s get an item!\n", currentThread->getName());
        ticks();
    }
}
void consumer_sem(int arg)
{
    for (int i = 0; i < 5; ++i)
    {
        have->P();
        ticks();
        mutex->P();
        ticks();
        printf("%s consume a delicious cake!\n", currentThread->getName());
        ticks();
        mutex->V();
        ticks();
        rest->V();
        ticks();
    }
}
void producer_sem(int arg)
{
    for (int i = 0; i < 10; ++i)
    {
    rest->P();
    ticks();
    mutex->P();
    ticks();
    printf("%s produce a delicious cake!\n", currentThread->getName());
    ticks();
    mutex->V();
    ticks();
    have->V();
    ticks();
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

void bar(int arg)
{
    for (int i = 0; i < 10; ++i)
    {
        // printf("%d come to barrier\n", currentThread->getTid());
        barrier->barrier();
        currentThread->Yield();
    }
}
void reader(int arg)
{
    for (int i = 0; i < 4; ++i)
    {
        buffer->read();
        ticks();
    }
}
void writer(int arg)
{
    for (int i = 0; i < 10; ++i)
    {
        buffer->write(NULL);
        ticks();
    }
}
void
ThreadTest()
{    
    #ifdef CONDITION
    printf("CONDITION\n");
    item_list = new SynchList();
    Thread* c1 = Thread::createThread("consumer 1");
    Thread* c2 = Thread::createThread("consumer 2");
    Thread* p1 = Thread::createThread("producer 1");
    c1->Fork(consumer_cond, 1);
    c2->Fork(consumer_cond, 1);
    p1->Fork(producer_cond, 1);
    #endif
    #ifdef BARRIER
    printf("BARRIER\n");
    Thread *t[5];
    for (int i = 0; i < 5; ++i)
    {
        t[i] = Thread::createThread("ha");
    }
    barrier = new Barrier("barrier", 5);
    for (int i = 0; i < 5; ++i)
    {
        t[i]->Fork(bar,1);
    }
    #endif
    #ifdef RWLOCK
    printf("RWLOCK\n");
    buffer = new RWLock("RWLock");
    Thread *r[5];
    Thread *w;
    for (int i = 0; i < 5; ++i)
    {
        r[i] = Thread::createThread("reader");
    }
    w = Thread::createThread("writer");
    w->Fork(writer,1);
    for (int i = 0; i < 5; ++i)
    {
        r[i]->Fork(reader,1);
    }
    #endif

 //    #ifdef SYNCHRONOUS
 //    printf("haha\n");
 //    have = new Semaphore("have", 0);
 //    rest = new Semaphore("rest slot", 10);
 //    mutex = new Semaphore("mutex", 1);
 //    Thread* c1 = Thread::createThread("consumer 1");
 //    Thread* c2 = Thread::createThread("consumer 2");
 //    Thread* p1 = Thread::createThread("producer 1");
 //    c1->Fork(consumer_sem, 1);
 //    c2->Fork(consumer_sem, 1);
 //    p1->Fork(producer_sem, 1);
 //    #else
 //    switch (testnum) {
 //    case 1:
	// ThreadTest1();
	// break;
 //    default:
 //    for (int i = 0; i < testnum; ++i)
 //    {
 //            Thread *t = Thread::createThread("forked thread");
 //            if(t)
 //            {
 //                #ifdef PRIORITY
 //                t->setPriority(((i*100)/20+99)%11);
 //                printf("thread %d has priority of %d \n",t->getTid(),t->getPriority() );
 //                #endif
 //                // t->Fork(SimpleThread, 1);
 //            }
 //    }

 //    TS();
	// printf("No test specified.\n");
	// break;
 //    }
 //    #endif
}

