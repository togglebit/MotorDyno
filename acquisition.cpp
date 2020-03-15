#include "acquisition.h"
#include "sensor.h"

/**
 * Static re-declarations for cAcquire class:
 * Since the header only makes a declaration for statics...re-declare statics for memory allocation.
 * This is necessary for statics in C++
 */
UINT32 cAcquire::count;
UINT32 cAcquire::prevCount;
UINT32 cAcquire::ticks; 
UINT32 cAcquire::usTicks;
UINT16 cAcquire::_1mSCntr;
UINT16 cAcquire::_10mSCntr;
UINT16 cAcquire::_100mSCntr;
UINT16 cAcquire::_1000mSCntr;
UINT32 cAcquire::usTsliceEnd;
UINT32 cAcquire::usTslice;
UINT32 cAcquire::usTsliceMax;
cSensor* cAcquire::Sensors[MAX_NUM_SENSORS];
/**
 */
UINT8 cAcquire::senCnt;

/**
 * 
 *     
 * Constructor definition for Acquire class, initialize counter variables and static members
 * 
 */
cAcquire::cAcquire()
{

    //initialize variables
    ticks        = 0;
    usTicks      = 0;
    prevCount    = 0;
    count        = 0;
    _1mSCntr     = 0;
    _10mSCntr    = 0;
    _100mSCntr   = 0;
    _1000mSCntr  = 0;
    usTsliceMax  = 0;
    usTslice     = 0;
}


/**
 * Method used by constructor's of derived cSensor classes. Adds sensor reference to the collection of references, 
 * increments counter. Bound by "MAX_NUM_SENSORS" macro
 * 
 * @param *S - pointer to cSensor object
 */
void cAcquire::addSensor(cSensor *S)
{
    if (S)
    {
        //add sensor into collection of pointers, bounds check
        Sensors[senCnt] = S;       
        senCnt = (senCnt < MAX_NUM_SENSORS)? senCnt + 1 : MAX_NUM_SENSORS - 1; 
    }

}

/**
 * This method simply scans through class array seeking the sensors ready to run for a given rate
 * 
 * @param rate - run the readSensor() method fall all sensors of this rate
 */
void cAcquire::runRates(ACQ_RATE rate)
{
    UINT8 i;

    //scan through sensor list and read the input for the sensors corresponding "rate"
    for (i=0; i < senCnt; i++)
    {
        if (Sensors[i]->getRate() == rate)
        {
            Sensors[i]->readSensor();
        }
    }   
}


/**
 * This is the master scheduler should be run in "loop()" function, assumes tight execution to keep on schedule.
 * This method keeps track of the number of uSeconds elapsed and calls the "runRates" method for a given rate, allowing
 * for the entire list of cSensor objects to be updated (readSensor) at it's scheduled perodic rate;
 * This is a static implementation, so the one method call is needed for all....again tight loop exectuton expected.
 */
void cAcquire::runAcquisition()
{
    //sample clock to determine elapsed number of microseconds
    count = micros();

    //compensate for rollover
    ticks = (prevCount < count) ? (count - prevCount) : (0xFFFFFFFF - prevCount) + count; 

    //capture new previous count
    prevCount = count;

    //accumulate uS ticks
    usTicks += ticks;

    //detect 1mS passed
    if (usTicks >= 1000)
    {
        //clear uSec count we've detected 1ms has passed
        usTicks = 0;

        //increment 1mS tick counter
        _1mSCntr++;

        //after Xms has passed, run all xHz acquisitions based upon 1-1000ms tick counter

        //1000Hz 
        cAcquire::runRates(_1000Hz_Rate);

        //100Hz
        if ((_1mSCntr - _10mSCntr) >= 10)
        {
            cAcquire::runRates(_100Hz_Rate);
            _10mSCntr = _1mSCntr;
        }

        //10Hz
        if ((_1mSCntr - _100mSCntr) >= 100)
        {
            cAcquire::runRates(_10Hz_Rate);
            _100mSCntr = _1mSCntr;
        }

        //1Hz
        if (_1mSCntr >= 1000)
        {
            cAcquire::runRates(_1Hz_Rate);
            _1mSCntr    = 0;
            _10mSCntr   = 0;
            _100mSCntr  = 0;

        }

        //perform diagnostic timer, provides service routine timing in uSec
        usTsliceEnd = micros();
        usTslice = (count < usTsliceEnd) ? usTsliceEnd - count : (0xFFFFFFFF - count) + usTsliceEnd; 

        //latch maximum value
        usTsliceMax = usTslice > usTsliceMax ? usTslice : usTsliceMax;

    }
}

/**
* diagnostic method. Retrieves acquisition execution time in uS, diagnostics.
* 
* @param max - "true" specifies maximum seen value (latched), otherwise last measured value returned
* @return - number of uSecs elapsed during "runAcquisition" method 
*/
UINT32 cAcquire::getTimeSlice(bool max)
{
    return( max ? usTsliceMax : usTslice );
}

/**
 * resets "max" capture time returned by getTimeSlice. This is used for debugging
 */
void cAcquire::resetTimeSlice()
{
    usTsliceMax = 0;
}
