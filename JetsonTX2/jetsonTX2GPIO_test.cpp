#include <iostream>
#include <time.h>
#include "jetsonGPIO.h" // Using modified source -> To compile using my header: git clone https://github.com/mrgibbes/jetsonTX1GPIO.git

using namespace std;

void delayMicrosecondsNoSleep (int delay_us);

int main (int argc, char **argv)
{
	//int to store GPIO pin value 0 or 1 while testing.
    unsigned int value = 15;

    //tells the compiler to use the jetsonTX2GPIONumber enum in jetsonGPIO.h.  I am also defining PIN?? to match the actual pin on the 40 pin header couting from the first Ground pin.
	jetsonTX2GPIONumber PIN18 = gpio481;
	jetsonTX2GPIONumber PIN29 = gpio398;
	jetsonTX2GPIONumber PIN31 = gpio298;
	jetsonTX2GPIONumber PIN33 = gpio389;
	jetsonTX2GPIONumber PIN37 = gpio388;

    //Remove the requested PIN from userspace by modifying /sys/class/gpio/export
	gpioExport(PIN18);
	gpioExport(PIN29);
	gpioExport(PIN31);
	gpioExport(PIN33);
	gpioExport(PIN37);

    delayMicrosecondsNoSleep(200);

    cout << "Setting direction as output" << endl;
	gpioSetDirection(PIN18,outputPin);
	gpioSetDirection(PIN29,outputPin);
	gpioSetDirection(PIN31,outputPin);
	gpioSetDirection(PIN33,outputPin);
	gpioSetDirection(PIN37,outputPin);

	delayMicrosecondsNoSleep(200);

    cout << "First values read as: " << endl;
	gpioGetValue(PIN18, &value);
	cout << PIN18 << ": " << value << endl;
	gpioGetValue(PIN29, &value);
	cout << PIN29 << ": " << value << endl;
	gpioGetValue(PIN31, &value);
	cout << PIN31 << ": " << value << endl;
	gpioGetValue(PIN33, &value);
	cout << PIN33 << ": " << value << endl;
	gpioGetValue(PIN37, &value);
	cout << PIN37 << ": " << value << endl;

	delayMicrosecondsNoSleep(200);

    cout << "Setting all pins low" << endl;
	gpioSetValue(PIN18,low);
	gpioSetValue(PIN29,low);
	gpioSetValue(PIN31,low);
	gpioSetValue(PIN33,low);
	gpioSetValue(PIN37,low);

    delayMicrosecondsNoSleep(200);

	cout << "Second values read as: " << endl;
	gpioGetValue(PIN18, &value);
	cout << PIN18 << ": " << value << endl;
	gpioGetValue(PIN29, &value);
	cout << PIN29 << ": " << value << endl;
	gpioGetValue(PIN31, &value);
	cout << PIN31 << ": " << value << endl;
	gpioGetValue(PIN33, &value);
	cout << PIN33 << ": " << value << endl;
	gpioGetValue(PIN37, &value);
	cout << PIN37 << ": " << value << endl;

	delayMicrosecondsNoSleep(200);

	cout << "Setting all pins high" << endl;
	gpioSetValue(PIN18,low);
	gpioSetValue(PIN29,low);
	gpioSetValue(PIN31,low);
	gpioSetValue(PIN33,low);
	gpioSetValue(PIN37,low);

	delayMicrosecondsNoSleep(200);

	cout << "Third values read as: " << endl;
	gpioGetValue(PIN18, &value);
	cout << PIN18 << ": " << value << endl;
	gpioGetValue(PIN29, &value);
	cout << PIN29 << ": " << value << endl;
	gpioGetValue(PIN31, &value);
	cout << PIN31 << ": " << value << endl;
	gpioGetValue(PIN33, &value);
	cout << PIN33 << ": " << value << endl;
	gpioGetValue(PIN37, &value);
	cout << PIN37 << ": " << value << endl;

    delayMicrosecondsNoSleep(200);

    //Return the requested PIN to userspace by modifying /sys/class/gpio/unexport
	gpioUnexport(PIN18);
	gpioUnexport(PIN29);
    gpioUnexport(PIN31);
	gpioUnexport(PIN33);
	gpioUnexport(PIN37);

	return 0;
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