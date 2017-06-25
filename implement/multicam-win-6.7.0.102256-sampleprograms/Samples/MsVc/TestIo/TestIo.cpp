/*
+-------------------------------- DISCLAIMER ---------------------------------+
|                                                                             |
| This application program is provided to you free of charge as an example.   |
| Despite the considerable efforts of Euresys personnel to create a usable    |
| example, you should not assume that this program is error-free or suitable  |
| for any purpose whatsoever.                                                 |
|                                                                             |
| EURESYS does not give any representation, warranty or undertaking that this |
| program is free of any defect or error or suitable for any purpose. EURESYS |
| shall not be liable, in contract, in torts or otherwise, for any damages,   |
| loss, costs, expenses or other claims for compensation, including those     |
| asserted by third parties, arising out of or in connection with the use of  |
| this program.                                                               |
|                                                                             |
+-----------------------------------------------------------------------------+
*/
// TestIo.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

using namespace std;

MCSTATUS status;
const MCHANDLE hConfig = MC_CONFIGURATION;
MCHANDLE hBoard;
int BoardCount, pci;
char name[64], identifier[64], state[10];
int SleepTime;

int test_error();
int test_output();
int test_input();


// -----------------------------------------------------------------------------------------
// TEST
int main(int argc, char *argv[])
{
	int i;
	char mode;

	if (argc > 1)
		SleepTime = atoi(argv[1]);
	else
		SleepTime = 1000;

	// OPEN DRIVER
	printf("Opening driver... ");
	fflush(stdout);
	status = McOpenDriver(NULL);
	if (status != MC_OK)
		return test_error();
	printf("done\n");

	// INITIALIZATION
	status = McGetParamNmInt(hConfig, "BoardCount", &BoardCount);
	if (status != MC_OK)
		return test_error();
	printf("BoardCount = %i\n", BoardCount);
	for (i = 0; i < BoardCount; i++)
	{
		status = McSetParamNmInt(hConfig, "DriverIndex", i);
		if (status != MC_OK)
			return test_error();
		status = McGetParamNmInt(hConfig, "PciPosition", &pci);
		if (status != MC_OK)
			return test_error();
		status = McGetParamNmStr(hConfig, "BoardName", name, sizeof(name));
		if (status != MC_OK)
			return test_error();
		status = McGetParamNmStr(hConfig, "BoardIdentifier", identifier, sizeof(identifier));
		if (status != MC_OK)
			return test_error();
		printf("%i   0x%08x   %16s   %s\n", i, pci, name, identifier);
	}
	printf("Pick a board (driver index): ");
	fflush(stdout);
	cin >> i;
	status = McSetParamNmInt(hConfig, "DriverIndex", i);
	if (status != MC_OK)
		return test_error();
	hBoard = MC_BOARD + i;

	printf("Input - Output (I/O)? ");
	fflush(stdout);
	cin >> mode;
	switch (mode)
	{
	case 'o':
	case 'O':
		status = test_output();
		break;
	case 'i':
	case 'I':
		status = test_input();
		break;
	}

	// CLOSE DRIVER
	printf("Closing driver... ");
	fflush(stdout);
	status = McCloseDriver();
	printf("done\n");
	printf("\n\nstatus = %i\n", status);
	return status;
}


// -----------------------------------------------------------------------------------------
// ERROR
int test_error()
{
	printf("\n\nstatus = %i\n", status);
	system("pause");
	return status;
}


// -----------------------------------------------------------------------------------------
// OUTPUT
int test_output()
{
	int i, output, mode;
	char c;
	int style;
	bool loop = true;
		
	// Select Output
	printf("Refer to the select board documentation to find available output index\n");
	printf("Pick an output index (1 based) \n 0 to exit: ");
	while (loop)
	{
		
		fflush(stdout);
		cin >> output;
		if ( output == 0) return 0; // user exit
		// check if the output exist and configure it
		printf("Setting OutputConfig:%i to SOFT... \n", output);
		fflush(stdout);
		status = McSetParamInt(hBoard, MC_OutputConfig+output, MC_OutputConfig_SOFT);
		if (status != MC_OK)
			printf("Output %i not available for selected board, select another output\n", output);
		else 
			loop = false;
	}
	// Select Output Style
	loop = true;
	printf("Refer to the select board documentation and selected output to find available output style\n");
	printf("Select Output Style\n0. Default\n1. TTL\n2. ITTL\n3. IOC\n4. IOE\n5. RELAY\n6. LED\n7. OC\n8. LVDS\n9. DIFF\n10. OPTO\n11. SSRLY\n");
	while (loop)
	{
		fflush(stdout);
		cin >> style;
		if ( style == 0) // lease style as it is
			loop = false;
		else
		{
			printf("Setting OutputStyle:%i...\n", style);
			fflush(stdout);
			status = McSetParamInt(hBoard, MC_OutputStyle+output, style);
			if (status != MC_OK)
				printf("Fail to set request style, select another output style\n");
			else
				loop = false;
		}
	}



	printf("Mode (1. random - 2. manual)? \n");
	fflush(stdout);
	cin >> mode;

	// OUTPUT STATE
	if (mode == 1) // random sequence
	{
		for (;;)
		{
			i = (rand() < RAND_MAX / 2) ? MC_OutputState_LOW : MC_OutputState_HIGH;
			if (i == MC_OutputState_LOW)
				strcpy_s(state,10, "LOW");
			else if (i == MC_OutputState_HIGH)
				strcpy_s(state,10, "HIGH");
			printf("  OutputState:%i = %s\n", output, state);
			status = McSetParamInt(hBoard, MC_OutputState+output, i);
			if (status != MC_OK)
				return test_error();
			Sleep(SleepTime);
		}
	}
	else // manual
	{
		printf("Enter 0 (LOW) or 1 (HIGH)\n\n");
		for (;;)
		{
			c = getchar();
			if (c == '0')
			{
				status = McSetParamInt(hBoard, MC_OutputState+output, MC_OutputState_LOW);
				if (status != MC_OK)
					return test_error();
			}
			else if (c == '1')
			{
				status = McSetParamInt(hBoard, MC_OutputState+output, MC_OutputState_HIGH);
				if (status != MC_OK)
					return test_error();
			}
		}
	}
}


// -----------------------------------------------------------------------------------------
// INPUT
int test_input()
{
	int i, input, mode;
	int style;
	bool loop = true;
		
	// Select Input
	printf("Refer to the select board documentation to find available input index\n");
	printf("Pick an input index (1 based) \n 0 to exit: ");

	while (loop)
	{
		fflush(stdout);
		cin >> input;
		if ( input == 0) return 0; // user exit
		// check if the input exist and configure it
		printf("Setting InputConfig:%i to SOFT... ", input);
		fflush(stdout);
		status = McSetParamInt(hBoard, MC_InputConfig+input, MC_InputConfig_SOFT);
		if (status != MC_OK)
			printf("Input %i not available for selected board, select another input\n", input);
		else 
			loop = false;
	}
	// Select Input Style
	loop = true;
	printf("Refer to the select board documentation and selected input to find available input style\n");
	printf("Select Input Style\n0. Default\n1. TTL\n2. ITTL\n3. I12v\n4. RS422\n5. LVDS\n6. DIFF\n7. CHANNELLINK\n8. CMOS\n9. AUTO\n10. RELAY\n11. 5V\n12. 12V\n13. TTLHiz\n14. TTLPD\n15. RS485\n16. DTTL\n17. ISO\n18. POWERSTATE\n");
	while (loop)
	{
		fflush(stdout);
		cin >> style;
		if ( style == 0) // lease style as it is
			loop = false;
		else
		{
			printf("Setting InputStyle:%i...\n", input);
			fflush(stdout);
			status = McSetParamInt(hBoard, MC_InputStyle+input, style);
			if (status != MC_OK)
				printf("Fail to set request style, select another input style\n");
			else
				loop = false;
		}
	}

	printf("Mode (1. auto - 2. manual)? ");
	fflush(stdout);
	cin >> mode;


	if (mode == 1) // automatic
	{
		for (;;)
		{
			status = McGetParamInt(hBoard, MC_InputState+input, &i);
			if (status != MC_OK)
				return test_error();
			if (i == MC_InputState_LOW)
				strcpy_s(state,10, "LOW");
			else if (i == MC_InputState_WENTLOW)
				strcpy_s(state,10, "HIGH -> LOW");
			else if (i == MC_InputState_HIGH)
				strcpy_s(state,10, "HIGH");
			else if (i == MC_InputState_WENTHIGH)
				strcpy_s(state,10, "LOW -> HIGH");
			else if (i == MC_InputState_NONE)
				strcpy_s(state,10, "NONE");
			else if (i == MC_InputState_DISCONNECTED)
				strcpy_s(state, 10,"DISCONNECTED");
			else if (i == MC_InputState_OPEN)
				strcpy_s(state,10, "OPEN");
			else
				sprintf_s(state,10, "%i", i);
			printf("  InputState:%i = %s\n", input, state);
			Sleep(SleepTime);
		}
	}
	else // manual
	{
		printf("Press ENTER to read input\n\n");
		for (;;)
		{
			getchar();
			status = McGetParamInt(hBoard, MC_InputState+input, &i);
			if (status != MC_OK)
				return test_error();
			if (i == MC_InputState_LOW)
				strcpy_s(state,10, "LOW");
			else if (i == MC_InputState_WENTLOW)
				strcpy_s(state,10, "HIGH -> LOW");
			else if (i == MC_InputState_HIGH)
				strcpy_s(state,10, "HIGH");
			else if (i == MC_InputState_WENTHIGH)
				strcpy_s(state, 10,"LOW -> HIGH");
			else if (i == MC_InputState_NONE)
				strcpy_s(state,10, "NONE");
			else if (i == MC_InputState_DISCONNECTED)
				strcpy_s(state,10, "DISCONNECTED");
			else if (i == MC_InputState_OPEN)
				strcpy_s(state, 10,"OPEN");
			else
				sprintf_s(state,10, "%i", i);
			printf("  InputState:%i = %s", input, state);
			fflush(stdout);
		}
	}
}
