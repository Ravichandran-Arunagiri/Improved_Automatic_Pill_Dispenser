/*
Erin Butler, Nithya Muralidaran, Arunagiri Ravichandran
Robots for Disability - ROBGY 6413
Simulation Project: Automatic Pill Dispenser
Fall 2022
*/

/*
v5 cleans up print statements for easier reading during demo. More comments are added.
v4 when pills are dispensed pillCounts is updated and warning 
	sent to careiver to refill
v3 when not dispensing, allows user to make edits to prescription.
v2 allows for pill profile entry and monitors dispensing and 
	pill status when min = 0.
v1 includes all user input functions for entering a pill profile. 
*/

//--------------------------------------------------------------------
// Include needed header files. 
#include <stdio.h>
#include <time.h>
#include <windows.h> // Getting current time.
#include <unistd.h> // Using timeout functions.
#include <sys/time.h>
#include <sys/types.h>
#include <stdlib.h> // Using system("clear"); to clear terminal.
/*
Sources: 
Obtaining current computer time. 
https://www.geeksforgeeks.org/time-h-header-file-in-c-with-examples/
Waiting a certain amount of time for user input (timeout function). 
https://stackoverflow.com/questions/7226603/timeout-function
*/
//--------------------------------------------------------------------

//--------------------------------------------------------------------
// Initialize variables and functions for storing pill profiles. 
#define MAX_PILLS 20 // Assume a patient takes no more than 20 pills.
/* 
   Each row in the following represents one pill. The columns represent
   when the pill will be dispensed. 0 = pill is not dispensed. 1 = pill
   is dipensed. 
*/   
int dispDayPill[MAX_PILLS][7] = {0}; // Columns represent one day.
int dispHourPill[MAX_PILLS][24] = {0}; // Columns represent one hour.
void inputPillInfo(void); // Walks user through entering prescription. 
void addDays(void); // Adds the days for current pill to be dispensed. 
int currentPill = 1; // Variable used to index into dispense arrays. 
void addHours(void); // Adds the hours at which pill will be dispensed. 
int pillCounts[MAX_PILLS][1] = {0}; // Stores how many pills are loaded. 
void addPillCount(void); // Adds the number of pills loaded. 
//--------------------------------------------------------------------

//--------------------------------------------------------------------
// Initialize variables and functions for current time and day. 
int day, hour, min;	
struct tm* ptr;
time_t t;
void updateNow(void); // Updates the current time.

// int day = 0, hour = 0, min = 0; // DEMO Demo starts on Sunday at 00:00. 
// timeCounter = -15; // DEMO used in demo code
//--------------------------------------------------------------------

//--------------------------------------------------------------------
/*
Initialize functions and variables for checking for dispense and 
monitoring if patient has retrieved their pills.
*/
void checkDispense(void); // Checks if any pills need to be dispensed.
/*
Time to wait for patient to take pills with each notification. In actual
use this time should be 5 minutes. 
*/
#define WAIT 300
// #define WAIT 5 // DEMO value
int pillFlag = 0; // Variable to monitor if pills have been taken. 
void monitorPillStatus(void); // Changes notification based on pill status.
void checkForRefill(); // Alerts caregiver if refill is needed.
//--------------------------------------------------------------------

//--------------------------------------------------------------------
// Initialize variables and functions for changing prescription. 
void checkForUpdates(); // Allows user to update prescription.
#define WAIT2 15 // Wait time for user input. Shorter for demo.
//#define WAIT2 0.5 // DEMO  value
//--------------------------------------------------------------------

//--------------------------------------------------------------------
// Displays values in prescription arrays to show update actions.
void printCheck();
//--------------------------------------------------------------------

//--------------------------------------------------------------------
//--------------------------------------------------------------------
int main(int argc, char *argv)
{   
	/*
	On first startup, the user will enter the current number of pills
	in the patients prescription. Then, they will be guided through
	entering the details for each pill.
	*/
	int count; // The number of pills to be entered at startup.
	printf("Enter number of pills: ");
	scanf(" %d", &count);
	getchar(); // Clear the input line. 
	
	/*
	For loop calls the inputPillInfo() function to allow user to enter
	details for each pill. The currentPill is incremented each time.
	*/
	for (int i = 0; i < count; i++)
	{	
		inputPillInfo();
		currentPill++;
	}
	printf("\nFinished initial prescription input. Clearing screen.\n\n");
	printf("Current input: \n");
	printCheck();
	printf("\n");
	Sleep(5000);
	system("clear");

	// The while loop will run indefinitly. 
	while(1)
	{
		// Update the current time. 
		updateNow();	
		/*
		Pills are dispensed on the hour mark. When the current 
		minute is 0, check if any pills need to be dispensed. 
		*/
		if (min == 0) 
		{
			printf("Checking for dispense. \n");
			printf("Current Day: %d 	Time: %d:", day+1, hour);
			if (min < 10)
			{ 
				printf("0%d\n", min);
			}
			else
			{
				printf("%d\n", min);
			}
			/*
			Check dispense output what pills are being dispensed at the
			current time. It will also update the pill counts.
			*/
			checkDispense(); 
			// The counts are checked to detemrine if refill is needed.
			checkForRefill();
		}	
		/*
		The user can make changes to the prescription as long as the
		time is well before the hour mark. This will ensure that no 
		pill dispenses are missed. 
		*/
		else if (min < 55) 
		{
			system("clear");
			// checkForUpdates() allows the user to make changes.
			checkForUpdates();
		}
		// An idle state is included to ensure no dispenses are missed.
		else
		{ 
			system("clear");
			printf("Idle state. No updates allowed.\n");
			//Sleep(10); // For demo, sleep for 1 second then check again.
		}
	} 
	
	return 0;
}
//--------------------------------------------------------------------
//--------------------------------------------------------------------

void printCheck()
{
	// print check 
	for (int j = 0; j<5; j++)
	{
		for (int i = 0; i<7; i++)
		{
			printf("%d", dispDayPill[j][i]);
		}
		printf("\t");
		
		for (int i = 0; i<24; i++)
		{
			printf("%d", dispHourPill[j][i]);
		}
		printf("\t%d", pillCounts[j][1]);
		printf("\n");
	}
}

//--------------------------------------------------------------------
/*
After pills have been dispensed, this function can be run to determine
if any tracks need to be refilled. If the pill count is less than 10 
for any pill, the caregiver will be notified.
*/
void checkForRefill()
{
	int flag = 0; // Tracks if any pills need to be refilled.
	int refill[MAX_PILLS] = {0}; // Updates if pill needs refill.
	for (int i = 0; i<MAX_PILLS; i++) // Check each track.
	{
		/*
		Check if this track dispenses pills. If track is not used then
		it will not signal a refill needed.
		*/
		for (int j = 0; j < 7; j++) 
		{
			if (dispDayPill[i][j] == 1) // A 1 indicates track is used.
			{
				// If the count is low, update the refill array.
				if (pillCounts[i][1] < 10)  
				{
					flag = 1; // Change flag if refill is needed.
					refill[i] = 1;
				}					
			}
		}
	}
	// If there are pills to be refilled, display message.
	if (flag == 1) 
	{
		printf("\nThe following tracks have less than 10 pills: ");
		for (int i = 0; i<MAX_PILLS; i++)
		{
			if (refill[i] == 1)
			{
				printf("%d  ", i+1);
			}
		}
		printf("\nRefill suggested. Notification sent to caregiver.\n");
		Sleep(5000);
	}
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
/*
This function allows the user to enter in an action to update the 
patient's prescription. This function was adapted from 
https://stackoverflow.com/questions/7226603/timeout-function
to have desired input and output types.
*/
void checkForUpdates()
{
    char            input[20] = {0}; 
    fd_set          input_set;
    struct timeval  timeout;
    int             ready_for_reading = 0;
    int             read_bytes = 0;
    
    /* Empty the FD Set */
    FD_ZERO(&input_set );
    /* Listen to the input descriptor */
    FD_SET(STDIN_FILENO, &input_set);

    /* Waiting for some seconds */
    timeout.tv_sec = WAIT2;    // WAIT seconds
    timeout.tv_usec = 0;    // 0 milliseconds

    /* 
	The following defines allowed inputs and thier actions for
	changing the prescription.
	*/
    printf("Enter action: \n");
	printf("\tU: Update existing pill\n");
	printf("\tR: Refill existing pill\n");
	printf("\tN: Add new pill\n");
	printf("\tD: Delete existing pill\n");

    // Listening for input stream for any activity.
    ready_for_reading = select(1, &input_set, NULL, NULL, &timeout);
    /* 
	Here, first parameter is number of FDs in the set, second is 
	FD set for reading, third is the FD set in which any write
	activity needs to be updated, which is not required in this 
	case. Fourth is timeout.
     */

    if (ready_for_reading == -1) 
	{
        /* Some error has occured in input */
        printf("Unable to read your input\n");
    } 

    if (ready_for_reading) 
	{
        read_bytes = read(0, input, 19);
        if(input[read_bytes-1]=='\n')
		{
        --read_bytes;
        input[read_bytes]='\0';
        }
        if(read_bytes==0)
		{
            printf("You just hit enter\n");
        } 
		else 
		{
			/*
			If an appropriate input is entered, the user will be 
			guided through the needed update actions. 
			*/
			if (input[0] == 'u' || input[0] == 'U')
			{
				printf("Update Existing:\n");
				printf("Enter pill (track number) to update: ");
				int temp;
				scanf("%d", &temp);
				printf("\n");
				getchar(); // Clear the input line.
				printf("Before update: \n");
				printCheck();
				printf("\n");
				currentPill = temp; // Update the pill to be edited.
				inputPillInfo(); // Call the input function to update.
			}
			else if (input[0] == 'r' || input[0] == 'R')
			{
				printf("Refill Existing:\n");
				printf("Enter pill (track number) to refill: ");
				int temp;
				scanf("%d", &temp);
				getchar(); // Clear the input line.
				printf("Enter new pill count: ");
				int temp2;
				scanf("%d", &temp2);
				getchar(); // Clear the input line.
				printf("Before update: \n");
				printCheck();
				printf("\n");
				pillCounts[temp-1][1] = temp2; // Update the pillCount.		
			}
			else if (input[0] == 'n' || input[0] == 'N')
			{
				printf("Add New Pill:\n");
				// Determine next open track.
				int i = 0;
				while (pillCounts[i][1] > 0)
				{
					i++;
				}
				/*
				When the loop breaks, i has the index of the first 
				unused track. This will become the current pill.
				*/
				currentPill = i+1;
				printf("Before update: \n");
				printCheck();
				printf("\n");
				inputPillInfo(); // Call the input function to update.			
			}	
			else if (input[0] == 'd' || input[0] == 'D')
			{
				printf("Delete Existing:\n");
				printf("Enter pill (track number) to delete: ");
				int temp; 
				scanf("%d", &temp);
				printf("Before update: \n");
				printCheck();
				printf("\n");
				// Clear the arrays at the index chosen. 
				for (int i = 0; i<7; i++) 
				{
					dispDayPill[temp-1][i] = 0;
				}
				for (int i = 0; i<24; i++)
				{
					dispHourPill[temp-1][i] = 0;
				}
				pillCounts[temp-1][1] = 0;
			}
			else
			{
				printf("Not a correct action.\n");
			}
			printf("After update: \n");
			printCheck();
			printf("\n");
			Sleep(5000);
			system("clear");
        }
    } 
	else 
	{
		// If not actions are entered print a new line. 
        printf("\n");
    }
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
/*
This function is called to determine if the patient has taken their
pills. Pills being taken is simualted by inputing anything and 
pressing enter. There are 3 levels of notification in an attempt to 
give the patinet an opportunity to take their medication on their own.

This function is called from the checkDispense() function. 

The same timeout function from the checkForUpdates() function is again
modified here to allow for the appropriate input and output.
*/
void monitorPillStatus ()
{
    char            input[20] = {0}; 
    fd_set          input_set;
    struct timeval  timeout;
    int             ready_for_reading = 0;
    int             read_bytes = 0;
    
    /* Empty the FD Set */
    FD_ZERO(&input_set );
    /* Listen to the input descriptor */
    FD_SET(STDIN_FILENO, &input_set);

    /* Waiting for some seconds */
    timeout.tv_sec = WAIT;    // WAIT seconds
    timeout.tv_usec = 0;    // 0 milliseconds

    // Prompt for an input when pills are taken.
    printf("Enter anything when pills are taken.\n");
    /* Listening for input stream for any activity */
    ready_for_reading = select(1, &input_set, NULL, NULL, &timeout);
    /* 
	Here, first parameter is number of FDs in the set, second is 
	FD set for reading, third is the FD set in which any write
	activity needs to be updated, which is not required in this 
	case. Fourth is timeout.
     */

    if (ready_for_reading == -1) 
	{
        /* Some error has occured in input */
        printf("Unable to read your input\n");
    } 

    if (ready_for_reading) 
	{
        read_bytes = read(0, input, 19);
        if(input[read_bytes-1]=='\n')
		{
        --read_bytes;
        input[read_bytes]='\0';
        }
		/*
		If pills have been taken, the message is printed and the pillFlag
		is updated. 
		*/
        if(read_bytes==0)
		{
            printf("Pills taken.\n");
			pillFlag = 1;
        } 
		else
		{
            printf("Pills taken.\n");
			pillFlag = 1;
        }
    } 
	// If pills are not taken, the message is printed. 
	else 
	{
		//printf("Pills not taken.\n");
		pillFlag = 0;
    }    
	return;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
/*
This function will be called when the minute value is 0. It will 
determine if any pills need to be dispensed. If pill are dispsened 
the monitorPillStatus function is called to determine if the patient 
has taken their pills. Depending on the pillFlag status, the 
notification type will be updated. 
*/
void checkDispense(void)
{
	int flag = 0; // Flag updates if there are pills to dispense.
	int dispenseNow[MAX_PILLS] = {0}; // Reset the dispense array.
	for (int i = 0; i < MAX_PILLS; i++) // Checks each track.
	{ 
		if (dispDayPill[i][day] == 1) // Checks the day this day.
		{
			if (dispHourPill[i][hour] == 1) // Checks the hour.
			{
				dispenseNow[i] = 1; // Updates dispense array.
				// Update pill count.
				pillCounts[i][1] = pillCounts[i][1] - 1;  
				flag = 1; // Update flag.
			}
		}
	}
	if (flag ==1) // Print which pills are dispensed if any.
	{
		printf("\nDispensing these pills: ");
		for (int i = 0; i < MAX_PILLS; i++)
		{
			if (dispenseNow[i] == 1)
			{
				printf("%d    ", i+1);
			}
		}	
		printf("\n");
		
		// Monitor pill status and update notification as needed.
		pillFlag = 0; // Reset pillFlag.
		// Display first notification.
		printf("\nNotification 1: LED flashing every 10 seconds.\n");
		// Monitor pill status and wait for input.
		monitorPillStatus();
		// If pills are not taken after notification 1, update notification.
		if (pillFlag == 0) 
		{
			printf("\nPills not taken in 5 minutes.\n");
			// Display second notification.
			printf("Notification 2: LED on, sound buzzer every 15 seconds.\n");
			// Monitor pill status and wait for input.
			monitorPillStatus();
		}
		// If pills are not taken after notification 2, update notification.		
		if (pillFlag == 0) 
		{
			printf("\nPills not taken in 10 minutes.\n");
			// Display third notification
			printf("Notification 3: Alert sent to caregiver. LED on, sound buzzer every 15 seconds.\n");
			// Monitor pill status and wait for input.
			monitorPillStatus();
		}
		/*
		If pills are not taken after notification 3, assume caregiver
		will notify patient of taking their pills. Allow dispenser to 
		continue monitoring for future dispenses. 
		*/
		if (pillFlag == 0) 
		{
			printf("\nPills not taken in 15 minutes. Caregiver will ensure pills are taken.\n");
		}
		printf("\nMonitor for next dispense.\n");
		Sleep(2000);
	}
	else 
	{
		printf("No pills to dispense. Dispenser locked. \n");
		Sleep(1000);
	}
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
// Updates the current time and day. 
void updateNow(void) {
	
	// These variables are used in the actual simulation.
	t = time(NULL);
    ptr = localtime(&t);
	day = ptr->tm_wday;
	hour = ptr->tm_hour;
	min = ptr->tm_min;
	
	/*
	// These varaible are used in the demo simulation. 	
	timeCounter = timeCounter + 15; // Counter initially starts at -1.
	
	There are 1440 minutes in a day. In the demo simulation, a counter
	will be used to go through each minute. When 60 minutes passes, 
	the hour will be updated and the minutes reset. When 24 hours passes, 
	the day will be updated and the hours reset. After 7 days (1 week), 
	the day will be reset. 
	
	if (timeCounter == 1440) // 1440 minutes in a day.
	{
		day++; // Increase the day if 1440 minutes has gone by.
		if (day == 7) // Reset the week if 7 days have gone by.
		{
			day = 0;
		}
		hour = 0; // Reset the hour.
		min = 0; // Reset the minute.
		timeCounter = 0; // Reset the counter for the next day.
	}
	else
	{
		if (timeCounter%60 == 0) // If 60 minutes has gone by, update. 
		{
			hour++; // Update the hour.
			min = 0; // Update the minute.
		}
		else 
		{
			min = timeCounter%60; // Set current minute. 
		}
	}
	*/
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
/* 
This function is used to input pill info. Before calling this 
function, the currentPill needs to be updated.On startup, an inital 
loop can be used to input sequentially. 
*/
void inputPillInfo(void)
{
	addDays(); // Add days current pill is taken. 
	addHours(); // Add hours current pill is taken each day. 
	addPillCount(); // Add the count of current pill filled.
	printf("\n\n");
	return; 
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
// Asks the user to enter the number of pills loaded.
void addPillCount(void)
{
	// Prompt for input.
	int tempCount;
	printf("Enter the number of pills loaded: ");
	scanf("%d", &tempCount);
	getchar(); // Clear the input line.
	// Store input in current pill index.
	pillCounts[currentPill-1][1] = tempCount;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
/*
Asks the user to enter the hours the current pill needs to be dispensed
every day. Updates the dispHourPill row coresponding to the current 
pill. 
*/
void addHours(void)
{
	// Prompt for input. Allows multiple inputs at once.
	printf("Enter the hours to dispense this pill each day separated by spaces.");
	printf("\nUse 24 hour clock time (0-23). End with an 'x'.: ");
	int i = 0, tempHours[24] = {0};
	
	while(scanf("%d", &tempHours[i]))
	{
		i++;
	}
	getchar(); // clear the input line 
	
	/*
	Go through the input and update the corresponding index in a temp
	updateHours[] array. 
	*/
	int j = 0, updateHours[24] = {0};
	i = 0;
	
	while(tempHours[i] > 0) // Go through each value of the input.
	{
		/*
		If the current input value is larger than the j index, 
		increase j. j corresponds to the index in the hours array. The
		value in tempHours corresponds to the actual hour to dispense at. 
		*/
		while(tempHours[i] > j) 
		{
			j++;
		}
		updateHours[j] = 1;
		j++;
		i++;
	}
	// Update the actual dispenseHourPill array. 
	for (i = 0; i<24; i++)
	{
		dispHourPill[currentPill-1][i] = updateHours[i];
	}
		
	return;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
/*
Asks the user to enter the days the current pill needs to be dispensed.
Updates the dispDayPill row coresponding to the current pill. 
*/
void addDays(void)
{
	// Print instuctions for entering the days to dispense. 
	printf("Current pill (track #): %d\n", currentPill);
	printf("Enter the days this pill will be dispensed:\n");
	printf("\t 1: Sunday\n");
	printf("\t 2: Monday\n");
	printf("\t 3: Tuesday\n");
	printf("\t 4: Wednesday\n");
	printf("\t 5: Thursday\n");
	printf("\t 6: Friday\n");
	printf("\t 7: Saturday\n");
	
	// Prompt for input.
	printf("Enter the numbers separated by spaces. End with an 'x'.: ");

	int i = 0, tempDays[7] = {0};
	
	// Store input in a temp array. 
	while(scanf("%d", &tempDays[i]))
	{
		i++;
	}
	getchar(); // Clear the input line.
	
	int j = 1, updateDays[7] = {0};
	i = 0;
	// Go through each value in the temp array. 
	while(tempDays[i] > 0)
	{
		/*
		If the current input value is larger than the j index, 
		increase j. j corresponds to the index in the days array. The
		value in tempDays corresponds to the actual day to dispense. 
		*/
		while(tempDays[i] > j)
		{
			j++;
		}
		updateDays[j-1] = 1;
		j++;
		i++;
	}
	// Update the actual dispenseHourPill array.
	for (i = 0; i<7; i++)
	{
		dispDayPill[currentPill-1][i] = updateDays[i];
	}
	
	return;
}
//--------------------------------------------------------------------
