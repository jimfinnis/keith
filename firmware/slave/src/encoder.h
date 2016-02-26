/**
 * @file encoder.h
 * @brief  Brief description of file.
 *
 */

#ifndef __ENCODER_H
#define __ENCODER_H

// encoder gets interrupted by one of the encoder inputs, and calls tick(),
// The other pin read code should also be in the interrupt and the value
// passed to tick().

class QuadEncoder {
    volatile uint32_t negativeCt;
    volatile uint32_t ct;
    uint32_t prevTime; 
    uint32_t odoTicks;
    
    float tickFreq; //!< encoder value calculated in update()
    
    
public:
    QuadEncoder(){
        negativeCt=0;
        ct=0;
        prevTime=0;
        odoTicks=0;
    }        
    
    // update the encoder. Every now and then, see how many ticks have
    // gone by. Returns true if a new value was calculated.
    
    bool update(){
        uint32_t now = micros();
        if(now<prevTime){
            // if time ran backwards (because of integer wraparound)
            // then skip a go
            prevTime = now;
            return false;
        }
        uint32_t t = now-prevTime;
        if(t>100000){ // update every N microseconds
            prevTime = now; // update previous time
            uint32_t ict = ct;   // atomic(ish) read of ct
            float count = ict; // convert to float
            tickFreq = (count*1e6)/(float)t; // and get tick frequency
            // negate frequency if we're going in reverse
            if(negativeCt>ct/3) 
                tickFreq = -tickFreq;
            ct=0;
            negativeCt=0;
            return true;
        } else
            return false; // no update occurred
    }
    
    
    /// called from interrupt on rising edge
    void tick(bool otherPinHigh){
        ct++;
        odoTicks++;
        //check sign of other pin to get direction. If it's high,
        //we're going backwards.
        if(otherPinHigh)
            negativeCt++;
    }
    
    /// return the tick frequency
    float getTickFreq(){
        return tickFreq;
    }
    
    void resetOdometry(){
        odoTicks=0;
    }
    uint32_t getOdometry(){
        return odoTicks;
    }
};




#endif /* __ENCODER_H */
