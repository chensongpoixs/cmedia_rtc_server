/***********************************************************************************************
created: 		2022-08-24

author:			chensong

purpose:		log
************************************************************************************************/
#include <iostream>
#include <csignal> // sigsuspend()
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>



 

bool stoped = false;

void signalHandler(int signum)
{
	stoped = true;
	
}



int  main(int argc, char *argv[])
{
	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);


	 
	return 0;
}