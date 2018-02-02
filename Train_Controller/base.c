/*******************************************************************************
*	base.c
*	Author: Joshua Newhouse
*
*	Tab width: 4
*
*	Purpose:
*
*	This implements the base.h interface.
*
*	Procedures:
*
*	base_init		Initializes a Base_ts.
*	base_close		Closes a Base_ts.
*	base_sendData	Sends bytes to the Base_ts.
*******************************************************************************/
#include <windows.h>
#include <stdio.h>

#include "base.h"

/*******************************************************************************
*	int base_init(Base_ts* base, char* comPort)
*
*	Description: Initializes a base object using the arguments passed.
*		The comm port passed as comPort is opened and set for read/write.
*
*		The device parameters for the base controller are set per the
*		documentation:
*			9600 baud
*			1 start bit
*			1 stop bit
*			no parity
*
*		The comm port timeouts are set to a constant of 50 ms with a multiplier
*		of 10.
*
*	Parameters:
*
*	base		I/O	A pointer to the base object to initialize.
*	comPort		I/P	The comm port on which to communicate with the base.
*
*	Returns:
*	int			If the base is initialized successfully returns 1, 0 otherwise.
*				Failure may occur due to:
*					not being able to open a connection on comPort,
*					not being able to poll the connected device for its	state,
*					not being able to set the device parameters,
*					not being able to set the com port timeouts. 
*******************************************************************************/
int base_init(Base_ts* base, char* comPort) {
	if(base == NULL) return 1;

	/* Open the serial port */
	fprintf(stderr, "Opening serial port...");
	base->hSerial = CreateFile(comPort, GENERIC_READ | GENERIC_WRITE, 0, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if(base->hSerial == INVALID_HANDLE_VALUE) {
		fprintf(stderr, "Error\n");
		return 1;
	}
	else {
		fprintf(stderr, "OK\n");
	}

	/* Set device parameters (9600 baud, 1 start bit, 1 stop bit, no parity) */
	base->dcbSerialParams.DCBlength = sizeof(base->dcbSerialParams);
	if(GetCommState(base->hSerial, &base->dcbSerialParams) == 0) {
		fprintf(stderr, "Error getting device state\n");
		base_close(base);
		return 1;
	}

	base->dcbSerialParams.BaudRate = CBR_9600;
	base->dcbSerialParams.ByteSize = 8;
	base->dcbSerialParams.StopBits = ONESTOPBIT;
	base->dcbSerialParams.Parity = NOPARITY;
	if(SetCommState(base->hSerial, &base->dcbSerialParams) == 0) {
		fprintf(stderr, "Error setting device parameters\n");
		base_close(base);
		return 1;
	}

	/* Set COM port timeout settings */
	base->timeouts.ReadIntervalTimeout = 50;
	base->timeouts.ReadTotalTimeoutConstant = 50;
	base->timeouts.ReadTotalTimeoutMultiplier = 10;
	base->timeouts.WriteTotalTimeoutConstant = 50;
	base->timeouts.WriteTotalTimeoutMultiplier = 10;
	if(SetCommTimeouts(base->hSerial, &base->timeouts) == 0) {
		fprintf(stderr, "Error setting timeouts\n");
		base_close(base);
		return 1;
	}

	return 0;
}

/*******************************************************************************
*	void base_close(Base_ts* base)
*
*	Description: Closes the base object
*
*		Closes the connection to the underlying communication port pointed to by
*		hSerial.
*
*	Parameters:
*
*	base		I/O	A pointer to the base object to close.
* 
*******************************************************************************/
void base_close(Base_ts* base) {
	fprintf(stderr, "Closing serial port...");

	if(CloseHandle(base->hSerial) == 0)
		fprintf(stderr, "Error\n");

	fprintf(stderr, "OK\n");
}

/*******************************************************************************
*	int base_sendData(Base_ts* base, int8_t bytesToSend[])
*
*	Description:	Sends the data bytes passed as the second argument to the
*					base object passed as the first argument.
*
*		The data is sent by passing the bytes to a WriteFile call on the
*		comm port pointed to by	hSerial. 
*
*	Parameters:
*
*	base			I/P	A pointer to the base object to send data to.
*	bytesToSend		I/P	Array of bytes to send to the base object.
*
*	Returns:
*	int			If the bytes are sent successfully returns 1, 0 otherwise.
*				Failure may occur due to a failure of the call to WriteFile. 
*******************************************************************************/
int base_sendData(Base_ts* base, int8_t bytesToSend[]) {
	int ret_val = 0;
	DWORD bytes_written = 0;

//	fprintf(stderr, "Sending bytes...");
	if(!WriteFile(base->hSerial, bytesToSend, 3, &bytes_written, NULL)) {
		fprintf(stderr, "Error\n");
		ret_val = 1;
	}

//	fprintf(stderr, "%ld bytes written\n", bytes_written);
	return ret_val;
}
