
#ifndef KERSEM_H_
#define KERSEM_H_

#include "tlist.h"
#include "PCB.h"
#include <conio.h>

class BlockedPCB{ // struktura koja cuva podatke o vremenu blokiranja
public:
	PCB* pcb;
	unsigned blockedTime;
	BlockedPCB(PCB* pcb, unsigned blockedTime){
		this->pcb = pcb;
		this->blockedTime = blockedTime;
	}
	~BlockedPCB(){
		//cprintf("~BlockedPCB()\r\n");
	}
	friend class KernelSem;
};

typedef TList<BlockedPCB> BlockedList;
typedef TList<BlockedPCB>::Node BlockedListNode;

class KernelSem{
	int val;
	BlockedList* blocked;
	BlockedList* blockedWithTime;
public:
	KernelSem(int init){
		val=init;
		blocked = new BlockedList();
		blockedWithTime = new BlockedList();
	}
	int getVal(){return val;}
	~KernelSem(){
		delete blocked;
		delete blockedWithTime;
	} // semafor sadrzi samo reference, svi node-ovi se brisu u metodama
	int wait(Time maxTimeToWait);
	void signal();
	void updateSem();
protected:
	friend class KernelSemList;
};

typedef TList<KernelSem> SemList;
typedef TList<KernelSem>::Node SemNode;

class KernelSemList{
public:
	static SemList *collection;
	static void updateCollection();
	static void add(KernelSem *newSem){
		collection->append(new SemNode(newSem));
	}
};

#endif /* KERSEM_H_ */
