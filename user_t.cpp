#include "thread.h"
#include <conio.h>
#include "user_t.h"

User* User::instance = new User(defaultStackSize, User_timeSlice);

int User::argc = 0;
char ** User::argv = 0;
int User::returnValue = 0;
unsigned User::user_done = 0;

User* User::Instance(){
	return User::instance;
}

void User::setArgs(int argc, char **argv){
	User::argc = argc;
	User::argv = argv;
}

User::~User(){
	//cprintf("~User()\r\n");
} // ne brise se argv jer je on samo plitka kopija

void User::run(){
	returnValue = userMain(argc, argv);
	if (getId() < 3) user_done = 1;
}

