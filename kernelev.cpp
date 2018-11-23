#include "kernelev.h"
#include "semaphor.h"

extern void dispatch();

KernelEv::KernelEv(unsigned char IVTno){
	events[IVTno] = this;
	myPCB = PCB::running;
	val=0;
}

void KernelEv::signal(){
	asm cli;
	if (val++<0){
		if (myPCB->processState!=PCB::Done){ // zbog Thread::kill()
			myPCB->processState=PCB::Active;
			Scheduler::put(myPCB);
		}
		val=0;
	}
	else val=1;
	asm sti;
	dispatch();
}

void KernelEv::wait(){
	asm cli;
	if (PCB::running == myPCB){
		if (--val<0){
			myPCB->processState=PCB::Blocked;
			asm sti;
			dispatch();
		}
	}
}




