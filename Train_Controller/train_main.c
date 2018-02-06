/*******************************************************************************
*	train_main.c
*	Author: Joshua Newhouse
*	Modified: Guy Maor, Ben Weng
*
*	Tab width: 4
*
*	Purpose:
*
*	This implements the interface for controlling the train.
*
*	Procedures:
*
*	main				contains the beginning of the code.
*	printMenu			displays a menu of key controls.
*	setSpeed			prompts the user for setting a train speed.
*******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <conio.h>

#include "base.h"
#include "target.h"

/* functions declaration */
/* function for print out options of operation in user interface */
void printMenu(void);
/* function for setting up absolute speed */
void setSpeed(void);

/* This function executes the specified command using the data if necessary */
void executeCommand(target_CmdType_te, uint8_t);

#define THRESHOLD 5
/* This function honks the horn continuously if the speed is above THRESHOLD */
DWORD WINAPI hornThread(void*);

/* define MAX_SPD as maximum speed, 20 */
#define MAX_SPD 20

/* Reserve space for the base, train, and a lock for them */
static Base_ts base;
static Target_ts train;
static CRITICAL_SECTION critical_section;

/* main function */
int main(void) {
	/* Connect to base controller */
	base_init(&base, "COM1");

	/* Set up train as target with initial speed 0 */
	uint8_t current_speed = 0;
	target_init(&train, 23, TRAIN, &current_speed);

	/* Initialize the lock to be used for sending commands 1 at a time */
	InitializeCriticalSection(&critical_section);

	/* Create a thread to run the hornThread function.  This requires its own
		thread since it must continual send horn commands to the train even
		while the main thread is blocked waiting for user input. 			  */
	CreateThread(NULL, 0, hornThread, NULL, 0, NULL);

	/* print out menu of operation options */
	printMenu();

	/* setting up run as true for continuously inquiring user input selection of
		operation */
	int run = 1;
	while(run) {
		/* read chosen option from user */
		char input = getch();

		switch(input) {
		case 'w':
			/* The train is sent a command to set its direction to forwards. */
			executeCommand(TRAIN_FORWARD, 0);
			break;
		case 's':
			/* The train is sent a command to set its direction to backwards. */
			executeCommand(TRAIN_REVERSE, 0);
			break;
		case '*':
			/* The user is first asked for a speed. Then, the train is sent a
				command to set its speed to what was given by the user. */
			setSpeed();
			printMenu();
			break;
		case '+':
			/* The train is sent a command to increase its speed by 1. */
		    if(current_speed < MAX_SPD) {
				executeCommand(TRAIN_RELSPD, 6);
				current_speed++;
			}
			break;
		case '-':
			/* The train is sent a command to decrease its speed by 1. */
		    if(current_speed) {
				executeCommand(TRAIN_RELSPD, 4);
				current_speed--;
			}
			break;
		case ' ':
			/* The train is sent a command to use its breaks. This does not set
				the speed back to 0. */
			executeCommand(TRAIN_BRAKE, 0);
			break;
		case 'b':
			/* The train is sent a command to boost its speed. The train will
				return to its original speed once the boost has ended. */
			executeCommand(TRAIN_BOOST, 0);
			break;
		case '1':
			/* The train is sent a command to use horn 1. */
			executeCommand(TRAIN_HORN1, 0);
			break;
		case '2':
			/* The train is sent a command to use horn 2. */
			executeCommand(TRAIN_HORN2, 0);
			break;
		case 'Q': case 'q':
			/* This sends a command to terminates the program. */
			executeCommand(SYSTEM_HALT, 0);
			run = 0;
			break;
		case 'h':
			/* The train is sent a command to set its speed to 0. */
			executeCommand(TRAIN_ABSSPD, 0);
			current_speed = 0;
			break;
		case 't':
			/* The train is sent a command to toggle its direction. This sets
				the speed to 0. */
			executeCommand(TRAIN_TOGGLE, 0);
			current_speed = 0;
			break;
		case 'r':
			/* The train is sent a command to toggle its direction. The original
				speed is kept. */
			EnterCriticalSection(&critical_section);
			target_setCommand(&train, TRAIN_TOGGLE, 0);
			base_sendData(&base, target_getCommand(&train));
			target_setCommand(&train, TRAIN_ABSSPD, current_speed);
			base_sendData(&base, target_getCommand(&train));
			LeaveCriticalSection(&critical_section);
			break;
		default:
			printf("Invalid Option: %d\nPress any key to continue.", input);
			getchar();
			printMenu();
		}
	}

	DeleteCriticalSection(&critical_section);
	base_close(&base);
	exit(EXIT_SUCCESS);
}

/*******************************************************************************
*	void printMenu(void)
*
*	Description:	Displays a menu for the user to know what keys to press.
*
*
*******************************************************************************/
void printMenu(void) {
	system("cls");
	printf("Train Controller; select action:\n"
		"w:\tForward\n"
		"s:\tReverse\n"
		"*:\tSet speed\n"
		"+:\tIncrease speed\n"
		"-:\tDecrease speed\n"
		"Space:\tBrake\n"
		"b:\tBoost\n"
		"1:\tHorn type 1\n"
		"2:\tHorn type 2\n"
		"t:\tToggle Direction\n"
		"r:\tReverse Speed\n"
		"h:\tHalt\n"
		"q:\tQuit\n"
	);
}

/*******************************************************************************
*	void setSpeed(Target_ts* train)
*
*	Description:	Prompts the user for a train speed. Then, it sets the speed
*					for the train on the target passed as the argument.
*
*	Parameters:
*	train		The target object that contains the train information needed for
*				controlling the train.
*
*******************************************************************************/
void setSpeed(void) {
	uint8_t spd;
	printf("Enter speed (valid range 0 to 20): \n");
	_cscanf("%u", &spd);
	getch();

	spd = spd > MAX_SPD ? MAX_SPD : spd;
	*(uint8_t*)train.attribute = spd;
	executeCommand(TRAIN_ABSSPD, spd);
}

/*******************************************************************************
*	DWORD WINAPI hornThread(void* no_arg)
*
*	Description:	Runs in a loop until SYSTEM_HALT command has been set in
*					train.  Checks the train's current speed is greater than
*					THRESHOLD and if it is it commands the train to honk.  It
*					then sleeps for 500 ms.
*
*	Parameters:
*	None
*
*******************************************************************************/
DWORD WINAPI hornThread(void* no_arg) {
	while(train.bytes[1] != -1 && train.bytes[2] != -1) {
		if(*(uint8_t*)train.attribute >= THRESHOLD)
			executeCommand(TRAIN_HORN1, 0);
		Sleep(500);
	}

	return 1;
}

/*******************************************************************************
*	void executeCommand(target_CmdType_te t, uint8_t d)
*
*	Description:	Executes the command passed as the first argument and if
*					necessary the data passed in the second argument.  It does
*					this by locking the critical section and then issuing the
*					command to the train and then sending it to the base.  It
*					finally unlocks the critical section.
*
*	Parameters:
*	t	The command to issue to the train.
*	d	The data that is necessary for certain commands such as TRAIN_ABSSPD and
*		TRAIN_RELSPD.
*
*******************************************************************************/
void executeCommand(target_CmdType_te t, uint8_t d) {
	EnterCriticalSection(&critical_section);
	target_setCommand(&train, t, d);
	base_sendData(&base, target_getCommand(&train));
	LeaveCriticalSection(&critical_section);
}
