#include "thread.h"
#include "semaphor.h"
#include "conio.h"

Semaphore * killSem = new Semaphore(0);

class KillTest : public Thread{
public:
	KillTest(){}
	~KillTest(){waitToComplete();}
	void run(){
		cprintf("PRE WAIT-A!\n\r");
		killSem->wait(0);
		cprintf("ZIV JE, UMRO NIJE!\n\r");
	}
};
