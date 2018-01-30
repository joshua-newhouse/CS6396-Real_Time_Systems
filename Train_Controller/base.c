#include <windows.h>
#include <stdio.h>

#include "base.h"

int base_init(Base_ts* base, char* comPort) {
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

void base_close(Base_ts* base) {
	fprintf(stderr, "Closing serial port...");

	if(CloseHandle(base->hSerial) == 0)
		fprintf(stderr, "Error\n");

	fprintf(stderr, "OK\n");
}

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
