#ifndef FIFOMATH_H
#define FIFOMATH_H

#include "typedef.h"

#define MAX_FIFO_SIZE 100

/**
 * The FIFOMath class is an object that can be used  to compute a moving sum, moving average, derivative
 * and integral calculations on a FIFO buffer of samples. These computaitons are not floating point based, and require
 * time and unit conversion by the "next layer up" (i.e. a derived class).
 * 
 * The buffer and computations are managed by the "update" method, which would typically be called at a periodic rate. 
 *
 * @author DJK
 * @version 0.1
 */
class cFIFOMath
{
private:
    /**
    array into which newest data is pushed, and oldest data is deleted
   */
  UINT16  FifoArray[MAX_FIFO_SIZE] ;
  
  /**
  * depth = sample depths passed into constructor. Represents number of samples used for computaiotns 
  indicies for fifo, head tail, update calls 
  *
  */
  UINT8   depth, head, tail, maxLen, updateCalls;
  /**
   * depth = sample depths passed into constructor. Represents number of samples used for computaiotns 
  * dt indicies members used for derivative calculation
  */
  UINT8   dtDepth, dtHead, dtTail;
  
  /**
   * depth = sample depths passed into constructor. Represents number of samples used for computaiotns 
   * it indicies used for integral calculation
  */
  UINT8   itDepth, itHead, itTail;

public:
  
  /**
   * Class constructor, pass in FIFO lengths for computaiton.
   * 
   * @param avgLength
   * @param dtLength
   * @param itLength
   */
  cFIFOMath(UINT8 avgLength, UINT8 dtLength, UINT8 itLength );

protected:

  void update(UINT16 data);
  /**
   //running sum used for average calculation, running sum used for integral calculation
   */
  UINT64  sum, sumIt;
  /**
  //average, max, min data. Note max min are latched values of acquired, not necessairly of what is in the buffer 
  */
  UINT16  avg,max,min;
  /**
  //derivative calculation for N samples, before time scaling applied (update rate) 
  */
  SINT32  derivN; 
  /**
  //integral calculation for N samples, before time scaling applied (update rate) 
  */
  UINT64  integN; 
};

#endif 
