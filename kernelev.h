
#ifndef KERNELEV_H_
#define KERNELEV_H_

#include "PCB.h"

class KernelEv{
public:
	static KernelEv** events; //inic u event.cpp
	int val;
	KernelEv(unsigned char IVTno);
	void signal();
	void wait();
private:
	PCB* myPCB;
};

#endif /* KERNELEV_H_ */
