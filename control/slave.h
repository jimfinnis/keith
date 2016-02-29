/**
 * @file slave.h
 * @brief  Brief description of file.
 *
 */

#ifndef __SLAVE_H
#define __SLAVE_H

#define PARAMOFFSET (REGDRIVE_M2_REQSPEED-REGDRIVE_M1_REQSPEED)

struct MotorData {
    float actual,error,errorIntegral,errorDeriv;
    float control;
    int current,odo;
    float req; // set in SlaveData::setspeed
    
    MotorData(){
        actual=error=errorIntegral=errorDeriv=0;
        current=odo=control=0;
        req=0;
    }
    
    // assumes that readRegs() has been called on the slave
    // for an appropriate read set, and this will be called
    // at the appropriate point in it.
    
    int read(SlaveDevice *s,int rct){
        actual = s->getRegFloat(rct++);
        error = s->getRegFloat(rct++);
        errorIntegral = s->getRegFloat(rct++);
        errorDeriv = s->getRegFloat(rct++);
        control = s->getRegFloat(rct++);
        current = s->getRegInt(rct++);
        odo = s->getRegInt(rct++);
        return rct;
    }
};

struct MotorParams {
    float pgain,igain,dgain;
    float icap,idecay,deadzone;
    
    MotorParams(){
        pgain=igain=dgain=0;
        icap=idecay=deadzone=0;
    }
    
};
    

struct SlaveData {
    SlaveDevice *slave;
    SlaveData(SlaveDevice *s){
        slave = s;
    }
    
    MotorData m[2];
    int timer,status,exceptionData,debug;
    
    void init(){
        slave->setReadSet(READSET_MOTORS,
                          REG_TIMER,
                          REG_STATUS,
                          REG_EXCEPTIONDATA,
                          REG_DEBUG,
                          
                          REGDRIVE_M1_ACTUALSPEED,
                          REGDRIVE_M1_ERROR,
                          REGDRIVE_M1_ERRORINTEGRAL,
                          REGDRIVE_M1_ERRORDERIV,
                          REGDRIVE_M1_CONTROL,
                          REGDRIVE_M1_CURRENT,
                          REGDRIVE_M1_ODO,
                          
                          REGDRIVE_M2_ACTUALSPEED,
                          REGDRIVE_M2_ERROR,
                          REGDRIVE_M2_ERRORINTEGRAL,
                          REGDRIVE_M2_ERRORDERIV,
                          REGDRIVE_M2_CONTROL,
                          REGDRIVE_M2_CURRENT,
                          REGDRIVE_M2_ODO,
                          -1);
    }
    
    void update(){
        slave->readRegs(READSET_MOTORS);
        int rct=0;
        
        timer = slave->getRegInt(rct++);
        status = slave->getRegInt(rct++);
        exceptionData = slave->getRegInt(rct++);
        debug = slave->getRegInt(rct++);
        
        rct = m[0].read(slave,rct);
        rct = m[1].read(slave,rct);
    }
    void sendMotorParams(int motor,MotorParams *p){
        int offset = PARAMOFFSET*motor;
        slave->startWrites();
        slave->writeFloat(REGDRIVE_M1_PGAIN+offset,p->pgain);
        slave->writeFloat(REGDRIVE_M1_IGAIN+offset,p->igain);
        slave->writeFloat(REGDRIVE_M1_DGAIN+offset,p->dgain);
        slave->writeFloat(REGDRIVE_M1_INTEGRALCAP+offset,p->icap);
        slave->writeFloat(REGDRIVE_M1_INTEGRALDECAY+offset,p->idecay);
        slave->writeFloat(REGDRIVE_M1_DEADZONE+offset,p->deadzone);
        slave->endWrites();
    }
    
    MotorData *getData(int i){
        return m+i;
    }
    
    void setSpeed(int motor,float req){
        int offset = PARAMOFFSET*motor;
        slave->startWrites();
        slave->writeFloat(REGDRIVE_M1_REQSPEED+offset,req);
        m[motor].req = req;
        slave->endWrites();
    }
    void reset(int flags){
        slave->startWrites();
        slave->writeInt(REG_RESET,flags);
        slave->endWrites();
    }
    void ping(){
        slave->startWrites();
        slave->writeInt(REG_PING,1);
        slave->endWrites();
    }
};
        

#endif /* __SLAVE_H */
