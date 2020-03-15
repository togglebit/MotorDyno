#ifndef SENSOR_H
#define SENSOR_H
#include "FIFOMath.h"
#include "acquisition.h"

//defines length of string array
#define STR_LNGTH 10

/**
 * rename for public access via sketch with something user friendly
 */
#define scanSensors()  cAcquire::runAcquisition ()
/**
 * again rename for user friendly access, get last known scan time, and max scan time
 */
#define scanTime()      cAcquire::getTimeSlice(false)
#define scanTimeMax()   cAcquire::getTimeSlice(true)
#define scanTimeReset() cAcquire::resetTimeSlice()

/**
 * pindef enum, to be used for setting IO mode of the pin and reading from analogs.
 * currently this is specific to for MAPLE v5 hardware
 */
enum ADC_PINS
{
  PIN_0 = 0,
  PIN_1 = 1,
  PIN_2 = 2,
  PIN_3 = 3,
  PIN_5 = 5,
  PIN_6 = 6,
  PIN_7 = 7,
  PIN_8 = 8,
  PIN_9 = 9,
  PIN_11 = 11,
  PIN_12 = 12,
  PIN_14 = 14,
  PIN_15 = 15,
  PIN_16 = 16,
  PIN_17 = 17,
  PIN_18 = 18,
  PIN_19 = 19,
  PIN_20 = 20,
  PIN_27 = 27,
  PIN_28 = 28

};


/**
 * Sensor structure that is used to create a "new" sensor. All attributes of the sensor are defined here.
 * Name, slope, offset, pin number etc. The intention is for the user to statically define these
 * in the sketch and then create a sensor class, passing a reference to "NEW_SENSOR" into the class.
 * 
 * @author DJK
 * @version 0.1
 */
struct NEW_SENSOR
{
  /**
   * string representing sensor name
   */
  char name[10];
  /**
   * string defining units
   */
  char units[10];
  /**
   * physical input pin on maple
   */
  ADC_PINS pin;
  /**
   * The sensor class assumes a linear relationship between counts and units. This is the slope.
   */
  float slope;
  /**
   * The sensor class assumes a linear relationship between counts and units. This is the offset.
   */
  float offset;
  /**
   * Each sensor acquisition(sample) is put into a FIFO buffer giving way to computations that can be performed (sum, avg, derivative, integral).
   * This determines the depth of the buffer for averaging, setting the max depth for other computaitons.
   */
  UINT8 sample_depth;
  /**
   * Depth of the derivative calculation (in # of samples)
   */
  UINT8 deriv_depth;
  /**
   * Depth of the integral calculation (in # of samples)
   */
  UINT8 integ_depth;
  /**
   * rate at which the sensor will be sampled by the acquisition scheduler.
   * (also determines time component for average, derivative and integral calculations)
   */
  ACQ_RATE rate;
};


/**
 * Sensor class, resposnible for transforming an "ADC count" into meaningful sensor data. Provides transform of counts to a floating point
 * number along with string names and units string for the sensor (engineering units). Derived from the FIFOMath class, allows for computations (avg, sum, derivative, integration)
 * on sensors based upon the last "N" (depth) samples of the input. This class is also derived from cAcquire which is a static base class that
 * can be used to schedule peroidic readings for all sensor objects (as defined by "rate" enum). A NEW_SENSOR struct reference is passed into the constructor for initialization and scheduling.
 * 
 * @see cFifoMath
 * @see cAcquire
 */
class cSensor : cFIFOMath, cAcquire 
{
private:

  void  calcLine();
  float normalize(UINT64 data);
  float normalize(UINT16 data);
  float normalize(SINT32 data);

public:
  cSensor(NEW_SENSOR *S);
  void  setX1Y1(UINT16 X1value, float Y1value);
  void  setX2Y2(UINT16 X2value, float Y2value);
  virtual void  readSensor(void);
  float getReading(bool filtered);
  float getDerivative();
  float getIntegral();
  float getSum();
  float getMax();
  float getMin();


  ACQ_RATE getRate(void);
  
protected: 

  /**
   * Y coordinates used for mapping coordinates for y = mx + b transform. Y is specified in floating eng units
   */
  float     y1,y2;
  /**
   * X coordinates used for mapping coordinates for y = mx + b transform. X is specified in ADC counts
   */
  UINT16    x1,x2;
   /**
   * slope and offset components used for linerization to engineering units
   */
  float     m,b;
  /**
  * ADC sensor data raw data in counts, last known reading
  */
  UINT16    counts;
  /**
  * floating point sensor data, last known conversion from ADC counts (last sample/avg, dt, it)
  */
  float     normalData, normalDataDt, normalDataIt;
  /**
  *  sensor units name (ASCII string) 
  */
  char units[STR_LNGTH] ;
  /**
  *  sensor name (ASCII string) 
  */
  char name[STR_LNGTH];
  /**
  * ADC pin used that the sensor is connected to 
  */
  ADC_PINS  pinNum;
  /**
  * Periodic update rate for sensor 
  */
  ACQ_RATE  rate;
};




#endif

