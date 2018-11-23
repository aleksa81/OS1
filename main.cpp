#include <iostream.h>
#include <stdlib.h>
#include <dos.h>
#include <conio.h>
#include "SCHEDULE.H"
#include "timer.h"
#include "PCB.h"
#include "thread.h"
#include "daemon.h"
#include "tlist.h"
#include "user_t.h"
#include "kersem.h"

int main(int argc, char **argv){
	PCB::running = PCB::main;

	User::argc = argc;
	User::argv = argv;

	Daemon::Instance()->start(); // u start se stavljaju u kolekciju
	User::Instance()->start();

	setTimer();
	dispatch();
	restoreTimer();

	Daemon::deleteInstance();
	User::deleteInstance();
	delete PCB::main;

	//cout << "THAT'S ALL FOLKS! " << User::returnValue << endl;

	return User::returnValue;
}







