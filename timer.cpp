#include "timer.h"
#include "PCB.h"
#include <dos.h>
#include "kersem.h"

unsigned dispatch_requested=0;
unsigned oldTimerSEG=0;
unsigned oldTimerOFF=0;
unsigned clock=20;

unsigned sp_temp;
unsigned ss_temp;
unsigned bp_temp;

void tick(){
	asm int 60h;
}

void dispatch(){
	asm cli
	dispatch_requested = 1;
	asm int 8h
	asm sti
}

void interrupt timer(...){
	if (!(PCB::running->isAtomic() && PCB::running->isActive() && !dispatch_requested)){
		if (!dispatch_requested) clock--;
		if (clock==0 || dispatch_requested){
			asm{
				mov sp_temp, sp
				mov ss_temp, ss
				mov bp_temp, bp
			}

			PCB::running->sp = sp_temp;
			PCB::running->ss = ss_temp;
			PCB::running->bp = bp_temp;

			if (PCB::running->processState!=PCB::Done &&
				PCB::running->processState!=PCB::Waiting &&
				PCB::running->processState!=PCB::System &&
				PCB::running->processState!=PCB::Blocked) Scheduler::put(PCB::running);
			PCB::running = Scheduler::get();
			while (PCB::running->processState==PCB::Done) PCB::running = Scheduler::get(); // zbog Thread::kill()

			sp_temp = PCB::running->sp;
			ss_temp = PCB::running->ss;
			bp_temp = PCB::running->bp;

			clock=PCB::running->getTimeSlice();

			asm{
				mov sp, sp_temp
				mov ss, ss_temp
				mov bp, bp_temp
			}
		}
	}
	if (!dispatch_requested) {
		asm int 60h;
		KernelSemList::updateCollection();
	}

	dispatch_requested=0;
}

void setTimer(){
	asm{
			cli
			push es
			push ax

			mov ax,0
			mov es,ax

			mov ax, word ptr es:0022h
			mov word ptr oldTimerSEG, ax
			mov ax, word ptr es:0020h
			mov word ptr oldTimerOFF, ax

			mov word ptr es:0022h, seg timer
			mov word ptr es:0020h, offset timer

			mov ax, oldTimerSEG
			mov word ptr es:0182h, ax
			mov ax, oldTimerOFF
			mov word ptr es:0180h, ax

			pop ax
			pop es

			sti
		}
}

void restoreTimer(){
	asm{
			cli
			push es
			push ax

			mov ax,0
			mov es, ax

			mov ax, word ptr oldTimerSEG
			mov word ptr es:0022h, ax
			mov ax, word ptr oldTimerOFF
			mov word ptr es:0020h, ax

			pop ax
			pop es
			sti
		}
}


