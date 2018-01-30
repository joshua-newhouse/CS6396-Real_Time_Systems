#ifndef BASE_H
#define BASE_H

#include <windows.h>
#include <stdint.h>

typedef struct {
	HANDLE hSerial;
	DCB dcbSerialParams;
	COMMTIMEOUTS timeouts;
} Base_ts;

int base_init(Base_ts*, char*);
void base_close(Base_ts*);
int base_sendData(Base_ts*, int8_t[]);

#endif
