#include "ivtentry.h"
#include "kernelev.h"
#include <stdlib.h>
#include <dos.h>
#include <conio.h>

IVTEntry** IVTEntry::entries = (IVTEntry**) calloc(256, sizeof(IVTEntry*));

IVTEntry::IVTEntry(unsigned char IVTno, InterruptPTR newRoutine){
	asm cli;
		entries[IVTno] = this;
#ifndef BCC_BLOCK_IGNORE
		oldRoutine = getvect(IVTno);
		setvect(IVTno, newRoutine);
#endif
	asm sti;
}

IVTEntry::~IVTEntry(){
	asm cli;
	//cprintf("DELETING EVENT ENTRY!\r\n");
#ifndef BCC_BLOCK_IGNORE
		setvect(IVTno, oldRoutine);
#endif
	asm sti;
}

void IVTEntry::callOldRoutine(){
	oldRoutine();
}

