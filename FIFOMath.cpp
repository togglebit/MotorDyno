#include "FIFOMath.h"

/**
 * Constructor for FIFOMath class. Here we initialize the depths and indicies for average, derivative and math computaitons
 * 
 * @param avgLength - Length in samples to be used for sum, average and max, min computations indexed into FifoArray[]. This must be less than MAX_FIFO_SIZE.
 * @param dtLength  - Length in samples to be used for deravitive computations indexed into FifoArray[]. This must be equal or less than avgLength.
 * @param itLength  - Length in samples to be used for integral computations indexed into FifoArray[]. This must  be equal or less than avgLength.
 */
cFIFOMath::cFIFOMath(UINT8 avgLength, UINT8 dtLength, UINT8 itLength )
{
    //set default max buffer size, and requested length
    maxLen = MAX_FIFO_SIZE; 

    //set buffer indicies
    depth = avgLength;
    dtDepth  = dtLength;
    itDepth  = itLength;

    //clip and set default depth to 1 if not specified
    depth = (depth > 0 && depth < maxLen+1) ? depth : 1 ;

    //It,dt members can be set to 0 for "disable" of calcuations, must be clipped to "depth" since fifo data is shared
    dtDepth  = ( dtDepth < depth+1) ? dtDepth : 0 ;
    itDepth  = ( dtDepth < depth+1) ? itDepth : 0 ;

    //initialize members
    //"tail" always points to the very oldest sample
    tail = depth - 1;

    //"head" always points to the very newest sample
    head = 0; 
    sum  = 0;
    avg  = 0;
    max  = 0;
    min  = 0xFFFF;

    //dt members, dtTail index is defined by tail index of buffer - (dtDepth - 1) for indexing. 
    //the dt/itTails points to the "nth" oldest sample
    dtTail = tail - (dtDepth - 1);
    
    //It members, ItTail index is defined by tail index of buffer - (itDepth - 1) for indexing. 
    //the dt/itTails points to the "nth" oldest sample
    itTail = tail - (itDepth - 1);
    sumIt  = 0;
    derivN = 0; 
    integN = 0;
}

/** 
 * The update function is responsible for managing the Fifo data, indicices and computation of: average, sum, derivative
 * integral, max and min data. Average, sum, max and min are based upon the same depth indexed into the fifo. 
 * Derivative and integral calcuations are based upon thier own depth indexed into the fifo.
 * Note that all computations are based upon U16 data and U32 results to optimize for speed and memory. It is assumed 
 * that floating point computations would be applied at the next layer up.
 *
 * Integer based integration and derivative calculations are performed. The floating point timescale can be applied 
 * at the next layer up
 * 
 * where:
 * 
 *     derivN = sample1 - sample2 / Nsamples
 * 
 *     The floating point derivative is then calculated by  di/dt =  derivN / t (where t is units of seconds)
 * 
 *    integN  = (sample[0] + sample[1].......sample[N]) for Nsamples (a running FIFO sum over N samples without a timescale applied)
 * 
 *    The floating point integral is calculated by  It =  integN * t (where t is units of seconds)
 * 
 *  Note: The maximum and minimum values that are latched are that of the "data" (all samples unitl reset) and not necesarily what is in the FIFO
 * 
 * @param data - new data for entry into the fifo
 */
void cFIFOMath::update(UINT16 data)
{

    //perform sum and average calculations 
    if (updateCalls >= depth)
    {
        //buffer is full of samples, pop tail off (head location now points at oldest sample)
        sum -= FifoArray[head];
        sum += data;
        avg = (UINT16)(sum/depth);


    } else
    {
        //the buffer is not yet full of samples, perform avg on samples collected so far
        updateCalls++;
        sum+=data;
        avg = (UINT16)(sum/updateCalls);
    }

    //perform derivatve calculation 
    if (dtDepth)
    {
        //wait for appropriate # of samples accumulated for deriv
        if (updateCalls >= dtDepth)
        {
            derivN = (FifoArray[dtTail] - data) / dtDepth; 
        }
    }

    //perform integral calculation 
    if (itDepth)
    {
        //sum samples
        sumIt += data;
        
        //wait for appropriate # of samples accumulated, pop off tail value 
        if(updateCalls >= itDepth) 
        {
            sumIt -= FifoArray[itTail];
            integN = sumIt;
        }
    }

    //insert new data into fifo
    FifoArray[head] = data;

    //update max/min values acquired (may not necessarily in the buffer any longer)
    max = data > max ? data : max; 
    min = data < min ? data : min; 

    //update head & tail indicies
    tail = (tail >= (depth - 1)) ? 0 : tail + 1;
    head = (head >= (depth - 1)) ? 0 : head + 1; 
    //dt
    dtTail = (dtTail >= (depth - 1)) ? 0 : dtTail + 1;
    //it
    itTail = (itTail >= (depth - 1)) ? 0 : itTail + 1;

}
