
#include "FIFOMath.h"
#include "sensor.h"

/**
 * Sensor base class from which all sensors are derived from
 * 
 * @param S      sensor structure containing slope, units, etc
 */
cSensor::cSensor(NEW_SENSOR *S) : cFIFOMath(S->sample_depth,S->deriv_depth,S->integ_depth) 
{

  UINT8 i,size;

  //set simple members, name, units etc

  //check pointers for string copy
  if (S)
  {

    //set size of string
    size = sizeof(S->units);


    //clip length if needed
    size = size < STR_LNGTH? size : STR_LNGTH ; 

    for (i=0; i < size; i++)
    {
      units[i] = S->units[i];
    }

    //set size of string
    size = sizeof(S->name);


    //clip length if needed
    size = size < STR_LNGTH? size : STR_LNGTH ; 

    for (i=0; i < size; i++)
    {
      name[i] = S->name[i];
    }


    //set default slope and offset
    m =  S->slope;
    b =  S->offset;

    //set pin number for ADC read
    pinNum = S->pin;

    //set acquisition rate
    rate = S->rate;
    
#ifdef MAPLE
    //init pin mode for analog input  
    pinMode(pinNum,INPUT_ANALOG);
#else
    pinMode(pinNum,INPUT);
#endif


    //add sensor to acquisition list
    addSensor(this);
  }


}


// Set first cal pair of points for line equation
void cSensor::setX1Y1(UINT16 X1value, float Y1value)
{
  x1 = X1value;
  y1 = Y1value;
  //update coefficients, store data
  calcLine();
}

// Set second cal point for line equation
void cSensor::setX2Y2(UINT16 X2value, float Y2value)
{
  x2 = X2value;
  y2 = Y2value;
  //update coefficients, store data
  calcLine();
}



/**
 * This method is responsible for transforming coordinate pairs(x1,y1,x2,y2) into slope and offset (m,b). 
 * Called from withing the class when either pair is set by the user.
 */
void cSensor::calcLine()
{
  float denom;

  //		y2-y1
  //m = -------- point slope formula (delta equation)
  //		x2-x1

  denom = x2 - x1;

  if (denom)
  {
    m = (y2- y1) / denom;
  }
  //
  // y = mx + b slope equation, solve for offset
  // b = y1 /(m*x1)
  //

  denom = m * x1;

  if (denom)
  {
    b = y1 /denom;
  }
  //store new coefficients and sensor data to the setup file
  //StoreSetupData();
}

/**
 * apply line equation to raw data input to produce floating point result (normalData)
 * 
 * @param data - data to be transformed from raw unsigned counts to floating point
 *  
 * @return - floating point result of y=mx+b transform (sensor reading in floating point units)  
 */
float cSensor::normalize(UINT16 data)
{
  //apply line equaiton
  normalData = (data * m) + b;

  //reply with normalized data
  return(normalData);
}

//overloaded for SINT64 (integral)
float cSensor::normalize(UINT64 data)
{
  //apply line equaiton
  normalData = (data * m) + b;

  //reply with normalized data
  return(normalData);
}

//overloaded for SINT32 (derivative)
float cSensor::normalize(SINT32 data)
{
  //apply line equaiton
  normalData = (data * m) + b;

  //reply with normalized data
  return(normalData);
}


/**
 * This method is responsible for reading sensor pin rawdata, storing into class variable.
 * The captured value is then pushed into the FIFO math object. Can be over-ridden for other hardware.
 */
void cSensor::readSensor()
{

  //result will be counts 0-1024 on UNO, 4096 on DUE
  counts = analogRead(pinNum);

  //push new raw data into FIFO buffer math algorithms
  update(counts);
}

/**
 * Get the sensor reading in floating point engineering units. Apply the linearizaiton (y=mx+b) for conversion to engineering units.
 * 
 * @param filtered - TRUE = the moving average result is used for converstion to floating pont + units (based upon depth of FIFO).
 *                 - FALSE = the last known ADC reading (counts) is used
 * @return - sensor reading in floating point engineering units is returned
 */
float cSensor::getReading(bool filtered)
{
  float retVal;
  
  //get the average of the raw data from moving average FIFO, pass through the sensor transfer function "Normalize"
  if (filtered)
  {
    retVal = normalize(avg);
  } 
  else
  {
    retVal = normalize(counts);
  }
  return(retVal);
}
/**
 * Get the sensor integral in floating point engineering units. Apply the linearizaiton (y=mx+b) and timebase for conversion to engineering units.
 * The floating point integral is calculated by  It =  integN * t (where t is units of seconds)
 * 
 * @return - sensor readings integrated over time in floating point engineering units is returned
 */
float cSensor::getIntegral()
{
    //apply time base and floating point scaling to integral calculation, scale rate from uS to S
    normalDataIt = normalize(integN) * ((float)rate * 0.000001);
    
    return(normalDataIt);
}
/**
 * Get the sensor derivative in floating point engineering units. Apply the linearizaiton (y=mx+b) and timebase for conversion to engineering units.
 * The floating point derivative is calculated by  di/dt =  derivN / t (where t is units of seconds)
 * 
 * @return - sensor readings derivative in floating point engineering units is returned
 */
float cSensor::getDerivative()
{
    //apply time base and floating point scaling to derivative calculation, scale rate from uS to S
    normalDataDt = normalize(derivN) / ((float)rate * 0.000001);

    return(normalDataDt);
}

/**
 * Get the sensor sum in floating point engineering units. Apply the linearizaiton (y=mx+b) for conversion to engineering units.
 * 
 * @return - sum of sensor readings in avg compution (from FIFO) floating point engineering units is returned
 */
float cSensor::getSum()
{
    return(normalize(sum));
}

/**
 * Get the sensor Max in floating point engineering units. Apply the linearizaiton (y=mx+b) for conversion to engineering units.
 * 
 * @return - latched maximum sensor reading seen since starting acquisiton, floating point engineering units
 */
float cSensor::getMax()
{
    return(normalize(max));
}


/**
 * Get the sensor Min in floating point engineering units. Apply the linearizaiton (y=mx+b) for conversion to engineering units.
 * 
 * @return - latched minimum sensor reading seen since starting acquisiton, floating point engineering units
 */
float cSensor::getMin()
{
    return(normalize(min));
}



/**
 * Gets the acqusition rate specified for the sensor. Utilized by the base cAcquire class
 * 
 * @return - ACQ_RATE enum, representing the acquisition rate in mSecs 
 */
ACQ_RATE cSensor::getRate(void)
{
      return (rate);
}

