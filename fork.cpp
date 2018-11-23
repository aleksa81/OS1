#include "fork.h"
#include "PCB.h"
#include "thread.h"
#include <dos.h>

unsigned parent_sp;
unsigned parent_ss;
unsigned parent_bp;

unsigned temp_bp;

unsigned __far* _disp;
unsigned disp;
unsigned old_disp;
unsigned new_disp;

void interrupt stack_init(...){

	for (int i=0;i<PCB::running->stackSize;i++){
			PCB::currentForkChild->stack[i] = PCB::running->stack[i];
	}

	PCB::currentForkChild->processState = PCB::Active;

	asm{
		mov parent_sp, sp
		mov parent_ss, ss
		mov parent_bp, bp
	}

#ifndef BCC_BLOCK_IGNORE
	_disp = (unsigned __far*)MK_FP(parent_ss, parent_sp);
	disp = (unsigned)(_disp - PCB::running->stack);

	//cout << "REG disp:" << disp << " stacksize:"<< PCB::running->stackSize << endl;

	PCB::currentForkChild->sp = FP_OFF(PCB::currentForkChild->stack+disp);
	PCB::currentForkChild->ss = FP_SEG(PCB::currentForkChild->stack+disp);

	_disp = (unsigned __far*)MK_FP(parent_ss, parent_bp);
	old_disp = (unsigned)(_disp - PCB::running->stack);
	PCB::currentForkChild->bp = FP_OFF(PCB::currentForkChild->stack + old_disp);

	while(old_disp != PCB::running->stackSize-1){
		_disp = (unsigned __far*)MK_FP(parent_ss, PCB::running->stack[old_disp]);
		new_disp = (unsigned)(_disp - PCB::running->stack);
		PCB::currentForkChild->stack[old_disp] = FP_OFF(PCB::currentForkChild->stack + new_disp);
		old_disp = new_disp;
	}

#endif

}

