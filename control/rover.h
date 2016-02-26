/**
 * @file rover.h
 * @brief  Brief description of file.
 *
 */

#ifndef __ROVER_H
#define __ROVER_H

#include "protocol.h"
#include "master.h"
#include "slave.h"

class Rover {
    SerialComms comms;
    SlaveProtocol protocol;
    
    SlaveDevice masterDev; // actually the master device, not a slave
    SlaveDevice slaveDev1;
    
    MasterData *masterData;
    SlaveData *slaveData1;
    bool valid;
public:
    Rover(){
        valid = false;
        masterData=NULL;
    }
    
    ~Rover(){
        if(masterData)
            delete masterData;
    }
    
    bool init(const char *port){
        comms.connect(port,115200);
        if(!comms.isReady())
            return false;
        protocol.init(&comms);
        
        masterData = new MasterData(&masterDev);
        // protocol, registers, I2C ID (=0 for master)
        masterDev.init(&protocol,registerTable_MASTER,0);
        masterData->init(); // set read set
        
        slaveData1 = new SlaveData(&slaveDev1);
        slaveDev1.init(&protocol,registerTable_DRIVE,1);
        slaveData1->init();
        
        
        valid =true;
    }
    
    void update(){
        if(valid){
            masterData->update();
            slaveData1->update();
        }
    }
    
    void attachCommsListener(StatusListener *s){
        comms.setStatusListener(s);
    }
                                                
    
    MasterData *getMasterData(){
        return masterData;
    }
    SlaveData *getSlaveData1(){
        return slaveData1;
    }
    
    void setMasterDebug(int v){
        masterDev.startWrites();
        masterDev.writeInt(REGMASTER_DEBUG,v);
        masterDev.endWrites();
        
    }
          
};

#endif /* __ROVER_H */
