/**
 * @file slave.h
 * @brief  Brief description of file.
 *
 */

#ifndef __SLAVE_H
#define __SLAVE_H

struct SlaveData {
    SlaveDevice *slave;
    
    SlaveData(SlaveDevice *s){
        slave = s;
    }
    
    
    float reqspeed1;
    int timer;
    
    void init(){
        slave->setReadSet(READSET_MOTORS,
                          REG_TIMER,
                          REGDRIVE_M1_REQSPEED,
                          -1);
    }
    
    void update(){
        slave->readRegs(READSET_MOTORS);
        int rct=0;
        timer = slave->getRegInt(rct++);
        reqspeed1 = slave->getRegFloat(rct++);
    }
};
        

#endif /* __SLAVE_H */
