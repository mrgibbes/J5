// singleStep_JTX2_a001
// Scott Smith
// Nov 7th 2017 - updated Nov xx
// alpha 1
// written for a Jetson TX2

#include <iostream>
#include <time.h>
#include "jetsonGPIO.h" // Using modified source -> To compile using my header: git clone https://github.com/mrgibbes/jetsonTX1GPIO.git

using namespace std;

//Tells the compiler to use the jetsonTX2GPIONumber enum in jetsonGPIO.h.  I am also defining PIN?? to match the actual pin on the 40 pin header counting from the first Ground pin.
jetsonTX2GPIONumber PIN18 = gpio481; //Clock
jetsonTX2GPIONumber PIN29 = gpio398; //Turn Stepper on or off( !ON/OFF )
jetsonTX2GPIONumber PIN31 = gpio298; //MS1
jetsonTX2GPIONumber PIN33 = gpio389; //MS2
jetsonTX2GPIONumber PIN37 = gpio388; //Direction

const int FULL_STEP = 1;
const int HALF_STEP = 2;
const int THIR_STEP = 3;
const int QUAR_STEP = 4;

const int CCW = 1;
const int CW = 0;

void pulse(long long int totalsteps, int highDelay, int lowDelay, int PIN);
void delayMicrosecondsNoSleep (int delay_us);
int main(int argc, char ** argv);
int turnOffStepper(void);
int turnOnStepper(void);
int setDirection(bool direction);
int setStepping(int stepping);
int gpioSetup(void);
int cleanup(void);

int main(int argc, char** argv)
{
  unsigned int stepping = 0;
  unsigned int direction = 0;
  unsigned long long totalsteps = 0;
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

  pulse(totalsteps,highpulsewidth,lowpulsewidth,PIN18);

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

void delayMicrosecondsNoSleep (int delay_us)
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
  if ((result_gpioSetup = gpioExport(PIN18)) != 0) {return result_gpioSetup};
  if ((result_gpioSetup = gpioExport(PIN29)) != 0) {return result_gpioSetup};
  if ((result_gpioSetup = gpioExport(PIN31)) != 0) {return result_gpioSetup};
  if ((result_gpioSetup = gpioExport(PIN33)) != 0) {return result_gpioSetup};
  if ((result_gpioSetup = gpioExport(PIN37)) != 0) {return result_gpioSetup};

  delayMicrosecondsNoSleep(5);

  cout << "Setting direction as output" << endl;

  if ((result_gpioSetup = gpioSetDirection(PIN18,outputPin)) != 0) {return result_gpioSetup};
  if ((result_gpioSetup = gpioSetDirection(PIN29,outputPin)) != 0) {return result_gpioSetup};
  if ((result_gpioSetup = gpioSetDirection(PIN31,outputPin)) != 0) {return result_gpioSetup};
  if ((result_gpioSetup = gpioSetDirection(PIN33,outputPin)) != 0) {return result_gpioSetup};
  if ((result_gpioSetup = gpioSetDirection(PIN37,outputPin)) != 0) {return result_gpioSetup};

  delayMicrosecondsNoSleep(5);
}

int turnOnStepper(void)
{
  gpioSetValue(PIN29, low);
}

int turnOffStepper(void)
{
  gpioSetValue(PIN29, high);
}

int setStepping(int stepping)
{
  if (stepping == FULL_STEP)
  {
    cout << "Setting Full Stepping" << endl;
    gpioWrite(PIN31, high);
    gpioWrite(PIN33, high);
  }

  if (stepping == HALF_STEP)
  {
    cout << "Setting Half Stepping" << endl;
    gpioWrite(PIN31, low);
    gpioWrite(PIN33, high);
  }

  if (stepping == THIR_STEP)
  {
    cout << "Setting Third Stepping" << endl;
    gpioWrite(PIN31, high);
    gpioWrite(PIN33, low);
  }

  if (stepping == QUAR_STEP)
  {
    cout << "Setting Quarter Stepping" << endl;
    gpioWrite(PIN31, low);
    gpioWrite(PIN33, low);
  }
  return 0;
}

int setDirection(bool direction)
{
  if (direction == 0) { gpioSetValue(PIN37, low); }
    else { gpioSetValue(PIN37, high); }
      return 0;
}

void pulse(long long int totalsteps, int highDelay, int lowDelay, int PIN)
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

  //gpioSetEdge(PIN18,"none");
  //gpioSetEdge(PIN18,"rising");
  //gpioSetEdge(PIN18,"falling");
  //gpioSetEdge(PIN18,"both");

  //gpioActiveLow(PIN18, 0);
  //gpioActiveLow(PIN18, 1);

  gpioUnexport(PIN18);
  gpioUnexport(PIN29);
  gpioUnexport(PIN31);
  gpioUnexport(PIN33);
  gpioUnexport(PIN37);

  return 0;
}