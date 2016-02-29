/**
 * @file pidMotor.h
 * @brief  Brief description of file.
 *
 */

#ifndef __PIDMOTOR_H
#define __PIDMOTOR_H

#include "motor.h"
#include "encoder.h"

/// decay factor for the motor speed (to ensure that once the motor is stopped we don't have too much current)
/// which only comes into play when the required speed is zero
#define MOTOR_SPEED_DECAY	0.92f

extern bool inException;

class PIDMotor {
    Motor m;
    QuadEncoder e;
    
    bool dirReversed; //!< motor should run backwards
    bool encReversed; //!< encoder is backwards
    
    float prevActual;
    unsigned long prevTime;
    
    /// calculate PID correction, does nothing and returns
    /// false if time wrapped around
    bool calculatePIDCorrection(){
        unsigned long now = micros();
        if(now<prevTime){
            prevTime = now;
            return false;
        }
        unsigned long micInterval = now-prevTime;
    
        float interval = ((float)micInterval) * 1.0e-6f; // interval in seconds
        
        float req = dirReversed ? -required : required;
        // get error
        error = req - actual;
        // dead zone
        if(error<0 && error>-deadZone ||
           error>0 && error<deadZone)error=0;
            
        // add to integral
        errorIntegral += error;
        errorIntegral *= integralDecay;
        
        if(errorIntegral>integralCap)
            errorIntegral=integralCap;
        if(errorIntegral<-integralCap)
            errorIntegral=-integralCap;
    
        // calculate derivative of MOTOR POSITION - see the wikipedia
        // page for why I do this rather than the more obvious error
        // deriv. Note that if the set point is the same, it amounts to
        // the same thing :)
        // NOTE THE NEGATION!
            
        errorDerivative = -(actual-prevActual);
        prevActual = actual;
            
        return true;
    }
    
public:
    
    PIDMotor(){
        pGain = 0.0f;
        iGain = 0.0f;
        dGain = 0.0f;
        errorIntegral = 0;
        integralCap=0;
        integralDecay=0;
        deadZone=0;
        actual=required=0;
    }
    
    int getOdometry(){
        return e.getOdometry();
    }
    
    void resetOdometry(){
        e.resetOdometry();
    }
    
    float getCurrent(){
        return 0; // NYI
    }
        
    
    /// set up the motor, passing in the motor control pins
    /// whether the direction should be reversed,
    /// and whether the encoder pins are backwards!
    void init(int pwm,int dir,bool drev,bool erev){
        m.init(pwm,dir);
        dirReversed = drev;
        encReversed = erev;
        prevTime = micros();
    }
    
    // tick this motor's encoder, passing in
    // the other encoder channel's state.
    void tickEncoder(bool otherChannel){
        e.tick(otherChannel);
    }
    
    /// read the encoder values and update PID if they
    // have changed
    void update(){
        if(e.update()){
            actual = e.getTickFreq();
            if(encReversed)actual = -actual;
            if(dirReversed)actual = -actual;
            
            if(calculatePIDCorrection()){
                // calculate correction
                float t = pGain*error+iGain*errorIntegral+dGain*errorDerivative;
                // apply to motor speed
                ctl+= t;
                // gradually drop the control speed if required is near zero
                if(required<0.001f && required>-0.001f)
                    ctl *= MOTOR_SPEED_DECAY; 
                // and set the motor
                if(inException)ctl=0; // to zero if we're in a bad way
                m.setSpeed(ctl);
            }
        }
    }
    
    /////////////////// motor values ///////////////////////
    
    float required,actual;
    /// integral of error for I-term calculation
    float errorIntegral;
    /// derivative of error for D-term calculation - *really* it's the position derivative!
    float errorDerivative;
    float ctl; //!< the motor control value
    
    
    /////////////////// parameters /////////////////////////
    
    /// proportional gain
    float pGain;
    /// integral gain
    float iGain;
    /// differential gain
    float dGain;
    /// error value
    float error;
    
    /// the value at which the error integral is clamped - writable
    float integralCap;
    
    /// the error integral's decay term
    float integralDecay;
    
    /// below this, the error is assumed to be zero
    float deadZone;
};

#endif /* __PIDMOTOR_H */
