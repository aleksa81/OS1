#include "PCB.h"
#include "thread.h"
#include "daemon.h"
#include <conio.h>
#include "user_t.h"
#include "fork.h"
#include "semaphor.h"
#include <iostream.h>

extern const Thread* forkParent;

extern void interrupt stack_init(...);

unsigned parent_sp_;
unsigned parent_ss_;
unsigned parent_bp_;

unsigned __far* _disp_;
unsigned disp_;

void Thread::start(){
	myPCB->start();
}

ID Thread::getId(){
	return myPCB->id;
}

ID Thread::getRunningId(){
	return PCB::running->id;
}

Thread::~Thread(){
	asm cli;
	if (myPCB->processState!=PCB::Done && myPCB->id>2){
		cprintf("ERROR: waitToComplete() must be called in ~ of classes dirived from Thread . . .\r\n");
		Daemon::returnToMain();
	}
	delete myPCB;
	asm sti;
}

Thread::Thread(StackSize stackSize, Time timeSlice){
	asm cli;
	myPCB = new PCB(stackSize, timeSlice, this);
	asm sti;
}

void Thread::run(){
	for (int i =0; i < 15; ++i) {
		lock
		cprintf("inside[%d](p:%d) i = %d!\r\n", myPCB->id,myPCB->parent->id, i);
		unlock
		for (int k = 0; k<10000; ++k)
			for (int j = 0; j <30000; ++j);
	}
}

void Thread::waitToComplete(){
	asm cli;
	if (myPCB->parent!=0 && myPCB->processState!=PCB::Done){ // nema potrebe proveravati
		myPCB->parent->processState = PCB::Waiting;
		myPCB->parent->waitingThreadId = myPCB->id;
	}
	asm sti;
	dispatch();
}

Thread* Thread::getThreadById(ID id){ // vraca jedan ako nema niti sa trazenim id-em
	for (PCBNode* iter = Daemon::Instance()->collection->head;iter != 0; iter = iter->next){
		if (iter->data->getId()==id){
			return iter->data->myThread;
		}
	}
	return 0;
}

void Thread::exit(){
	asm cli;
	Daemon::numOfThreads--;
	PCB::running->processState = PCB::Done;
	if (PCB::running->parent!=0 &&
		PCB::running->parent->processState==PCB::Waiting &&
		PCB::running->parent->waitingThreadId == PCB::running->id) PCB::resume(PCB::running->parent);
	if (PCB::running->forkOrigin==1 && PCB::running->parent!=0){
		PCB::running->parent->forkChildrenSem->signal();
	}
	asm sti;
	dispatch();
}

Thread* Thread::clone() const{
	Thread*ret = new Thread();
	delete ret->myPCB;
	ret->myPCB = new PCB(PCB::running, ret);
	return ret;
}

ID Thread::fork(){
	asm cli;
	ID forkParentID = PCB::running->id;
	Thread *forkChild = PCB::running->myThread->clone();
	if (forkChild == 0){
		asm sti;
		return -1;
	}
	PCB::running->numOfForkChildren++;
	PCB::currentForkChild = forkChild->myPCB;

	asm cli;
	Scheduler::put(PCB::currentForkChild);
	Daemon::Instance()->put(PCB::currentForkChild);
	Daemon::numOfThreads++;

	stack_init();

	asm sti;
	if (PCB::running->id == forkParentID){
		return forkChild->getId();
	}
	else return 0;
}

void Thread::waitForForkChildren(){ //promeniti na semafor da se ne bi iteritalo kroz celu listu
	while (PCB::running->numOfForkChildren>0){
		PCB::running->forkChildrenSem->wait(0);
		PCB::running->numOfForkChildren--;
	}
}

int Thread::kill(int id){ // vraca 0 za neuspeh, 1 za uspeh
	asm cli;
	if (id < 3) { // id < 3 imaju nekorisnicke niti
		asm sti;
		return 0;
	}
	Thread * targetThread = Thread::getThreadById(id);
	if (targetThread==0) { // ako id nije validan
		asm sti;
		return 0;
	}
	Daemon::numOfThreads--;
	targetThread->myPCB->processState=PCB::Done;
	if (targetThread->myPCB->parent!=0 &&
		targetThread->myPCB->parent->processState==PCB::Waiting &&
		targetThread->myPCB->parent->waitingThreadId == targetThread->myPCB->id) PCB::resume(targetThread->myPCB->parent);
	if (targetThread->myPCB->forkOrigin==1 && targetThread->myPCB->parent!=0){
		targetThread->myPCB->parent->forkChildrenSem->signal();
	}
	return 1;
}









