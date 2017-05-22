// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------

void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);

    if ((which == SyscallException) && (type == SC_Halt)) {
	DEBUG('a', "Shutdown, initiated by user program.\n");
   	interrupt->Halt();
    } 
    else if((which == SyscallException) && (type == SC_Exit)){
        DEBUG('a', "program exit\n");
        // machine->clear();
        int NextPC = machine->ReadRegister(NextPCReg);
        machine->WriteRegister(PCReg, NextPC);
        
        currentThread->Finish();
    }
    //create
    else if ((which == SyscallException) && (type == SC_Create))
    {
        printf("System call create\n");
        int address = machine->ReadRegister(4);
        char name[10];
        int pos = 0;
        int data;
        while(1) {
            //ReadMem一次只能读1,2,4个字节
            machine->ReadMem(address + pos, 1, &data);
            if (data == 0)
            {
                name[pos++] = '\0';
                break;
            }
            name[pos++] = char(data);
        }

        fileSystem->Create(name, 128);
        machine->PC_advance();
    }
    //open
/* Open the Nachos file "name", and return an "OpenFileId" that can 
 * be used to read and write to the file.
 */
    else if ((which == SyscallException) && (type == SC_Open))
    {
        printf("System call open\n");
        int address = machine->ReadRegister(4);
        char name[10];
        int pos = 0;
        int data;
        while(1) {
            //ReadMem一次只能读1,2,4个字节
            machine->ReadMem(address + pos, 1, &data);
            if (data == 0)
            {
                name[pos++] = '\0';
                break;
            }
            name[pos++] = char(data);
        }
        OpenFile *openfile = fileSystem->Open(name);
        machine->WriteRegister(2, int(openfile));
        machine->PC_advance();
    }
    //close
    /* Close the file, we're done reading and writing to it. */
    else if ((which == SyscallException) && (type == SC_Close))
    {
        printf("system call Close\n");
        int fd = machine->ReadRegister(4);
        OpenFile *openfile = (OpenFile*)fd;
        delete openfile;
        machine->PC_advance();
    }
    //read
    /* Read "size" bytes from the open file into "buffer".  
 * Return the number of bytes actually read -- if the open file isn't
 * long enough, or if it is an I/O device, and there aren't enough 
 * characters to read, return whatever is available (for I/O devices, 
 * you should always wait until you can return at least one character).
 */
    else if ((which == SyscallException) && (type == SC_Read))
    {
        printf("System call Read\n");
        int base_position = machine->ReadRegister(4);
        int count = machine->ReadRegister(5);
        int fd = machine->ReadRegister(6);
        OpenFile *openfile = (OpenFile*)fd;
        char content[count];
        int result = openfile->Read(content, count);
        for (int i = 0; i < result; ++i)
        {
            machine->WriteMem(base_position + i, 1, int(content[i]));
        }
        machine->WriteRegister(2, result);
        machine->PC_advance();
        // printf("******\n");
    }
    // Write
    /* Write "size" bytes from "buffer" to the open file. */
    else if ((which == SyscallException) && (type == SC_Write))
    {
        printf("System call Write\n");
        int base_position = machine->ReadRegister(4);
        int count = machine->ReadRegister(5);
        int fd = machine->ReadRegister(6);
        OpenFile *openfile = (OpenFile*)fd;
        char content[count];
        int data;
        for (int i = 0; i < count; ++i)
        {
            machine->ReadMem(base_position + i, 1, &data);
            content[i] = char(data);
        }
        openfile->Write(content, count);
        machine->PC_advance();
    }
    else {
    printf("Unexpected user mode exception %d %d\n", which, type);
    ASSERT(FALSE);
    }
}

void Machine::PC_advance()
{
    WriteRegister(PrevPCReg, registers[PCReg]);
    WriteRegister(PCReg, registers[PCReg] + sizeof(int));
    WriteRegister(NextPCReg, registers[NextPCReg] + sizeof(int));
}