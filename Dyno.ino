#include "typedef.h"
#include "sensor.h"
#define FIRMWARE_VER 0x0100

//this macro is used to "calibrate" the count down timer against the scope while servicing the protocol polling loop
//the goal is for the timer to be within 10mS
#define CALIBRATED_TIME 9


//based upon, 1.1V ADC ref, 1024 counts                  
#define DEFAULT_1_1V_SLOPE  0.001074
//based upon, 5V ADC ref, 1024 counts 
#define DEFAULT_5V_SLOPE  0.00488
//this is the number of pulses per revolution
#define PULSES_REV 10
//this is the gain for ADC counts to force in NM. 1023 = full scale ADC. Example if 10NM full scale = 0.0999nm/V =  0.00978/cnt
#define _10NM_FULLSCALE 0.00978


//SENSORS DEFINITION *******************************************************************************************************************************************************************
//
//WE CREATE A NEW SENSOR HERE  "sensor type", "units",     pin#,       slope,     			    offset,             #samples to avg,  #samples dt,  #samples it,    acquisiton rate   
//
NEW_SENSOR voltagePin0      =   {"Voltage" ,     "Volts",       PIN_0,       DEFAULT_5V_SLOPE,        0.0,                10,              1,            1,            _100Hz_Rate};
NEW_SENSOR load             =   {"Load" ,        "Nm",          PIN_0,       _10NM_FULLSCALE,                       0.0,                10,              1,            1,            _100Hz_Rate};

//
//INFORM LIBRARY: WE TELL THE SENSOR LIBRARY ABOUT OUR NEW SENSORS HERE
//
cSensor LoadVolts(&voltagePin0);
cSensor LoadTorque(&load);



//globals
UINT32 mSecs, mSecsNow, mSecsPrev,ontime,offtime;
float sensor,freq,period,rpm,power,torque;
UINT8 _1HzCtr;
bool tLED;



void measureFreq(UINT8 pinNum)
{
   ontime = pulseIn(pinNum,HIGH);
   offtime = pulseIn(pinNum,LOW);
   period = ontime+offtime;
   freq = 1000000.0/period;
}

void calcRPM()
{
    if(PULSES_REV)
        rpm =(freq/PULSES_REV)*60; 
}


void calcPowerWatts()
{
    torque = LoadTorque.getReading(false);
    power =  (torque * rpm) / 9.5488;
}



void setup() 
{

    //led output for  debug
    pinMode(13, OUTPUT);    
    //set pin7 as our digital input
    pinMode(7, INPUT);  
    //use a digital output to simulate RPM, 490Hz %50 duty
    pinMode(9, OUTPUT);  

    
    Serial.begin(9600);
    Serial.flush();

    //use 1.1V ADC reference
    //analogReference(INTERNAL);    
   

 }

void loop() 
{

    //************** start continous polling loop ***********************************************
    //run sensor tasker, must be in continus loop with minimal interruptions
    scanSensors();
    
    //simulate output
    analogWrite(9, 128);
    
    //measure frequency input on pin 7
    measureFreq(7);
    
    //poll for num mSecs elapsed (50day rollover)
    mSecsNow = millis();
    //************** end continous polling loop ***********************************************


    //************** start 100Hz polling loop ***********************************************
    if ((mSecsNow - mSecsPrev) > CALIBRATED_TIME)
    {
        //store latest timer read
        mSecsPrev = mSecsNow;


        //************** start 1Hz loop ***********************************************
        _1HzCtr+=1;
        if (_1HzCtr == 99)
        {
          //must call in this order, RPM then power
          calcRPM();
          calcPowerWatts();

          //print in this style to use serial plotter
          Serial.print(LoadVolts.getReading(false));
          Serial.print(" ");
          Serial.print(torque);
          Serial.print(" ");
          Serial.print(freq);
          Serial.print(" ");
          Serial.print(rpm);
          Serial.print(" ");
          Serial.println(power); 
            
           //flash status LED to indicate alive
          tLED = !tLED? 100 : 0;
          digitalWrite(13, tLED);
            
          //clear counter
          _1HzCtr = _1HzCtr == 99 ? 0 : _1HzCtr;

        }        
        //**************** end 1Hz polling loop  *********************************************

    }
    //**************** end 100Hz polling loop  *********************************************
}
