// step_PI3B_a001
// Scott Smith
// Nov 2nd 2017
// alpha 1 of merged code
// written for a PI3B

#include <pigpio.h>
#include <iostream>
#include <thread>
#include <time.h>

using namespace std;

void pulse(int steps, int highDelay, int lowDelay, int PIN);
void delayMicrosecondsNoSleep (int delay_us);
int main(int argc, char ** argv);
int turnOffStepper(short motorIndex);
int turnOnStepper(short motorIndex);
int setDirection(bool direction, short motorIndex);
int setStepping(int stepping);
int gpioSetup(void);
int cleanup(void);

// Convert BCM indexing to physical pin.

const int PIN07 = 4;  //Power Front Left
const int PIN08 = 14; // - Disable UART!
const int PIN10 = 15; // - Disable UART!
const int PIN11 = 17; //Power Front Right
const int PIN12 = 18; //Power Back Left
const int PIN13 = 27; //Power Back Right
const int PIN15 = 22; //Clock Front Left
const int PIN16 = 23; //Clock Front Right
const int PIN18 = 24; //Clock Back Left
const int PIN22 = 25; //Clock Back Right
const int PIN29 = 5;  //MS1 Global
const int PIN31 = 6;  //MS2 Global
const int PIN32 = 12; //Direction Front Left
const int PIN33 = 13; //Direction Front Right
const int PIN35 = 19; //Direction Back Left
const int PIN36 = 16; //Direction Back Right

const int FULL_STEP = 1;
const int HALF_STEP = 2;
const int THIR_STEP = 3;
const int QUAR_STEP = 4;

const int CCW = 1;
const int CW = 0;

const unsigned int HIGH = 1;
const unsigned int LOW = 0;

int main(int argc, char** argv)
{
  unsigned int stepping = 0;
  unsigned int direction = 0;
  unsigned long long totalsteps = 0;
  unsigned int highpulsewidth = 0;
  unsigned int lowpulsewidth = 0;
  char* parent;

  if(argc > 6) {
    cout<<"Too many arguments \n";
    return -1;
  }

  if(argc < 6) {
    cout<<"Too few arguments \n";
    return -1;
  }

  else {
    parent = (argv[0]);
    stepping = atoi(argv[1]);
    direction = atoi(argv[2]);
    totalsteps = atoi(argv[3]);
    highpulsewidth = atoi(argv[4]);
    lowpulsewidth = atoi(argv[5]);
  }

  int result_gpioInitialise = gpioInitialise();
  if (result_gpioInitialise < 0)
  {
    cout << "Houston, we have a problem: Cannot init GPIO Library" << endl;
	return result_gpioInitialise;
  }
  else
  {
    cout << "gpioInitialise Success!" << endl;
  }
  int result_gpioSetup = gpioSetup();
  if (result_gpioSetup < 0)
  {
    cout << "Error: " << result_gpioSetup << " " << "gpioSetup Failed." << endl;
	return result_gpioSetup;
  }
  else
  {
    cout << "gpioSetup Success!" << endl;
  }

  turnOnStepper(0);
  turnOnStepper(1);
  turnOnStepper(2);
  turnOnStepper(3);

  setStepping(stepping);

  delayMicrosecondsNoSleep(5);
  setDirection(direction, 0);
  setDirection(direction, 1);
  setDirection(direction, 2);
  setDirection(direction, 3);

  delayMicrosecondsNoSleep(5);

  thread t1_pulse(pulse, totalsteps,highpulsewidth,lowpulsewidth,PIN15);
  thread t2_pulse(pulse, totalsteps,highpulsewidth,lowpulsewidth,PIN22); 
  thread t3_pulse(pulse, totalsteps,highpulsewidth,lowpulsewidth,PIN16);
  thread t4_pulse(pulse, totalsteps,highpulsewidth,lowpulsewidth,PIN18);

  t1_pulse.join();
  t2_pulse.join();
  t3_pulse.join();
  t4_pulse.join();

  //t1_pulse.detach();
  //t2_pulse.detach();

  cout << "Turning off Steppers." << endl;

  turnOffStepper(0);
  turnOffStepper(1);
  turnOffStepper(2);
  turnOffStepper(3);

  int result_cleanup = cleanup();
  if (result_cleanup < 0)
  {
    cout << "Problem with Cleanup" << endl;
	return result_cleanup;
  }
  else
  {
	cout << "Closing Program." << endl;
	return 0;
  }
}

void delayMicrosecondsNoSleep (int delay_us)
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

int gpioSetup(void)
{
  gpioSetPullUpDown(PIN07, PI_PUD_OFF); // Clear PIN State
  gpioSetPullUpDown(PIN08, PI_PUD_OFF); // Clear PIN State
  gpioSetPullUpDown(PIN10, PI_PUD_OFF); // Clear PIN State
  gpioSetPullUpDown(PIN11, PI_PUD_OFF); // Clear PIN State
  gpioSetPullUpDown(PIN12, PI_PUD_OFF); // Clear PIN State
  gpioSetPullUpDown(PIN13, PI_PUD_OFF); // Clear PIN State
  gpioSetPullUpDown(PIN15, PI_PUD_OFF); // Clear PIN State
  gpioSetPullUpDown(PIN16, PI_PUD_OFF); // Clear PIN State
  gpioSetPullUpDown(PIN18, PI_PUD_OFF); // Clear PIN State
  gpioSetPullUpDown(PIN22, PI_PUD_OFF); // Clear PIN State
  gpioSetPullUpDown(PIN29, PI_PUD_OFF); // Clear PIN State
  gpioSetPullUpDown(PIN31, PI_PUD_OFF); // Clear PIN State
  gpioSetPullUpDown(PIN32, PI_PUD_OFF); // Clear PIN State
  gpioSetPullUpDown(PIN33, PI_PUD_OFF); // Clear PIN State
  gpioSetPullUpDown(PIN35, PI_PUD_OFF); // Clear PIN State
  gpioSetPullUpDown(PIN36, PI_PUD_OFF); // Clear PIN State

  gpioSetMode(PIN07, PI_OUTPUT); // Set GPIO-7 as output.
  gpioSetMode(PIN08, PI_OUTPUT); // Set GPIO-7 as output.
  gpioSetMode(PIN10, PI_OUTPUT); // Set GPIO-7 as output.
  gpioSetMode(PIN11, PI_OUTPUT); // Set GPIO-0 as output.
  gpioSetMode(PIN12, PI_OUTPUT); // Set GPIO-1 as output.
  gpioSetMode(PIN13, PI_OUTPUT); // Set GPIO-2 as output.
  gpioSetMode(PIN15, PI_OUTPUT); // Set GPIO-3 as output.
  gpioSetMode(PIN16, PI_OUTPUT); // Set GPIO-4 as output.
  gpioSetMode(PIN18, PI_OUTPUT); // Set GPIO-5 as output.
  gpioSetMode(PIN22, PI_OUTPUT); // Set GPIO-6 as output.
  gpioSetMode(PIN29, PI_OUTPUT); // Set GPIO-6 as output.
  gpioSetMode(PIN31, PI_OUTPUT); // Set GPIO-6 as output.
  gpioSetMode(PIN32, PI_OUTPUT); // Set GPIO-6 as output.
  gpioSetMode(PIN33, PI_OUTPUT); // Set GPIO-6 as output.
  gpioSetMode(PIN35, PI_OUTPUT); // Set GPIO-6 as output.
  gpioSetMode(PIN36, PI_OUTPUT); // Set GPIO-6 as output.

  gpioSetPullUpDown(PIN07, PI_PUD_DOWN); // Sets a pull-down.
  gpioSetPullUpDown(PIN08, PI_PUD_DOWN); // Sets a pull-down.
  gpioSetPullUpDown(PIN10, PI_PUD_DOWN); // Sets a pull-down.
  gpioSetPullUpDown(PIN11, PI_PUD_DOWN); // Sets a pull-down.
  gpioSetPullUpDown(PIN12, PI_PUD_DOWN); // Sets a pull-down.
  gpioSetPullUpDown(PIN13, PI_PUD_DOWN); // Sets a pull-down.
  gpioSetPullUpDown(PIN15, PI_PUD_DOWN); // Sets a pull-down.
  gpioSetPullUpDown(PIN16, PI_PUD_DOWN); // Sets a pull-down.
  gpioSetPullUpDown(PIN18, PI_PUD_DOWN); // Sets a pull-down.
  gpioSetPullUpDown(PIN22, PI_PUD_DOWN); // Sets a pull-down.
  gpioSetPullUpDown(PIN29, PI_PUD_DOWN); // Sets a pull-down.
  gpioSetPullUpDown(PIN31, PI_PUD_DOWN); // Sets a pull-down.
  gpioSetPullUpDown(PIN32, PI_PUD_DOWN); // Sets a pull-down.
  gpioSetPullUpDown(PIN33, PI_PUD_DOWN); // Sets a pull-down.
  gpioSetPullUpDown(PIN35, PI_PUD_DOWN); // Sets a pull-down.
  gpioSetPullUpDown(PIN36, PI_PUD_DOWN); // Sets a pull-down

  return 0;
}

int setDirection(bool direction, short motorIndex)
{
  cout << "Setting Direction" << endl;

  if (motorIndex == 0)
  {
	if (direction == 0) { gpioWrite(PIN32, LOW); }
	else { gpioWrite(PIN32, HIGH); }
	return 0;
  }

  if (motorIndex == 1)
  {
	if (direction == 0) { gpioWrite(PIN33, LOW); }
	else { gpioWrite(PIN33, HIGH); }
	return 0;
  }

  if (motorIndex == 2)
	{
	  if (direction == 0) { gpioWrite(PIN35, LOW); }
	  else { gpioWrite(PIN35, HIGH); }
	  return 0;
	}
	if (motorIndex == 3)
	{
	  if (direction == 0) { gpioWrite(PIN36, LOW); }
	  else { gpioWrite(PIN36, HIGH); }
	  return 0;
	}
	return 0;
}

int setStepping(int stepping)
{
  if (stepping == FULL_STEP)
  {
	cout << "Setting Full Stepping" << endl;
	gpioWrite(PIN29, HIGH);
	gpioWrite(PIN31, HIGH);
	}
	if (stepping == HALF_STEP)
	{
		cout << "Setting Half Stepping" << endl;
		gpioWrite(PIN29, LOW);
		gpioWrite(PIN31, HIGH);
	}
	if (stepping == THIR_STEP)
	{
		cout << "Setting Third Stepping" << endl;
		gpioWrite(PIN29, HIGH);
		gpioWrite(PIN31, LOW);
	}
	if (stepping == QUAR_STEP)
	{
		cout << "Setting Quarter Stepping" << endl;
		gpioWrite(PIN29, LOW);
		gpioWrite(PIN31, LOW);
	}
	return 0;
}

void pulse(long long int totalsteps, int highpulsewidth, int lowpulsewidth, int PIN)
{
	while(steps>0)
	{
		gpioWrite(PIN, HIGH);
		delayMicrosecondsNoSleep(highpulsewidth);
		gpioWrite(PIN, LOW);
		delayMicrosecondsNoSleep(lowpulsewidth);
		totalsteps--;
	}
}

int turnOnStepper(short motorIndex)
{
	if(motorIndex == 0)
	{
		gpioWrite(PIN07, LOW); //FL
	}

	if(motorIndex == 1)
	{
		gpioWrite(PIN11, LOW); //FR
	}

	if(motorIndex == 2)
	{
		gpioWrite(PIN12, LOW); //BL
	}

	if(motorIndex == 3)
	{
		gpioWrite(PIN13, LOW); //BR
	}

	return 0;
}

int turnOffStepper(short motorIndex)
{
	if(motorIndex == 0)
	{
		gpioWrite(PIN07, HIGH);
	}

	if(motorIndex == 1)
	{
		gpioWrite(PIN11, HIGH);
	}

	if(motorIndex == 2)
	{
		gpioWrite(PIN12, HIGH);
	}

	if(motorIndex == 3)
	{
		gpioWrite(PIN13, HIGH);
	}

	return 0;
}

int cleanup(void)
{
	//gpioWrite(PIN08, LOW);
	//gpioWrite(PIN10, LOW);
	//gpioWrite(PIN15, LOW);
	//gpioWrite(PIN16, LOW);
	//gpioWrite(PIN18, LOW);
	//gpioWrite(PIN22, LOW);
	//gpioWrite(PIN29, LOW);
	//gpioWrite(PIN31, LOW);
	//gpioWrite(PIN32, LOW);
	//gpioWrite(PIN33, LOW);
	//gpioWrite(PIN35, LOW);
	//gpioWrite(PIN36, LOW);

	gpioWrite(PIN07, LOW);
	gpioWrite(PIN11, LOW);
	gpioWrite(PIN12, LOW);
	gpioWrite(PIN13, LOW);

	gpioSetMode(PIN07, PI_INPUT);
	gpioSetMode(PIN11, PI_INPUT);
	gpioSetMode(PIN12, PI_INPUT);
	gpioSetMode(PIN13, PI_INPUT);

	gpioSetPullUpDown(PIN08, PI_PUD_OFF); // Clear PIN State
	gpioSetPullUpDown(PIN10, PI_PUD_OFF); // Clear PIN State
	gpioSetPullUpDown(PIN15, PI_PUD_OFF); // Clear PIN State
	gpioSetPullUpDown(PIN16, PI_PUD_OFF); // Clear PIN State
	gpioSetPullUpDown(PIN18, PI_PUD_OFF); // Clear PIN State
	gpioSetPullUpDown(PIN22, PI_PUD_OFF); // Clear PIN State
	gpioSetPullUpDown(PIN29, PI_PUD_OFF); // Clear PIN State
	gpioSetPullUpDown(PIN31, PI_PUD_OFF); // Clear PIN State
	gpioSetPullUpDown(PIN32, PI_PUD_OFF); // Clear PIN State
	gpioSetPullUpDown(PIN33, PI_PUD_OFF); // Clear PIN State
	gpioSetPullUpDown(PIN35, PI_PUD_OFF); // Clear PIN State
	gpioSetPullUpDown(PIN36, PI_PUD_OFF); // Clear PIN State

	delayMicrosecondsNoSleep(100);
	gpioTerminate();

	return 0;
}