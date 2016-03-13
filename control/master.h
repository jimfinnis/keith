/**
 * @file master.h
 * @brief  Brief description of file.
 *
 */

#ifndef __MASTER_H
#define __MASTER_H

#include "protocol.h"
#include "../firmware/common/regsauto.h"

struct MasterData {
    /// a "fake" device that looks like an I2C device, but in
    /// fact is used for talking to the master directly.
    SlaveDevice *slave;
    // data read from serial
    int debug;
    int exceptionData;
    int sonarDists[3];
    
    MasterData(SlaveDevice *s){
        slave = s;
    }
    
    void init(){
        slave->setReadSet(READSET_MASTER,
                          REGMASTER_DEBUG,
                          REGMASTER_EXCEPTIONDATA,
                          REGMASTER_SONAR1,
                          REGMASTER_SONAR2,
                          REGMASTER_SONAR3,
                          -1);
    }
    
    void update(){
        slave->readRegs(READSET_MASTER);
        
        // here, we read the registers in the order in which they are
        // given in the read set
        
        int rct=0;
        debug = slave->getRegInt(rct++);
        exceptionData = slave->getRegInt(rct++);
        sonarDists[0]=slave->getRegInt(rct++);
        sonarDists[1]=slave->getRegInt(rct++);
        sonarDists[2]=slave->getRegInt(rct++);
    }
    
    void setServo(int v){
//        slave->startWrites();
//        slave->writeInt(REGMASTER_SERVO,v);
        //        slave->endWrites();
        printf("SERVO NOT IMPLEMENTED\n");
    }
};



#endif /* __MASTER_H */
