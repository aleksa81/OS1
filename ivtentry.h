#ifndef IVTENTRY_H_
#define IVTENTRY_H_

#include "Semaphore.h"
#include "PCB.h"
#include <dos.h>
#include "kernelev.h"

typedef void interrupt (*InterruptPTR)(...);

class IVTEntry{
public:
	static IVTEntry** entries; // samo zbog poziva stare rutine
	IVTEntry(unsigned char IVTno, InterruptPTR newRoutine);
	void callOldRoutine();
	~IVTEntry();
private:
	unsigned char IVTno;
	InterruptPTR oldRoutine;
};

#define PREPAREENTRY(IVTno,OldRoutineMask)\
\
void interrupt interruptNumber ## IVTno(...){\
asm cli;\
	if (OldRoutineMask==1) {\
		IVTEntry::entries[IVTno]->callOldRoutine();\
	}\
\
	if (KernelEv::events[IVTno]!=0) KernelEv::events[IVTno]->signal();\
\
asm sti;\
}\
IVTEntry ivtEntryNumber ## IVTno(IVTno,interruptNumber ## IVTno);\

#endif /* IVTENTRY_H_ */
