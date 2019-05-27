// singleStep_JTXNano_a001
// Scott Smith
// May 23rd
// alpha 1
// written for a Jetson TX Nano

#include <iostream>
#include <time.h>
#include "jetsonGPIO.h" // Using modified source -> To compile using my header: git clone https://github.com/mrgibbes/jetsonTX1GPIO.git

using namespace std;

//Tells the compiler to use the jetsonTX2GPIONumber enum in jetsonGPIO.h.  I am also defining PIN?? to match the actual pin on the 40 pin header counting from the first Ground pin.
jetsonTXNanoGPIONumber PIN19 = gpio16; //Clock
jetsonTXNanoGPIONumber PIN21 = gpio17; //Turn Stepper on or off( !ON/OFF )
jetsonTXNanoGPIONumber PIN22 = gpio13; //MS1
jetsonTXNanoGPIONumber PIN23 = gpio18; //MS2
jetsonTXNanoGPIONumber PIN24 = gpio19; //Direction

const int FULL_STEP = 1;
const int HALF_STEP = 2;
const int THIR_STEP = 3;
const int QUAR_STEP = 4;

const int CCW = 1;
const int CW = 0;

void pulse(long long int totalsteps, unsigned int highDelay, unsigned int lowDelay, jetsonTXNanoGPIONumber PIN);
void delayMicrosecondsNoSleep (unsigned int delay_us);
int main(int argc, char ** argv);
int turnOffStepper(void);
int turnOnStepper(void);
int setDirection(bool direction);
int setStepping(unsigned int stepping);
int gpioSetup(void);
int cleanup(void);

int main(int argc, char** argv)
{
  unsigned int stepping = 0;
  unsigned int direction = 0;
  long long totalsteps = 0;
  unsigned int highpulsewidth = 0;
  unsigned int lowpulsewidth = 0;
  char* parent;

  if(argc > 6) {
    cout<<"Too many arguments"<<endl;
    return -1;
  }

  if(argc < 6) {
    cout<<"Too few arguments"<<endl;
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

  turnOnStepper();

  setStepping(stepping);

  delayMicrosecondsNoSleep(5);

  setDirection(direction);

  delayMicrosecondsNoSleep(5);

  pulse(totalsteps,highpulsewidth,lowpulsewidth,PIN19);

  cout << "Turning off Stepper." << endl;

  turnOffStepper();

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

void delayMicrosecondsNoSleep (unsigned int delay_us)
{
  long int start_time;
  long int time_difference;
  struct timespec gettime_now;

  clock_gettime(CLOCK_REALTIME, &gettime_now);
  start_time = gettime_now.tv_nsec;               //Get nS value
  while (true)
  {
    clock_gettime(CLOCK_REALTIME, &gettime_now);
    time_difference = gettime_now.tv_nsec - start_time;
    if (time_difference < 0)
      time_difference += 1000000000;              //(Rolls over every 1 second)
    if (time_difference > (delay_us * 1000))      //Delay for # nS
      break;
  }
}

int gpioSetup()
{
  int result_gpioSetup;

  //Remove the requested PIN from userspace by modifying /sys/class/gpio/export
  if ((result_gpioSetup = gpioExport(PIN19)) != 0) {return result_gpioSetup;}
  if ((result_gpioSetup = gpioExport(PIN21)) != 0) {return result_gpioSetup;}
  if ((result_gpioSetup = gpioExport(PIN22)) != 0) {return result_gpioSetup;}
  if ((result_gpioSetup = gpioExport(PIN23)) != 0) {return result_gpioSetup;}
  if ((result_gpioSetup = gpioExport(PIN24)) != 0) {return result_gpioSetup;}

  delayMicrosecondsNoSleep(5);

  cout << "Setting direction as output" << endl;

  if ((result_gpioSetup = gpioSetDirection(PIN19,outputPin)) != 0) {return result_gpioSetup;}
  if ((result_gpioSetup = gpioSetDirection(PIN21,outputPin)) != 0) {return result_gpioSetup;}
  if ((result_gpioSetup = gpioSetDirection(PIN22,outputPin)) != 0) {return result_gpioSetup;}
  if ((result_gpioSetup = gpioSetDirection(PIN23,outputPin)) != 0) {return result_gpioSetup;}
  if ((result_gpioSetup = gpioSetDirection(PIN24,outputPin)) != 0) {return result_gpioSetup;}

  delayMicrosecondsNoSleep(5);
}

int turnOnStepper(void)
{
  gpioSetValue(PIN21, low);
}

int turnOffStepper(void)
{
  gpioSetValue(PIN21, high);
}

int setStepping(unsigned int stepping)
{
  if (stepping == FULL_STEP)
  {
    cout << "Setting Full Stepping" << endl;
    gpioSetValue(PIN22, high);
    gpioSetValue(PIN23, high);

  if (stepping == HALF_STEP)
  {
    cout << "Setting Half Stepping" << endl;
    gpioSetValue(PIN22, low);
    gpioSetValue(PIN23, high);
  }

  if (stepping == THIR_STEP)
  {
    cout << "Setting Third Stepping" << endl;
    gpioSetValue(PIN22, high);
    gpioSetValue(PIN23, low);
  }

  if (stepping == QUAR_STEP)
  {
    cout << "Setting Quarter Stepping" << endl;
    gpioSetValue(PIN22, low);
    gpioSetValue(PIN23, low);
  }
  return 0;
}

int setDirection(bool direction)
{
  if (direction == 0) { gpioSetValue(PIN24, low); }
    else { gpioSetValue(PIN24, high); }
      return 0;
}

void pulse(long long int totalsteps, unsigned int highpulsewidth, unsigned int lowpulsewidth, jetsonTXNanoGPIONumber PIN)
{
  while(totalsteps>0)
  {
    gpioSetValue(PIN, high);
    delayMicrosecondsNoSleep(highpulsewidth);
    gpioSetValue(PIN, low);
    delayMicrosecondsNoSleep(lowpulsewidth);
    totalsteps--;
  }
}

int cleanup(void)
{
  gpioUnexport(PIN19);
  gpioUnexport(PIN21);
  gpioUnexport(PIN22);
  gpioUnexport(PIN23);
  gpioUnexport(PIN24);

  return 0;
}
