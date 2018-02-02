/*******************************************************************************
*	train_main.c
*	Author: Joshua Newhouse
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

/* functions delcaration */
/* function for print out options of operation in user interface */
void printMenu(void);
/* function for setting up absolute speed */
void setSpeed(Target_ts*);

/* define MAX_SPD as maximun speed, 20 */
#define MAX_SPD 20

/* main function */
int main(void) {
	/* Connect to base controller */
	Base_ts base;
	base_init(&base, "COM1");

	/* Set up train as target with initial speed 0 */
	Target_ts train;
	uint8_t current_speed = 0;
	target_init(&train, 23, TRAIN, &current_speed);
        
	/* print out manu of operation options */
	printMenu();

	/* setting up run as true for continuously inquiring user input selection of operation */
	int run = 1;
	while(run) {
		/* read chosen option from user */
		char input = getch();
	
		switch(input) {
		case 0:
			break;
		case 'w':
			/* The train is sent a command to set its direction to forwards. */
			target_setCommand(&train, TRAIN_FORWARD, 0);
			base_sendData(&base, target_getCommand(&train));
			break;
		case 's':
			/* The train is sent a command to set its direction to backwards. */
			target_setCommand(&train, TRAIN_REVERSE, 0);
			base_sendData(&base, target_getCommand(&train));
			break;
		case '*':
			/* The user is first asked for a speed. Then, the train is sent a command to set its speed to what was given by the user. If the train reaches the maximum speed, the horn is honked. */
			setSpeed(&train);
			base_sendData(&base, target_getCommand(&train));
			/////////////////////////////////////////////////////////
                        // Horn when reaching the maximum speed
                        if(current_speed == MAX_SPD)
                	{
                    		target_setCommand(&train, TRAIN_HORN1,0);
                   		base_sendData(&base, target_getCommand(&train));
               		}
            		/////////////////////////////////////////////////////////
			printMenu();
			break;
		case '+':
			/* The train is sent a command to increase its speed by 1. */
		    if(current_speed < MAX_SPD) {
				target_setCommand(&train, TRAIN_RELSPD, 6);
				base_sendData(&base, target_getCommand(&train));
				current_speed++;
			}
			break;
		case '-':
			/* The train is sent a command to decrease its speed by 1. */
		    if(current_speed) {
				target_setCommand(&train, TRAIN_RELSPD, 4);
				base_sendData(&base, target_getCommand(&train));
				current_speed--;
			}
			break;
		case ' ':
			/* The train is sent a command to use its breaks. This does not set the speed back to 0. */
			target_setCommand(&train, TRAIN_BRAKE, 0);
			base_sendData(&base, target_getCommand(&train));
			break;
		case 'b':
			/* The train is sent a command to boost its speed. The train will return to its original speed once the boost has ended. */
			target_setCommand(&train, TRAIN_BOOST, 0);
			base_sendData(&base, target_getCommand(&train));
			break;
		case '1':
			/* The train is sent a command to use horn 1. */
			target_setCommand(&train, TRAIN_HORN1, 0);
			base_sendData(&base, target_getCommand(&train));
			break;
		case '2':
			/* The train is sent a command to use horn 2. */
			target_setCommand(&train, TRAIN_HORN2, 0);
			base_sendData(&base, target_getCommand(&train));
			break;
		case 'Q': case 'q':
			/* This sends a command to terminates the program. */
			target_setCommand(&train, SYSTEM_HALT, 0);
			base_sendData(&base, target_getCommand(&train));
			run = 0;
			break;
		case 'h':
			/* The train is sent a command to set its speed to 0. */
			target_setCommand(&train, TRAIN_ABSSPD, 0);
			base_sendData(&base, target_getCommand(&train));
			break;
		case 't':
			/* The train is sent a command to toggle its direction. This sets the speed to 0. */
			target_setCommand(&train, TRAIN_TOGGLE, 0);
			base_sendData(&base, target_getCommand(&train));
			current_speed = 0;
			break;
		case 'r':
			/* The train is sent a command to toggle its direction. The original speed is kept. */
			target_setCommand(&train, TRAIN_TOGGLE, 0);
			base_sendData(&base, target_getCommand(&train));
			target_setCommand(&train, TRAIN_ABSSPD, current_speed);
			base_sendData(&base, target_getCommand(&train));
			break;
		default:
			printf("Invalid Option: %d\nPress any key to continue.", input);
			getchar();
			printMenu();
		}
	}

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
*	Description:	Prompts the user for a train speed. Then, it sets the speed for the train.
*					on the target passed as the argument.
*
*	Parameters:
*	train		The target object that contains the train information needed for controllin the train.
*
*	
*******************************************************************************/

void setSpeed(Target_ts* train) {
	uint8_t spd;
	printf("Enter speed (valid range 0 to 20): \n");
	_cscanf("%u", &spd);
	getch();

	spd = spd > MAX_SPD ? MAX_SPD : spd;
	target_setCommand(train, TRAIN_ABSSPD, spd);
	*(uint8_t*)train->attribute = spd;
}
