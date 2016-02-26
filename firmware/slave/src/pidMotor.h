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

class PIDMotor {
    Motor m;
    QuadEncoder e;
    
    float ctl; //!< the motor control value
    bool reversed; //!< encoder is backwards
    
    float prevActual;
    unsigned long prevTime;
    
    /// integral of error for I-term calculation
    float errorIntegral;
    /// derivative of error for D-term calculation - *really* it's the position derivative!
    float errorDerivative;
    
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
        
        // get error
        error = required - actual;
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
        
    
    /// set up the motor, passing in the motor control pins
    /// and whether the encoder pins are backwards!
    void init(int pwm,int dir,bool rev){
        m.init(pwm,dir);
        reversed = rev;
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
            if(reversed)actual = -actual;
            
            if(calculatePIDCorrection()){
                // calculate correction
                float t = pGain*error+iGain*errorIntegral+dGain*errorDerivative;
                // apply to motor speed
                ctl+= t;
                // gradually drop the control speed if required is near zero
                if(required<0.001f && required>-0.001f)
                    ctl *= MOTOR_SPEED_DECAY; 
                // and set the motor
                m.setSpeed(ctl);
            }
        }
    }
    
    /////////////////// motor values ///////////////////////
    
    float actual; //!< reported encoder speed
    float required; //!< required speed
    
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
