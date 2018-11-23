#include "event.h"
#include "kernelev.h"
#include <stdlib.h>
#include <stdio.h>

KernelEv** KernelEv::events = (KernelEv**) calloc(256, sizeof(KernelEv*));

Event::Event(IVTNo ivtNo){
	myImpl = new KernelEv(ivtNo);
}

Event::~Event(){delete myImpl;}

void Event::wait(){
	myImpl->wait();
}

void Event::signal(){
	myImpl->signal();
}


