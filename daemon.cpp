#include "daemon.h"
#include "PCB.h"
#include <conio.h>
#include "user_t.h"


Daemon* Daemon::instance = new Daemon(1024, Daemon_timeSlice);

int Daemon::numOfThreads = 0;

Daemon::Daemon(StackSize stackSize, Time timeSlice):Thread(stackSize, timeSlice){
	collection = new PCBList();
}

Daemon::~Daemon(){
	//cprintf("~Daemon()\r\n");
}

Daemon* Daemon::Instance(){
	return instance;
}

PCBList* Daemon::Collection(){
	return instance->collection;
}

void Daemon::put(PCB* newPCB){
	collection->append(new PCBNode(newPCB));
}

int Daemon::hasActiveChildren(int myID){ //PCBmain ima parent 0
	for (PCBNode *iterator = collection->head;iterator!=0;iterator = iterator->next){
		if (iterator->data->parent!=0){
			if (iterator->data->parent->id==myID) return 1;
		}
	}
	return 0;
}

void Daemon::returnToMain(){
	delete Daemon::Instance()->collection;
	Daemon::Instance()->collection = 0;
	PCB::resume(PCB::main);
	dispatch();
}

void Daemon::run(){
	while(1){
		asm cli; //*LOCK*

		if (numOfThreads==1){
			returnToMain();
		}
		asm sti; //*UNLOCK*
		dispatch();
	}
}


