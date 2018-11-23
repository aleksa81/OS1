#ifndef USER_T_H_
#define USER_T_H_

#include "thread.h"

const unsigned User_timeSlice = 20;

extern int userMain(int, char**);

class User: public Thread{
public:
	~User();
	static User *Instance();
	static void deleteInstance(){delete instance;}
	static void setArgs(int, char**);
	static int returnValue;
	static unsigned user_done;
	static int argc;
	static char **argv;
private:
	User(StackSize stackSize, Time timeSlice):Thread(stackSize, timeSlice){}
	static User* instance;
	virtual void run();
};

#endif /* USER_T_H_ */
