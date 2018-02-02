/*******************************************************************************
*	target.c
*	Author: Joshua Newhouse
*
*	Tab width: 4
*
*	Purpose:
*
*	This implements the target.h interface.
*
*	Procedures:
*
*	target_init			initializes a Target_ts.
*	target_setCommand	sets the command field of the target.
*	target_getCommand	returns the command field from the target.
*******************************************************************************/
#include <stdint.h>

#include "target.h"

/*******************************************************************************
*	void target_init(Target_ts* target, int8_t adr, target_Type_te t, void* atr)
*
*	Description: Initializes a target object using the arguments passed.
*		The target type, t, and attributes, atr, are set to the passed values.
*
*		The target command bytes array is initialized with the passed address as
*		follows:
*			11111110 - 00AAAAAA - A0000000
*
*		The first byte is a constant, 0xFE; the second byte begins with 00
*		followed by the 6 MSB of the address; the third byte begins with the LSB
*		of the address followed by 0's in the command and data bit locations.
*
*	Parameters:
*
*	target	I/O	A pointer to the target object to initialize.
*	adr		I/P	The address of the target associated with the train set.
*	t		I/P	The type of the target.
*	atr		I/P	A pointer to any logical attributes that the target has.
*******************************************************************************/
void target_init(Target_ts* target, int8_t adr, target_Type_te t, void* atr) {
	target->bytes[0] = (int8_t)0xFE;
	target->bytes[1] = adr >> 1;
	target->bytes[2] = adr % 2 ? 0x80 : 0x0;
	target->type = t;
	target->attribute = atr;
}

/*******************************************************************************
*  void target_setCommand(Target_ts* target, target_CmdType_te cmd, uint8_t spd)
*
*	Description:	Sets the command passed as the second argument to the target
*					object passed as the first argument.  The third argument is
*					used to pass additional information if the particular
*					command requires it; i.e. speed value to set for the train.
*
*		The target command is set by using cmd as a mask and performing a
*		bitwise OR of the mask with the third byte in the target's command byte
*		array.
*
*		If the particular command passed as cmd requires additional data then
*		the third parameter is used to pass this data.  Currently only ABSPD and
*		RELSPD commands require data, which is the speed setting.
*
*		To set this the passed argument is bitwise ORed with the third byte in
*		the target's command byte array.
*
*	Parameters:
*
*	target	I/O	A pointer to the target object to set the command of.
*	cmd		I/P	The command mask to set the target command to.
*	spd		I/P	The variable data that must be set for some commands.
*******************************************************************************/
void target_setCommand(Target_ts* target, target_CmdType_te cmd, uint8_t spd) {
	target->bytes[2] &= CLEAR;
	if(cmd == SYSTEM_HALT)
		target->bytes[1] |= cmd;

	target->bytes[2] |= cmd;

	if(cmd == TRAIN_ABSSPD)
		target->bytes[2] |= spd > 0x1F ? 0x1F : spd;
	else if(cmd == TRAIN_RELSPD)
		target->bytes[2] |= spd > 0x0A ? 0x0A : spd;
}

/*******************************************************************************
*	int8_t* target_getCommand(Target_ts* target)
*
*	Description:	Returns the 3 byte array containing the current command set
*					on the target passed as the argument.
*
*	Parameters:
*	Target_ts*		The target object whose command will be returned.
*
*	Returns:
*	int8_t[3]		The 3 byte command.
*******************************************************************************/
int8_t* target_getCommand(Target_ts* target) {
	return target->bytes;
}
