/************************************************************************************************
 Author: Davis DeRuiter
 Date: 11/24/2016
 File: Main.cpp

 Class: CSIS 443-001
 School: Liberty University
 Assignment: Final Project

 Description: The purpose of this program is simulate race conditions by spawning multiple
              threads and having them modify a variable (hopefully at the same time at some 
			  point). Also, a mutex is created and used to show how locking prevents an
			  inconsistent state arising when modifying a variable with multiple threads. 
 ************************************************************************************************/

#include <iostream>
#include <process.h>
#include <windows.h>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <stdlib.h>
#include <sstream>

using namespace std;

// Function prototypes
unsigned WINAPI threadWork(LPVOID param);
bool checkArgValidRange(int numberOfThreads, int mutexOption);

// Global variables
bool g_mutexEnabled = false;			// whether or not to use mutex for critical section
long long int g_count = 0;		// actual count after update by threads
unsigned long g_numberIter = 0;			// number of cycles to burn to attempt to cause contention
HANDLE g_mutex;							// mutex for critical section
const int CS_ITERATIONS = 10;			// hold number of iterations for critical section

/************************************************************************************************
 Drives the program by showing the inital parameters, spawning the threads, closing them once 
 they have finished, and displaying the results
************************************************************************************************/
int main(int argc, char * argv[]) 
{
	// Local variables
	int numberOfThreads;
	unsigned long long int theoreticalCount = 0;
	DWORD targetThreadID;
	int mutexOption;

	printf("CSIS 443 - Final Project\n");
	printf("\n");

	// Check if invalid number of arguments entered
	if (argc != 4) {
		printf("    ERROR: Invalid number (%d) of parameters.\n", argc);
		printf("\n");

		// Pause before program termination
		char temp;
		cout << "Enter any key to exit: ";
		cin >> temp;

		return -1;
	}
	else {
		// Store command line parameters
		numberOfThreads = atoi(argv[1]);		// Number of threads
		g_numberIter = atol(argv[2]);			// Number of iterations
		mutexOption = atoi(argv[3]);			// Enable/disable mutex
	}

	// Check that all passed in arguments are in valid range
	if (!checkArgValidRange(numberOfThreads, mutexOption)) {
		// Pause before program termination
		char temp;
		cout << "Enter any key to exit: ";
		cin >> temp;

		return 1;
	}
	else {
		// Convert from int arg to boolean
		g_mutexEnabled = mutexOption;
	}

	// Dynamic variables
	HANDLE * threadHandles = new HANDLE[numberOfThreads];

	// Log execution parameters
	printf("-----------------------------------------\n");
	printf("|         Execution parameters          |\n");
	printf("-----------------------------------------\n");
	printf("  # of Threads:    %u\n", numberOfThreads);
	printf("  # of Iterations: %u\n", g_numberIter);
	if (g_mutexEnabled) {
		printf("  Mutex enabled:   %d (YES)\n", g_mutexEnabled);
	}
	else {
		printf("  Mutex enabled:   %d (NO)\n", g_mutexEnabled);
	}
	printf("--------------------------------------\n");
	printf("\n");

	// Initialize global mutex used for critical section
	g_mutex = CreateMutex(
		NULL,	// default security attributes
		FALSE,	// initially not owned
		NULL);	// unnamed mutex

	// Check if mutex creation failed
	if (g_mutex == NULL) {
		printf("    ERROR: Mutex creation failed. %d\n", GetLastError());
		return 1;
	}

	// Log thread execution header
	printf("-----------------------------------------\n");
	printf("|           Threads Running             |\n");
	printf("-----------------------------------------\n");

	// Spawn threads
	for (int i = 0; i < numberOfThreads; i++) {
		// Create a new thread to execute simulated work
		threadHandles[i] = (HANDLE)_beginthreadex(
			0,													// default security attributes
			0,													// default stack size
			&threadWork,										// function to run
			0,													// thread function arguments
			0,													// default creation flags
			(unsigned *)&targetThreadID);						// receive thread identifier

		// Check if thread creation failed
		if (threadHandles[i] == NULL) {
			stringstream message;
			message << "    ERROR: Thread creation failed. " << GetLastError() << endl;

			// Log thread creation error
			cout << message.str();
			return 1;
		}
		else {
			// Calculate theoretical value of count
			theoreticalCount = theoreticalCount + (CS_ITERATIONS * targetThreadID * g_numberIter);
		}

		Sleep(10);	// let the new thread run
	}
	
	// Wait till all threads have finished executing
	// (Used WaitForSingleObject because WaitForMultipleObjects can only wait on a maximum of 64 threads)
	for (int i = 0; i < numberOfThreads; i++) {
		DWORD result = WaitForSingleObject(threadHandles[i], 0);

		// Check if thread not signaled
		while (result != WAIT_OBJECT_0) {
			// Thread is still alive
			Sleep(100);
			result = WaitForSingleObject(threadHandles[i], 0);
		}
	}

	// Output thread execution ending line
	printf("-----------------------------------------\n");

	// Close all thread handles
	for (int i = 0; i < numberOfThreads; i++) {
		CloseHandle(threadHandles[i]);
	}

	// Close mutex handle
	CloseHandle(g_mutex);

	// Output the actual value of count and the theoretical
	printf("\n");
	printf("-----------------------------------------\n");
	printf("|    Values of global variable count    |\n");
	printf("-----------------------------------------\n");
	printf(" Theoretical : %d\n", theoreticalCount);
	printf(" Actual      : %d\n", g_count);
	printf("-----------------------------------------\n");
	printf("\n");

	// Pause before program termination
	char temp;
	cout << "Enter any key to exit: ";
	cin >> temp;
	return 0;
}

/************************************************************************************************
 Simulates CPU activity and generates possible race events
 ***********************************************************************************************/
unsigned WINAPI threadWork(LPVOID param)
{
	stringstream message;
	int threadId = GetCurrentThreadId();
	
	// Log thread creation
	message << "  Thread " << threadId << " created." << endl;
	cout << message.str();
	
	// Create a CPU load by burning some cycles in order to hope to cause contention
	for (unsigned long i = 0; i < g_numberIter; i++) {
		
		// Check if mutex enabled
		if (g_mutexEnabled) {
			DWORD waitResult = WaitForSingleObject(
				g_mutex,		// handle to mutex
				INFINITE);		// no time-out interval

			switch (waitResult) {
				case WAIT_OBJECT_0:{
					// Critical section.
					for (int j = 0; j < CS_ITERATIONS; j++) {
						g_count = g_count + abs((int)threadId);
					}

					// Check if release ownership of the mutex object succeeded
					if (!ReleaseMutex(g_mutex)) {
						message << "    ERROR: Mutex release failed." << endl;
						cout << message.str();
					}
				}
				case WAIT_FAILED: {
					// Don't do anything (just keep running)
				}
				case WAIT_ABANDONED: {
					// Don't do anything (just keep running)
				}
				default: 
					// Don't do anything (just keep running)
					break;
				}
		}
		else {
			// Critical section.
			for (int j = 0; j < CS_ITERATIONS; j++)
				g_count = g_count + abs((int)threadId);
		}
	}

	// Log thread termination
	message << "  Thread " << threadId << " terminated." << endl;
	cout << message.str();

	// Notify main this thread is done
	return TRUE;
}

/************************************************************************************************
 Check if passed in arguments are in valid range
***********************************************************************************************/
bool checkArgValidRange(int numberOfThreads, int mutexOption) 
{
	// Check if number of threads is in valid range
	if (numberOfThreads < 0 || numberOfThreads > 100) {
		printf("    ERROR: Invalid number number of threads entered.\n");
		printf("           Must be between 0 to 100.\n");
		printf("\n");

		return false;
	}

	// Check if number of iterations is in valid range
	if (g_numberIter < 0 || g_numberIter > 100000000) {
		printf("    ERROR: Invalid number number of threads entered.\n");
		printf("           Must be between 0 to 100000000.\n");
		printf("\n");

		return false;
	}

	// Check if mutex enabled option is in valid range
	if (mutexOption != 0 && mutexOption != 1) {
		printf("    ERROR: Invalid number number of threads entered.\n");
		printf("           Must be 0 (NO) or 1 (YES).\n");
		printf("\n");

		return false;
	}

	return true;
}