/*
  Synchronizes a temperature reading with LED output for the opto-thermocycler setup
  This experiment is designed to test the response to heat and red light stimuli
  individually and in combination. 
*/

#include <Adafruit_MAX31856.h>


// Initialize 
Adafruit_MAX31856 maxthermo = Adafruit_MAX31856(10, 11, 12, 13);

//constant variables
const int synchPin =  3; // synchronization LED
const int redPin = 5; // red light LED
const int greenPin = 6; // green light LED 
const long TempReadInterval = 100; // how often to read the thermometer
const double highThresh = 26.0; // threshold for triggering the start of a temperature stimulus
const long minIHI = 60000L; // minimum time before detecting another temperature increase
const double lowThresh = 23.7; // threshold for triggering the start of an experiment

// Light stimuli for experiment, allowing up to 3 light stimuli after each temperature stimulus
// The first three lists are the wait time after a heat stimulus
// No stimulus is indicated by 0L
// The next 3 lists are the durations of stimuli (5 seconds)
const double firstLEDstim [7] = {0L, 1213000L, 1L, 0L, 1L, 1L, 1213000L};
const double secondLEDstim [7] = {0L, 0L, 1213000L, 0L, 0L, 0L, 0L};
const double thirdLEDstim [7] = {0L, 0L, 0L, 0L, 0L, 0L, 0L};
const double firstLEDdur [7] = {5000L, 5000L, 5000L, 5000L, 5000L, 5000L, 5000L};
const double secondLEDdur [7] = {5000L, 5000L, 5000L, 5000L, 5000L, 5000L, 5000L};
const double thirdLEDdur [7] = {5000L, 5000L, 5000L, 5000L, 5000L, 5000L, 5000L};
//const double firstLEDstim [4] = {5000L, 2000L, 0L, 10000L}; //test stimuli
//const double secondLEDstim [4] = {0L, 10000L, 0L, 0L}; //test stimuli

// Variables:
int tempStimulus = 0; //increments each time there is a temperature stimulus
int ledState = LOW;  // ledState used to set the LED
int tempReading; // the analog reading from the sensor

// Use "unsigned long" for variables that hold time:
long previousHeatRamp = -60000L; // set large value to wait for first trigger
long previousLEDMillis = 0;   //Last time there was a light stimulus
long previousTempMillis = 0;  //Last time there was a temperature stimulus

void setup() {
  // set the LED digital pins as output:
  pinMode(synchPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(redPin, OUTPUT);
  digitalWrite(synchPin, LOW);
  digitalWrite(greenPin, LOW);
  digitalWrite(redPin, LOW);
  // set up thermocouple
  Serial.begin(115200);
  maxthermo.begin();
  maxthermo.setThermocoupleType(MAX31856_TCTYPE_T);
  maxthermo.Config(); // added line after editing the library to remove delay aspect
}

void loop() {
  //read the clock
  unsigned long currentTime = millis();
  
  //read thermometer
  if (currentTime - previousTempMillis >= TempReadInterval) {
    previousTempMillis = currentTime;
    float temperatureC = maxthermo.readThermocoupleTemperature();

    //detect heat increase stimulus or heat dip to trigger experiment
    if (temperatureC > highThresh && currentTime - previousHeatRamp > minIHI) {
      previousHeatRamp = currentTime;
      tempStimulus += 1;
    } 
    else if (temperatureC < lowThresh && currentTime - previousHeatRamp > minIHI) {
      previousHeatRamp = currentTime;
      tempStimulus += 1;
    }
    // record status to Serial monitor
    Serial.print(millis()); Serial.print(' '); Serial.print(temperatureC); Serial.print(" degrees C "); Serial.print(ledState); Serial.print(' ');
    Serial.print(tempStimulus); Serial.print(' ');
    Serial.println(currentTime - previousHeatRamp);
  }

  // control LED stimulus in response to detected heat ramps
  if (tempStimulus > 0) { // if the first heat stimulus has been detected (must start with heat stimulus to begin synchronization)
    if (firstLEDstim[tempStimulus-1] > 0) { //if there is an LED stimulus after this temperature stimulus
      //detect whether in first LED stimulus
      if (currentTime - previousHeatRamp >= firstLEDstim[tempStimulus-1] && currentTime - previousHeatRamp <= firstLEDstim[tempStimulus-1]+firstLEDdur[tempStimulus-1] ) { 
        //turn on the LED
        if (ledState == LOW) {previousLEDMillis = currentTime; ledState = HIGH; digitalWrite(synchPin, ledState); digitalWrite(redPin, ledState); }
      }
      //if there is a second LED stimulus after this temperature stimulus
      if (secondLEDstim[tempStimulus-1] > 0) { 
        //detect whether in between first and second LED stimuli
        if (currentTime - previousHeatRamp >= firstLEDstim[tempStimulus-1]+firstLEDdur[tempStimulus-1] && currentTime - previousHeatRamp <= secondLEDstim[tempStimulus-1] ) {
          //turn off the LED 
          if (ledState == HIGH) { previousLEDMillis = currentTime; ledState = LOW; digitalWrite(synchPin, ledState); digitalWrite(redPin, ledState); }
        }
          //detect whether in second LED stimulus
        else if (currentTime - previousHeatRamp >= secondLEDstim[tempStimulus-1] && currentTime - previousHeatRamp <= secondLEDstim[tempStimulus-1]+secondLEDdur[tempStimulus-1] ) {
          //turn on the LED
          if (ledState == LOW) {previousLEDMillis = currentTime; ledState = HIGH; digitalWrite(synchPin, ledState); digitalWrite(redPin, ledState); }
        }
        
        //if there is a third LED stimulus after this temperature stimulus
        if (thirdLEDstim[tempStimulus-1] > 0) { 
          //detect whether in between second and third LED stimuli
          if (currentTime - previousHeatRamp >= secondLEDstim[tempStimulus-1]+secondLEDdur[tempStimulus-1] && currentTime - previousHeatRamp <= thirdLEDstim[tempStimulus-1] ) {
            //turn off the LED 
            if (ledState == HIGH) { previousLEDMillis = currentTime; ledState = LOW; digitalWrite(synchPin, ledState); digitalWrite(redPin, ledState); }
          }
          //detect whether in third LED stimulus
          else if (currentTime - previousHeatRamp >= thirdLEDstim[tempStimulus-1] && currentTime - previousHeatRamp <= thirdLEDstim[tempStimulus-1]+thirdLEDdur[tempStimulus-1] ) {
            //turn on the LED
            if (ledState == LOW) {previousLEDMillis = currentTime; ledState = HIGH; digitalWrite(synchPin, ledState); digitalWrite(redPin, ledState); }
          }
          //detect whether after third LED stimulus
          else if (currentTime - previousHeatRamp >= thirdLEDstim[tempStimulus-1]+thirdLEDdur[tempStimulus-1] ) { 
            //turn off the LED 
            if (ledState == HIGH) { previousLEDMillis = currentTime; ledState = LOW; digitalWrite(synchPin, ledState); digitalWrite(redPin, ledState); }
          }
        }
        
        //if no third stimulus, detect whether after second LED stimulus
        else if (currentTime - previousHeatRamp >= secondLEDstim[tempStimulus-1]+secondLEDdur[tempStimulus-1] ) { 
          //turn off the LED 
          if (ledState == HIGH) { previousLEDMillis = currentTime; ledState = LOW; digitalWrite(synchPin, ledState); digitalWrite(redPin, ledState); }
        }
      }
      //if no second stimulus, detect whether after first LED stimulus
      else if (currentTime - previousHeatRamp >= firstLEDstim[tempStimulus-1]+firstLEDdur[tempStimulus-1] ) { 
        //turn off the LED 
        if (ledState == HIGH) { previousLEDMillis = currentTime; ledState = LOW; digitalWrite(synchPin, ledState); digitalWrite(redPin, ledState); }
      }
    }
  }
  //end of LED stuff
}
