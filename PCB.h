#ifndef PCB_H_
#define PCB_H_

#include <iostream.h>
#include <dos.h>
#include "SCHEDULE.H"
#include "thread.h"
#include "semaphor.h"

#define lock asm cli;
#define unlock asm sti;

const unsigned HostMain_timeSlice = 0; // ne userMain nego ona nit koja se prva stavlja kao running

class PCB{
private:
	static ID PCBcounter;
	static void wrapper();
	static void resume(PCB *somePCB);
	ID id;
	PCB *parent;
	unsigned char signalUnblockedMe; // za povratnu vrednost za wait

	unsigned numOfForkChildren;
	Semaphore * forkChildrenSem;

	Time timeSlice;
	PCB(Time timeSlice = HostMain_timeSlice);
	PCB(StackSize stackSize, Time timeSlice, Thread *myThread);
	PCB(PCB *srcPCB, Thread *myThread);
public:
	unsigned *stack;
	unsigned sp;
	unsigned ss;
	unsigned bp;

	Thread *myThread;
	unsigned waitingThreadId;
	unsigned forkOrigin;
	StackSize stackSize;

	static PCB *running;
	static PCB *main;
	static PCB *currentForkChild;

	enum ProcessState{System, Blocked, Done, Active, Waiting, Beginning};
	ProcessState processState;

	~PCB();
	void start();
	int isAtomic()const{return timeSlice==0;}
	int isActive()const{return processState == Active;}
	Time getTimeSlice()const{return timeSlice;}
	ID getId()const{return id;}

protected:
	friend class Thread;
	friend class Daemon;
	friend class List;
	friend class KernelSem;
};

#endif /* PCB_H_ */
