#include <stdint.h>

#include "target.h"

void target_init(Target_ts* target, int8_t adr, target_Type_te t, void* atr) {
	target->bytes[0] = (int8_t)0xFE;
	target->bytes[1] = adr >> 1;
	target->bytes[2] = adr % 2 ? 0x80 : 0x0;
	target->type = t;
	target->attribute = atr;
}

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

int8_t* target_getCommand(Target_ts* target) {
	return target->bytes;
}
