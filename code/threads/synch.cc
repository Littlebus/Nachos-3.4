// synch.cc 
//	Routines for synchronizing threads.  Three kinds of
//	synchronization routines are defined here: semaphores, locks 
//   	and condition variables (the implementation of the last two
//	are left to the reader).
//
// Any implementation of a synchronization routine needs some
// primitive atomic operation.  We assume Nachos is running on
// a uniprocessor, and thus atomicity can be provided by
// turning off interrupts.  While interrupts are disabled, no
// context switch can occur, and thus the current thread is guaranteed
// to hold the CPU throughout, until interrupts are reenabled.
//
// Because some of these routines might be called with interrupts
// already disabled (Semaphore::V for one), instead of turning
// on interrupts at the end of the atomic operation, we always simply
// re-set the interrupt state back to its original value (whether
// that be disabled or enabled).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "synch.h"
#include "system.h"

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	Initialize a semaphore, so that it can be used for synchronization.
//
//	"debugName" is an arbitrary name, useful for debugging.
//	"initialValue" is the initial value of the semaphore.
//----------------------------------------------------------------------

Semaphore::Semaphore(char* debugName, int initialValue)
{
    name = debugName;
    value = initialValue;
    queue = new List;
}

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	De-allocate semaphore, when no longer needed.  Assume no one
//	is still waiting on the semaphore!
//----------------------------------------------------------------------

Semaphore::~Semaphore()
{
    delete queue;
}

//----------------------------------------------------------------------
// Semaphore::P
// 	Wait until semaphore value > 0, then decrement.  Checking the
//	value and decrementing must be done atomically, so we
//	need to disable interrupts before checking the value.
//
//	Note that Thread::Sleep assumes that interrupts are disabled
//	when it is called.
//----------------------------------------------------------------------

void
Semaphore::P()
{
    IntStatus oldLevel = interrupt->SetLevel(IntOff);	// disable interrupts
    
    while (value == 0) { 			// semaphore not available
	queue->Append((void *)currentThread);	// so go to sleep
	currentThread->Sleep();
    } 
    value--; 					// semaphore available, 
						// consume its value
    
    (void) interrupt->SetLevel(oldLevel);	// re-enable interrupts
}

//----------------------------------------------------------------------
// Semaphore::V
// 	Increment semaphore value, waking up a waiter if necessary.
//	As with P(), this operation must be atomic, so we need to disable
//	interrupts.  Scheduler::ReadyToRun() assumes that threads
//	are disabled when it is called.
//----------------------------------------------------------------------

void
Semaphore::V()
{
    Thread *thread;
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    thread = (Thread *)queue->Remove();
    if (thread != NULL)	   // make thread ready, consuming the V immediately
	scheduler->ReadyToRun(thread);
    value++;
    (void) interrupt->SetLevel(oldLevel);
}

// Dummy functions -- so we can compile our later assignments 
// Note -- without a correct implementation of Condition::Wait(), 
// the test case in the network assignment won't work!
Lock::Lock(char* debugName) 
{
    lock = new Semaphore(NULL, 1);
}
Lock::~Lock() 
{
    delete lock;
}
int
Lock::getOwner()
{
    return this->held_tid;
}
bool
Lock::isHeldByCurrentThread()
{
    return (currentThread->getTid() == held_tid);
}
void
Lock::Acquire() 
{
    lock->P();
    // printf("Acquire by %d\n", currentThread->getTid());
    held_tid = currentThread->getTid();
}
void Lock::Release() 
{
    // printf("Release by %d and %d hold it\n", currentThread->getTid(), held_tid);
    if (isHeldByCurrentThread())
        lock->V();
    else
    printf("wrong lock realease at %s\n", currentThread->getName());
}

Condition::Condition(){

}
Condition::Condition(char* debugName) 
{
    name = debugName;
    queue = new List;
}
Condition::~Condition() 
{
    delete queue;
}
void Condition::Wait(Lock* conditionLock) 
{ 
    // ASSERT(FALSE);
    IntStatus oldLevel = interrupt->SetLevel(IntOff);   //interrupt off
    conditionLock->Release();
    queue->Append(currentThread);
    currentThread->Sleep();
    conditionLock->Acquire();
    (void) interrupt->SetLevel(oldLevel);
}
void Condition::Signal(Lock* conditionLock) 
{
    IntStatus oldLevel = interrupt->SetLevel(IntOff);
    Thread* thread = (Thread *)queue->Remove();
    if (thread != NULL)
        scheduler->ReadyToRun(thread);
    (void) interrupt->SetLevel(oldLevel);
}
void Condition::Broadcast(Lock* conditionLock) 
{
    IntStatus oldLevel = interrupt->SetLevel(IntOff);
    Thread* thread;
    
    while (thread = (Thread *)queue->Remove())
    {
        scheduler->ReadyToRun(thread);
    }
    (void) interrupt->SetLevel(oldLevel);
}

Barrier::Barrier(char* debugName, int wait) 
{
    name = debugName;
    queue = new List;
    lock = new Lock("barrier lock");
    this->reach_number = this->barrier_number = wait;
}
void Barrier::barrier()
{
    
    lock->Acquire();
    reach_number--;
    if(reach_number == 0)   //reach the barrier
    {
        this->reach_number = this->barrier_number;
        Broadcast(lock);
        lock->Release();
        printf("%d break the barrier\n", currentThread->getTid());
    }
    else
    {
        printf("%d reach the barrier\n", currentThread->getTid());
        Wait(lock);
        lock->Release();
    }
}


RWLock::RWLock(char* debugName)
{
    write_lock = new Semaphore("write_lock",1);
    mutex = new Lock("mutex");
    reader_number = 0;
}
void* 
RWLock::read()
{
    mutex->Acquire();
    reader_number++;
    if(reader_number == 1)
        write_lock->P();
    mutex->Release();
    printf("reader %d is doing something\n", currentThread->getTid());
    mutex->Acquire();
    reader_number--;
    if(reader_number == 0)
        write_lock->V();
    mutex->Release();
    return NULL;
}
void 
RWLock::write(void* buffer)
{
    write_lock->P();
    printf("writer %d is doing something\n", currentThread->getTid());
    write_lock->V();
}