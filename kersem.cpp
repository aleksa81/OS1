
#include "kersem.h"
#include "PCB.h"
#include <conio.h>

SemList * KernelSemList::collection = new SemList();

int KernelSem::wait(Time maxTimeToWait){
	asm cli
	if (--val < 0){
		PCB::running->processState = PCB::Blocked;
		//cprintf("Thread %d is blocked!\r\n", PCB::running->getId());
		if (maxTimeToWait==0){ // ako nema vremensko ogranicenje
			blocked->append(new BlockedListNode(new BlockedPCB(PCB::running, maxTimeToWait)));
		}
		else{ // ako ima vremensko ogranicenje
			if (blockedWithTime->isEmpty()){ // ako je prvi dodat, semafor se stavlja u kolekciju
				KernelSemList::add(this);
			}
			blockedWithTime->append(new BlockedListNode(new BlockedPCB(PCB::running, maxTimeToWait)));
		}
		dispatch();
	}
	asm sti;
	return PCB::running->signalUnblockedMe;
}

void KernelSem::signal(){
	asm cli;
	if (val++<0){
		if (!blocked->isEmpty()){
			BlockedPCB * temp = blocked->getFirst();
			PCB* tempPCB = temp->pcb;
			tempPCB->signalUnblockedMe = 1;
			if (tempPCB->processState!=PCB::Done) Scheduler::put(tempPCB); // Thread::kill()
			tempPCB->processState = PCB::Active;
			delete temp;
		}
	}
	asm sti;
}

void KernelSem::updateSem(){
	asm cli;
	BlockedListNode* iter = blockedWithTime->head;
	while (iter!=0){
		if (--iter->data->blockedTime==0){
			val++;
			iter->data->pcb->signalUnblockedMe = 0;
			if (iter->data->pcb->processState != PCB::Done) Scheduler::put(iter->data->pcb); // Thread::kill()
			iter->data->pcb->processState = PCB::Active;
			iter = blockedWithTime->remove(iter);
		}
		else iter = iter->next;
	}
	asm sti;
}

void KernelSemList::updateCollection(){
	SemNode* iter = collection->head;
	while (iter!=0){
		iter->data->updateSem();
		if (iter->data->blockedWithTime->isEmpty()){
			iter = collection->removeNode(iter);
		}
		else iter = iter->next;
	}
}



