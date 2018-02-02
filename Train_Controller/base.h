/*******************************************************************************
*	base.h
*	Author: Joshua Newhouse
*
*	Tab width: 4
*
*	Purpose:
*
*	This module defines a base class which encapsulates attributes and
*	procedures for the base controller in the train-set system.
*
*	Data Types:
*
*	Base_ts		structure used to model the base controller object.
*
*	Procedures:
*
*	base_init		Initializes a Base_ts.
*	base_close		Closes a Base_ts.
*	base_sendData	Sends bytes to the Base_ts.
*******************************************************************************/
#ifndef BASE_H
#define BASE_H

#include <windows.h>
#include <stdint.h>

/**
* Base_ts:
*	Fields:
*		HANDLE			Pointer to the serial communication port
*
*		DCB				Parameters for the device being communicated with.
*	
*		COMMTIMEOUTS	Timeout settings for the serial communication port.
*/
typedef struct {
	HANDLE hSerial;
	DCB dcbSerialParams;
	COMMTIMEOUTS timeouts;
} Base_ts;

/*******************************************************************************
*	base_init
*
*	Description: Initializes a base object to the arguments passed.
*
*	Parameters:
*
*	Base_ts*	A pointer to the base object to initialize.
*
*	char*		The comm port of the serial line. Ex "COM1".
*
*	Returns:
*
*	int			0 if base was initialized successfully, 1 otherwise.
*******************************************************************************/
int base_init(Base_ts*, char*);

/*******************************************************************************
*	base_close
*
*	Description:	Closes the base object.
*
*	Parameters:
*
*	Base_ts*		The base object to close.
*
*******************************************************************************/
void base_close(Base_ts*);

/*******************************************************************************
*	base_sendData
*
*	Description:	Sends binary data, in the form of 3 bytes, to the base.
*
*	Parameters:
*
*	Base_ts*	The base object to send the data to.
*
*	int8_t[]	The array of bytes to send to the base.
*
*	Returns:
*
*	int			0 if data sent successfully, 1 otherwise.
*******************************************************************************/
int base_sendData(Base_ts*, int8_t[]);

#endif
