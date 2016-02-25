/**
 * @file motor.h
 * @brief  Brief description of file.
 *
 */

#ifndef __MOTOR_H
#define __MOTOR_H

    
class Motor {
    int pwmPin,dirPin;
    
public:
    void init(int ppin,int dpin){
        pinMode(ppin,OUTPUT);
        pinMode(dpin,OUTPUT);
        pwmPin = ppin;
        dirPin = dpin;
        setSpeed(0);
    }
    
    
    // set control speed, -255 to 255.
    void setSpeed(int s){
        int pwm = (s<0) ? -s : s;
        digitalWrite(dirPin, (s<0) ? HIGH : LOW);
        if(pwm>255)pwm=255;
        analogWrite(pwmPin, pwm);
    }
};
        

#endif /* __MOTOR_H */
