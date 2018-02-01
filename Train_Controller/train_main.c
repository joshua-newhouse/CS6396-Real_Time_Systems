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
			target_setCommand(&train, TRAIN_FORWARD, 0);
			base_sendData(&base, target_getCommand(&train));
			break;
		case 's':
			target_setCommand(&train, TRAIN_REVERSE, 0);
			base_sendData(&base, target_getCommand(&train));
			break;
		case '*':
			setSpeed(&train);
			base_sendData(&base, target_getCommand(&train));
			printMenu();
			break;
		case '+':
		    if(current_speed < MAX_SPD) {
				target_setCommand(&train, TRAIN_RELSPD, 6);
				base_sendData(&base, target_getCommand(&train));
				current_speed++;
			}
			break;
		case '-':
		    if(current_speed) {
				target_setCommand(&train, TRAIN_RELSPD, 4);
				base_sendData(&base, target_getCommand(&train));
				current_speed--;
			}
			break;
		case ' ':
			target_setCommand(&train, TRAIN_BRAKE, 0);
			base_sendData(&base, target_getCommand(&train));
			break;
		case 'b':
			target_setCommand(&train, TRAIN_BOOST, 0);
			base_sendData(&base, target_getCommand(&train));
			break;
		case '1':
			target_setCommand(&train, TRAIN_HORN1, 0);
			base_sendData(&base, target_getCommand(&train));
			break;
		case '2':
			target_setCommand(&train, TRAIN_HORN2, 0);
			base_sendData(&base, target_getCommand(&train));
			break;
		case 'Q': case 'q':
			target_setCommand(&train, SYSTEM_HALT, 0);
			base_sendData(&base, target_getCommand(&train));
			run = 0;
			break;
		case 'h':
			target_setCommand(&train, TRAIN_ABSSPD, 0);
			base_sendData(&base, target_getCommand(&train));
			break;
		case 't':
			target_setCommand(&train, TRAIN_TOGGLE, 0);
			base_sendData(&base, target_getCommand(&train));
			current_speed = 0;
			break;
		case 'r':
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

void setSpeed(Target_ts* train) {
	uint8_t spd;
	printf("Enter speed (valid range 0 to 20): \n");
	_cscanf("%u", &spd);
	getch();

	spd = spd > MAX_SPD ? MAX_SPD : spd;
	target_setCommand(train, TRAIN_ABSSPD, spd);
	*(uint8_t*)train->attribute = spd;
}
