#ifndef TARGET_H
#define TARGET_H

#include <stdint.h>

typedef enum {
	TRAIN,
	SWITCH
} target_Type_te;

typedef struct {
	int8_t bytes[3];
	target_Type_te type;
	void* attribute;
} Target_ts;

typedef enum {
	TRAIN_ABSSPD	= 0x60,
	TRAIN_BOOST		= 0x04,
	TRAIN_BRAKE		= 0x07,
	TRAIN_FORWARD	= 0x00,
	TRAIN_HORN1		= 0x1C,
	TRAIN_HORN2		= 0x1F,
	TRAIN_RELSPD	= 0x40,
	TRAIN_REVERSE	= 0x03,
	TRAIN_TOGGLE    = 0x01,
	SYSTEM_HALT		= 0xFF,
	CLEAR			= 0x80
} target_CmdType_te;

void target_init(Target_ts*, int8_t, target_Type_te, void*);
void target_setCommand(Target_ts*, target_CmdType_te, uint8_t);
int8_t* target_getCommand(Target_ts*);

#endif
