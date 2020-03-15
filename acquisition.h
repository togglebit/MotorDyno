#ifndef ACQ_H
#define ACQ_H
#include "typedef.h"

//defines current max numer of sensors allowed
#define  MAX_NUM_SENSORS 20


/**
    forward declare the sensor class to the base class to support circular reference
 */
class cSensor;

/**
 * This enum represents the rates at which a sensor's update funciton may be called ( data acquried, fifo math executed).
 * Untis are in uSecs
 */
enum ACQ_RATE
{
  _1000Hz_Rate  = 1000,
  _100Hz_Rate   = 10000,
  _10Hz_Rate    = 100000,
  _1Hz_Rate     = 1000000,
  NONE          = 0
};



/**
 * The acquire class is intended to act as a simple perodic acquisition scheduler for all created sensor objects. It is therefore implemented as a static base class.
 * A sensor object"s "ACQ_RATE" enum defines it's update rate, and "registers it" into the scheduler (unless NONE is specified). 
 * The Acquire class's "runAcquisition" method assumes a tight loop call (while(1)) within which it tracks elapsed time (in uSecs)
 * and runs each sensors "readSensor" method at the defined perodic rate.
 * 
 * @author DJK
 * @version v0.1
 */
class cAcquire
{
private:
    /**
     * counters used to accumulate usTicks for acquisition timing
     */
    static  UINT32 count, prevCount, ticks, usTicks;
    /**
     * counters used to determine # of mSecs that have elapsed
     */
    static UINT16 _1mSCntr, _10mSCntr, _100mSCntr, _1000mSCntr;
    
    /**
     * diagnostic timing varibles used to track the execution time of the scheduler
     */
    static UINT32 usTsliceEnd, usTslice, usTsliceMax;

    /**
     * This is the array of sensor class pointers. One entry is created each time an object is created. 
     * bound by #define macro "MAX_NUM_SENSORS"
     */
    static cSensor *Sensors[MAX_NUM_SENSORS];

    /**
     * static counter that keeps track of the number of sensors that have been created
     */
    static UINT8 senCnt;

    /**
     * This method simply scans through class array seeking the sensors ready to run for a given rate
     * 
     * @param rate - run the readSensor() method fall all sensors of this rate
     */
    static void runRates(ACQ_RATE rate);

public:

    /**
     * constructor definition for Acquire class
     */
    cAcquire();
    
    /**
     * this is the master scheduler should be run in "loop()" function, assumes tight execution to keep on schedule
     */
    static void runAcquisition();
    
    /**
     * diagnostic method. Retrieves acquisition execution time in uS, diagnostics.
     * 
     * @param max - "true" specifies maximum seen value (latched), otherwise last measured value returned
     * @return - number of uSecs elapsed during "runAcquisition" method 
     */
    static UINT32 getTimeSlice(bool max);
    
    /**
     * resets "max" capture time returned by getTimeSlice. This is used for debugging
     */
    static void resetTimeSlice();

protected:
    /**
     * Called by derived class's constructor to add sensor (pointer) to the acquisition list
     * 
     * @param S      - pointer to sensor object to be added to acquisition list
     */
    void addSensor(cSensor *S);

    
};   


#endif
