//Johnny 5 - Manual Drive System.  Utilizing WiringPi over MPI to allow for High Availability (HA) and Load Sharing.
//j5drive VS 1.0 Alpha - written 1-1-19 on the RPI3-B+ and BPI-R1
//mpic++ -std=c++11 -o j5drive j5drive.cpp -lpthread -lwiringPi -lrt -lcrypt -lserial

//#include <wiringSerial.h> //Only use if planning on utilizing UART on the RPIvB3+
#include <SerialStream.h>   //Being utilized for the BPI-R1 only
#include <wiringPi.h>
#include <stdio.h>
#include <thread>
#include <time.h>
#include <unistd.h>
#include <mpi.h>
#include <string>

//So you don't have to put STD:: in front onf every function linked to the Standard Library  IE: STD::thread
using namespace std;
using namespace LibSerial;

//NODE A/B 0=A 1=B 2=C 3=failover - Function 0=Stepper Driver 1=Manual Controller 2=AI(Automatic Controller)
int NODEFunction = 5;
int NODE = 5;

//Used for human readability later in the code.
const int FULL_STEP = 1;
const int HALF_STEP = 2;
const int THIR_STEP = 3;
const int QUAR_STEP = 4;

//Used for human readability later in the code.
const int CCW = 1;
const int CW = 0;

//Used for human readability later in the code.
//const unsigned int HIGH = 1; already defined in WiringPi
//const unsigned int LOW = 0; already defined in WiringPi

//Used for human readability later in the code.
//v3B+ Physical PIN to WiringPi indexing - Gathered by running gpio readall              NODE P/S
const int PIN07 = 7;  //BCM04 WiringPi - 07 GPIO 07 Pi-EZConnect GPIO 04  CLK FL         NODE A/B
const int PIN11 = 0;  //BCM17 WiringPi - 00 GPIO 00 Pi-EZConnect GPIO 17  CLK FR         NODE A/B
const int PIN12 = 1;  //BCM18 WiringPi - 01 GPIO 01 Pi-EZConnect GPIO 18  CLK BL         NODE B/A
const int PIN13 = 2;  //BCM27 WiringPi - 02 GPIO 02 Pi-EZConnect GPIO 27  CLK BR         NODE B/A
const int PIN15 = 3;  //BCM22 WiringPi - 03 GPIO 03 Pi-EZConnect GPIO 22  MS1 FL         NODE A/B
const int PIN16 = 4;  //BCM23 WiringPi - 04 GPIO 04 Pi-EZConnect GPIO 23  MS1 FR         NODE A/B
const int PIN18 = 5;  //BCM24 WiringPi - 05 GPIO 05 Pi-EZConnect GPIO 24  MS1 BL         NODE B/A
const int PIN22 = 6;  //BCM25 WiringPi - 06 GPIO 06 Pi-EZConnect GPIO 25  MS1 BR         NODE B/A
const int PIN29 = 21; //BCM05 WiringPi - 21 GPIO 21 Pi-EZConnect GPIO 05  MS2 FL         NODE A/B
const int PIN31 = 22; //BCM06 WiringPi - 22 GPIO 22 Pi-EZConnect GPIO 06  MS2 FR         NODE A/B
const int PIN32 = 26; //BCM12 WiringPi - 26 GPIO 26 Pi-EZConnect GPIO 12  MS2 BL         NODE B/A
const int PIN33 = 23; //BCM13 WiringPi - 23 GPIO 23 Pi-EZConnect GPIO 13  MS2 BR         NODE B/A
const int PIN35 = 24; //BCM19 WiringPi - 24 GPIO 24 Pi-EZConnect GPIO 19  DIR FL         NODE A/B
const int PIN36 = 27; //BCM16 WiringPi - 27 GPIO 27 Pi-EZConnect GPIO 16  DIR FR         NODE A/B
const int PIN37 = 25; //BCM26 WiringPi - 25 GPIO 25 Pi-EZConnect GPIO 26  DIR BL         NODE B/A
const int PIN38 = 28; //BCM20 WiringPi - 28 GPIO 28 Pi-EZConnect GPIO 20  DIR BR         NODE B/A
const int PIN40 = 29; //BCM21 WiringPi - 29 GPIO 29 Pi-EZConnect GPIO 21  PWR COMMON     NODE A&B

//Used as a GLOBAL shared variable to determine if the motors should be ON or OFF at any given time.
//!!Need to make MPI Broadcast and set local
bool ShutDown = false;
bool PowerSteppers = false;
bool SteppersPoweredOff = true;

//Used as a GLOBAL shared variable to determine if diagnostics have failed at any given time.
//!!Diagnostic needs to be able to run locally to set these variables.  Broadcast elsewhere with other variables
bool RunDiagnostic = false; //Broadcast
bool DiagnosticRan = false;
bool DiagnosticPassed = true;
int  DiagnosticErrorHash = 0;

//Used as a GLOBAL shared variable to determine if failover is required at any time. Per Node.  No need to broadcast?
bool FailoverForOtherNode = false;

//Used as a GLOBAL shared variable to determine if autopilot is required at any time.
//!!Need to make MPI Broadcast and set local
bool ManualControlDetermined = false;
bool ManualControl = false;
bool ReadSerialInput = false;

//MPI Global Variables
//Keep in mind.  These be local!
int world_size;
int world_rank;
int name_len;
char processor_name[MPI_MAX_PROCESSOR_NAME];

void delayMicrosecondsNoSleep(int delay_us)
{
	long int start_time;
	long int time_difference;
	struct timespec gettime_now;

	clock_gettime(CLOCK_REALTIME, &gettime_now);
	start_time = gettime_now.tv_nsec;                   //Get nS value
	while (true)
	{
		clock_gettime(CLOCK_REALTIME, &gettime_now);
		time_difference = gettime_now.tv_nsec - start_time;
		if (time_difference < 0)
			time_difference += 1000000000;              //(Rolls over every 1 second)
		if (time_difference > (delay_us * 1000))        //Delay for # nS
			break;
	}
}

void determineNode(void)
{
	if (strcmp("JohnnyR",processor_name) == 0)
	{
		NODEFunction = 0;
		NODE = 0;
	}

	if (strcmp("JohnnyPiOne", processor_name) == 0)
	{
		NODEFunction = 1;
		NODE = 0;
	}

	if (strcmp("JohnnyPiTwo", processor_name) == 0)
	{
		NODEFunction = 1;
		NODE = 1;
	}

	if (NODEFunction == 1 && FailoverForOtherNode == true)
	{
		NODE = 3;
	}

	cout << processor_name << " NODE:" << NODE << " NODEFuntion:" << NODEFunction << "\n";
}

void runDiagnostic(void)
{
	DiagnosticErrorHash = 1;
	cout << "Diagnostic Loop Running\n";
	do
	{
		if (RunDiagnostic == true)
		{
			try
			{
				if (true)
				{
					DiagnosticErrorHash = 0;
					DiagnosticPassed = true;
					DiagnosticRan = true;
				}
			}
			catch(int erroDiagnostic)
			{
				cout << "We fail J5!." << DiagnosticErrorHash << "\n";
				DiagnosticRan = true;
				DiagnosticPassed = false;
			}
		}

		DiagnosticErrorHash = 0; //Remove Me!!!!
		RunDiagnostic = false;

		sleep(5); //Sleep 5 second

	} while (ShutDown == false);
}

void listenForControllerInput(void)
{
	if (NODEFunction == 0)
	{
		cout << processor_name << " Listening for Controller Input.\n";

		int result = 0;

		const int BUFFER_SIZE = 256;
		char input_buffer[BUFFER_SIZE];
		//char next_char; 

		//Initialize Serial Interface on the BPI-R1
		try
		{
			SerialStream serialBPI;

			serialBPI.Open("/dev/ttys2");
			serialBPI.SetBaudRate(SerialStreamBuf::BAUD_115200);
			serialBPI.SetCharSize(SerialStreamBuf::CHAR_SIZE_8);
			serialBPI.SetNumOfStopBits(STOP_BITS_1);
			serialBPI.SetParity(SerialStreamBuf::PARITY_ODD);
			serialBPI.SetFlowControl(SerialStreamBuf::FLOW_CONTROL_HARD);
		}
		catch (int initSerialResult)
		{
			cout << "Could Not Initialize Serial On: " << processor_name << "\n";
		}

		do
		{
			int count = 0;
			bool serialReceived = false;

			if (serialBPI.IsDataAvailable() == true) //Attempt to align read to serial input
			{
				serialBPI.Flush();
				usleep(2000);
				serialBPI.Flush();
			}

			while (count <= 3 && serialReceived == false) //Wait to see if RC Controller is ON - If YES do stuff
			{
				if (serialBPI.IsDataAvailable() == true)
				{
						serialBPI.read(input_buffer, BUFFER_SIZE); //Read a Block
						serialReceived = true;
						//serialBPI.read(next_char); //Read one character - loop
				}

				count++;
				usleep(2500);  //More alignment stuff

				//!!Need to Broadcast and set locally on each node.
				ManualControlDetermined = true;
			}

			sleep(10);
		} while(ShutDown == false)
	}

	serialBPI.Close();

	//!!Need to Broadcast and set locally on each node. - Remove!
	ManualControlDetermined = true;
}

void turnOnStepperMotors(void)
{
	cout << "Stepper Power Loop Running.\n";
	pinMode(PIN40, OUTPUT); //Rquires root on BPI-R1: WTF?

	do
	{
		if (PowerSteppers == true)
		{
			try //Turn ON Steppers
			{
				digitalWrite(PIN40,HIGH); //Should do this high once?
				SteppersPoweredOff = false;
			}
			catch(int errorStepperPower)
			{
				SteppersPoweredOff = true;
				cout << "Crap, Can't write to PIN40. No turning ON the stepper\n";
			}
		}
		else
		{
			try //Turn OFF Steppers
			{
				digitalWrite(PIN40,LOW); //Should do this low once?
				SteppersPoweredOff = true;
			}  
			catch(int errorStepperPower)
			{
				SteppersPoweredOff = false;
				cout << "Crap, Can't write to PIN40. No turning OFF the Stepper\n";
			}
		}
		sleep(3); //Sleep 3 seconds

	} while (ShutDown == false);
}

//Sqaurewave Generator
void pulse(long long int totalpulse, int highpulsewidth, int lowpulsewidth, int PIN)
{
	pinMode(PIN, OUTPUT);

	while (totalpulse > 0)
	{
		digitalWrite(PIN,HIGH);
		delayMicrosecondsNoSleep(highpulsewidth);
		digitalWrite(PIN,LOW);
		delayMicrosecondsNoSleep(lowpulsewidth);
		totalsteps--;
	}
}

void cleanup()
{
	cout << "Cleaning Up.\n";

	RunDiagnostic = false;
	PowerSteppers = false;
	ReadSerialInput = false;
	ShutDown = true;

	//Return PIN's to previous state.

	//Wait for threads to close themselves gracefully. - need to do this better IE: lock
	sleep(1);  //!!because I am lazy IE: No Lock
}

//Parent || bool RunDiagnostic bool ReadSerialInput bool PowerSteppers
int main(bool rd, bool rsi, bool ps)
{
	RunDiagnostic = rd;
	ReadSerialInput = rsi;
	PowerSteppers = ps;

	//Initialize MPI and determine Node
	try 
	{
		//Initialize the MPI environment
		MPI_Init(NULL, NULL);
		//Get the number of processes
		MPI_Comm_size(MPI_COMM_WORLD, &world_size);
		//Get the rank of the process
		MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
		//Get the name of the processor
		MPI_Get_processor_name(processor_name, &name_len);
	}
	catch(int errorMain)
	{
		cout << "Could not set up MPI, investigate you douche: " << "\n";
	}

	//Determine if NODE is A or B
	determineNode();

	//Initialize WireingPi
	try
	{
		if (NODEFunction == 1) { wiringPiSetup(); }
	}
	catch(int errorMain)
	{
		cout << "Could not set up WiringPi, investigate you douche: " << "\n";
	}

	//Run Diagnostic to determin if any NODE's are not communicating or are detecing issues.
	try
	{
		thread t1_runDiagnostic(runDiagnostic);
		t1_runDiagnostic.detach();
	}
	catch(int errorMain)
	{
		cout << "Diagnostic ran into an error before Steppers could be turned ON, See Error: " << DiagnosticErrorHash << "\n";
	}

	//Wait for the Diagnostic
	if (RunDiagnostic == true) {
		cout << "Waiting for Diagnostic\n";
	}

	while (DiagnosticRan == false && RunDiagnostic == true)
	{
		sleep(5); //Sleep 1 second
	}
	
	if (RunDiagnostic == true) {
		cout << "Diagnostic Complete\n";
	}

	if (ReadSerialInput == true)
	{
		thread t1_listenForControllerInput(listenForControllerInput);
		t1_listenForControllerInput.detach();
	}

	//Spawn a Thread to turn ON motors if specified by the RC handheld Controller
	if (DiagnosticPassed == true)
	{
		thread t1_turnOnStepperMotors(turnOnStepperMotors);
		t1_turnOnStepperMotors.detach();
	}

	while (ManualControlDetermined == false)
	{
		sleep(1); //Sleep 1 second
	}

	//MAIN LOOP!!!
	while (ManualControl == true)
	{
		//Do stuff with data collected from listenForControllerInput()
	}

	//Try to close threads nicely and cleanup.
	cleanup();

    //Finalize the MPI environment.
	MPI_Finalize();

	return 0;
}

//pinMode(PIN, OUTPUT);
//digitalWrite(PIN, 1);
//wiringPiSetupSys(void);

/*
void pullUpDnControl (int pin, int pud) ;
This sets the pull-up or pull-down resistor mode on the given pin, which should be set as an input.
Unlike the Arduino, the BCM2835 has both pull-up an down internal resistors.
The parameter pud should be; PUD_OFF, (no pull up/down), PUD_DOWN (pull to ground) or PUD_UP (pull to 3.3v).
The internal pull up/down resistors have a value of approximately 50KΩ on the Raspberry Pi.

This function has no effect on the Raspberry Pi’s GPIO pins when in Sys mode.
If you need to activate a pull-up/pull-down, then you can do it with the gpio program in a script before you start your program.
*/

/*
#include <wiringSerial.h>
int   serialDataAvail(int fd);
Returns the number of characters available for reading, or -1 for any error condition, in which case errno will be set appropriately.

int serialGetchar(int fd);
Returns the next character available on the serial device.This call will block for up to 10 seconds if no data is available(when it will return -1)

void serialFlush(int fd);
This discards all data received, or waiting to be send down the given device.

int serialOpen (char *device, int baud) ;
This opens and initialises the serial device and sets the baud rate. It sets the port into “raw” mode (character at a time and no translations), and sets the read timeout to 10 seconds. The return value is the file descriptor or -1 for any error, in which case errno will be set as appropriate.

void serialClose (int fd) ;
Closes the device identified by the file descriptor given.
*/

//Rotation: %(as decimal) Accesleration %(as decimal) Direction(1 FWD or 0 BKW)
