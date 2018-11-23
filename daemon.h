#ifndef DAEMON_H_
#define DAEMON_H_

#include "thread.h"
#include "PCB.h"
#include "tlist.h"

const unsigned Daemon_timeSlice = 10;

typedef TList<PCB> PCBList;
typedef TList<PCB>::Node PCBNode;

class Daemon: public Thread{
public:
	static int numOfThreads;
	static void returnToMain();
	static Daemon* Instance();
	static PCBList* Collection();
	static void deleteInstance(){delete instance;}
	void put (PCB*);
	int hasActiveChildren(ID myID);
	~Daemon();
protected:
	friend class PCB;
	friend class List;
	friend class Thread;
private:
	virtual void run();
	Daemon(StackSize stackSize, Time timeSlice);
	PCBList* collection;
	static Daemon* instance;
};

#endif /* DAEMON_H_ */
