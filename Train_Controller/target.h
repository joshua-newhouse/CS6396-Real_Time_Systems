/*******************************************************************************
*	target.h
*	Author: Joshua Newhouse
*
*	Tab width: 4
*
*	Purpose:
*
*	This module defines a target class which encapsulates attributes and
*	procedures for the actors in the train-set system.  Potential targets
*	include:
*		train engine
*		track switch
*
*	Data Types:
*
*	target_Type_te		enumeration used to specify type of target.
*	Target_ts			structure used to model a particular taget object.
*	target_CmdType_te	enumeration used as symbolic constants for masks used to
						set commands.
*
*	Procedures:
*
*	target_init			initializes a Target_ts.
*	target_setCommand	sets the command field of the target.
*	target_getCommand	returns the command field from the target.
*******************************************************************************/
#ifndef TARGET_H
#define TARGET_H

#include <stdint.h>

typedef enum {
	TRAIN,
	SWITCH
} target_Type_te;

/**
* Target_ts:
*	Fields:
*		int8_t[3]	byte array for storing the the binary command to be sent to
*					this target.
*
*		target_Type_te	stores the type of this target.
*
*		void*		a pointer to any attributes associated with this target.
*/
typedef struct {
	int8_t bytes[3];
	target_Type_te type;
	void* attribute;
} Target_ts;

typedef enum {
	TRAIN_ABSSPD	= 0x60,		/* Absolute speed */
	TRAIN_BOOST		= 0x04,
	TRAIN_BRAKE		= 0x07,
	TRAIN_FORWARD	= 0x00,
	TRAIN_HORN1		= 0x1C,
	TRAIN_HORN2		= 0x1F,
	TRAIN_RELSPD	= 0x40,		/* Relative speed */
	TRAIN_REVERSE	= 0x03,
	TRAIN_TOGGLE    = 0x01,
	SYSTEM_HALT		= 0xFF,
	CLEAR			= 0x80
} target_CmdType_te;

/*******************************************************************************
*	target_init
*
*	Description: Initializes a target object to the arguments passed.
*
*	Parameters:
*
*	Target_ts*		A pointer to the target object to initialize.
*
*	int8_t			The address of the target associated with the train set.
*
*	target_Type_te	The type of the target.
*
*	void*			A pointer to any logical attributes that the target has.
*******************************************************************************/
void target_init(Target_ts*, int8_t, target_Type_te, void*);

/*******************************************************************************
*	target_setCommand
*
*	Description:	Sets the command passed as the second argument to the target
*					object passed as the first argument.  The third argument is
*					used to pass additional information if the particular
*					command requires it; i.e. speed value to set for the train.
*
*	Parameters:
*
*	Target_ts*			The target object whose command will be set.
*
*	target_CmdType_te	The command mask to use to set the target command.
*
*	uint8_t				Used for variable command data, such as speed value.
*						Currently only used for ABSSPD and RELSPD commands.
*						For ABSSPD valid values are in the range [0,1F].
*						For RELSPD valid values are in the range [0,9].
*******************************************************************************/
void target_setCommand(Target_ts*, target_CmdType_te, uint8_t);

/*******************************************************************************
*	target_getCommand
*
*	Description:	Returns the 3 byte array containing the current command set
*					on the target passed as the argument.
*
*	Parameters:
*
*	Target_ts*		The target object whose command will be returned.
*
*	Returns:
*
*	int8_t[3]		The 3 byte command.
*******************************************************************************/
int8_t* target_getCommand(Target_ts*);

#endif
