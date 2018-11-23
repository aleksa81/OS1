#include <conio.h>
#include "PCB.h"
#include "timer.h"
#include "daemon.h"
#include <dos.h>

extern unsigned lockFlag;
extern unsigned dispatch_requested;

extern void interrupt reg_init(...);

const unsigned MAX_STACK_SIZE = 32000; // 32000K * 2B = 64KB

ID PCB::PCBcounter=0;
PCB* PCB::running = 0;
PCB* PCB::main = new PCB();
PCB* PCB::currentForkChild = 0;

PCB::PCB(StackSize stackSize, Time timeSlice, Thread *myThread){ // normalni konstruktor
	id = PCBcounter++;
	processState = Beginning;
	waitingThreadId = -1;
	forkOrigin = 0;

	if (id<3)parent = 0;
	else parent = PCB::running;

	this->timeSlice = timeSlice;

	if (stackSize > MAX_STACK_SIZE) this->stackSize = MAX_STACK_SIZE;
	else this->stackSize = stackSize;

	this->myThread = myThread;
	stack = 0; // za slucaj da se nit ne startuje, da destruktor ne bi pukao
}

PCB::PCB(Time timeSlice){ //za PCB::Main
	id = PCBcounter++;
	processState = System;
	this->timeSlice = timeSlice;

	forkOrigin = 0;
	parent = 0;
	stack = 0;
	myThread=0;
}

PCB::PCB(PCB *srcPCB, Thread *myThread){ // za clone
	this->id = PCBcounter++;
	this->processState = Beginning;
	this->forkOrigin = 1;
	this->waitingThreadId = -1;
	this->parent = srcPCB;
	this->timeSlice = srcPCB->timeSlice;
	this->stackSize = srcPCB->stackSize;
	this->myThread = myThread;

	this->stack = new unsigned[stackSize];
}

PCB::~PCB(){
	asm cli;
	delete stack;
	asm sti;
}

void PCB::start(){
	if (processState==PCB::Beginning){
		asm cli;
		Daemon::numOfThreads++;
		numOfForkChildren=0;
		forkChildrenSem= new Semaphore(0);
		this->stack = new unsigned[stackSize];

#ifndef BCC_BLOCK_IGNORE
		stack[stackSize - 1] = 0x200;
		stack[stackSize - 2] = FP_SEG(PCB::wrapper);
		stack[stackSize - 3] = FP_OFF(PCB::wrapper);
		this->sp = FP_OFF(stack + stackSize - 12);
		this->bp = FP_OFF(stack + stackSize - 12);
		this->ss = FP_SEG(stack + stackSize - 12);
#endif

		Scheduler::put(this);
		Daemon::Instance()->put(this);

		asm sti;
	}
}

void PCB::wrapper(){
	asm cli
	PCB::running->processState = Active;

	asm sti
    PCB::running->myThread->run();
	asm cli

	Daemon::numOfThreads--;
	PCB::running->processState=PCB::Done; //done
	if (running->parent!=0 &&
		running->parent->processState==Waiting &&
		running->parent->waitingThreadId == running->id) resume(running->parent);
	if (running->forkOrigin==1 && running->parent!=0){
		running->parent->forkChildrenSem->signal();
	}
	dispatch();
}

void PCB::resume(PCB *somePCB){
	somePCB->processState=Active;
	Scheduler::put(somePCB);
}
