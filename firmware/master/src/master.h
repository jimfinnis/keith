/**
 * @file
 * Master device - address zero, fakes being an I2C device but is actually
 * local.
 * 
 */

#ifndef __MASTER_H
#define __MASTER_H

#include "device.h"
#include "../../common/regsauto.h"

class MasterDevice : public Device {
public:    
    MasterDevice() : Device(registerTable_MASTER){
    }
    
    void init(){
    }
          
    /// reset will reset the master's exception state
    
    virtual int writeRegister(byte r,uint16_t val){
        int rv;
        if(rv=checkAndLoadRegister(r,true))return rv;
        
        switch(r){
        case REGMASTER_RESET:
            break;
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
            // put other registers in here
        default:
            *readValue = reg.map(-1);
        }
        return 0;
    }
    
    /// called every few seconds to do anything clever.
   
    void tick(){
    }
          
    
};


#endif /* __MASTER_H */
