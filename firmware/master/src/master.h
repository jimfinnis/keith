/**
 * @file
 * Master device - address zero, fakes being an I2C device but is actually
 * local.
 * 
 */

#ifndef __MASTER_H
#define __MASTER_H

#define SERVOS 0

#if SERVOS
#include <Servo.h>
static Servo servo;
#endif

#include <NewPing.h>

#include "device.h"
#include "../../common/regsauto.h"

extern int debledct;

#define NUMSONARS 3

static NewPing sonars[NUMSONARS]={
    NewPing(7,8,100),
    NewPing(9,10,100),
    NewPing(11,12,100)
};

static int nextSonar=0;
static int curSonar=-1;
static int sonarDists[NUMSONARS];
static int sonarRawDists[NUMSONARS];

void echoCheck(){
    if(sonars[curSonar].check_timer()){
        sonarRawDists[curSonar]=sonars[curSonar].ping_result;
    }
}

class MasterDevice : public Device {
    int debug;
public:    
    MasterDevice() : Device(registerTable_MASTER){
        debug = 123;
        for(int i=0;i<NUMSONARS;i++)
            sonarDists[i]=42;
    }
    
    void init(){
#if SERVOS
        servo.attach(9);
#endif
    }
          
    /// reset will reset the master's exception state
    
    virtual int writeRegister(byte r,uint16_t val){
        int rv;
        if(rv=checkAndLoadRegister(r,true))return rv;
        
        switch(r){
        case REGMASTER_RESET:
            break;
        case REGMASTER_DEBUG:
            debug=val;
            debledct=val;
            if(debledct)digitalWrite(13,HIGH);
            break;
#if SERVOS
        case REGMASTER_SERVO:
            servo.write(val);
            break;
#endif
        }
        return 0;
    }
    
    virtual int readRegister(byte r,uint16_t *readValue){
        int rv;
        float t;
        int i;
        
        if(rv=checkAndLoadRegister(r,false)){
            // not ideal, putting the bad return value into
            // the data
            *readValue=rv;
            return rv;
        }
        switch(r){
        case REGMASTER_EXCEPTIONDATA:
            *readValue = 0;
            break;
#if SERVOS
        case REGMASTER_SERVO:
            *readValue = servo.read();
            break;
#endif
        case REGMASTER_SONAR1:
            *readValue = sonarDists[0];
            break;
        case REGMASTER_SONAR2:
            *readValue = sonarDists[1];
            break;
        case REGMASTER_SONAR3:
            *readValue = sonarDists[2];
            break;
        case REGMASTER_DEBUG:
            *readValue = debug;
            break;
            // put other registers in here
        default:
            *readValue = reg.map(-1);
        }
        return 0;
    }
    
    /// called every few milliseconds to do anything clever.
   
    void tick(){
        if(nextSonar==0){
            for(int i=0;i<NUMSONARS;i++)
                sonarDists[i]=sonarRawDists[i] / US_ROUNDTRIP_CM;
        }
        
        // ping the next sonar
        curSonar=nextSonar;
        sonars[curSonar].timer_stop();
        sonarRawDists[curSonar]=0;
        sonars[nextSonar++].ping_timer(echoCheck);
        nextSonar %= NUMSONARS;
    }
          
    
};


#endif /* __MASTER_H */
