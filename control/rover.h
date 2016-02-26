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
    SlaveDevice slaveDev[2];
    
    MasterData *masterData;
    SlaveData *slaveData[2];
    bool valid;
public:
    Rover(){
        valid = false;
        masterData=NULL;
        slaveData[0]=slaveData[1]=NULL;
    }
    
    ~Rover(){
        if(masterData)
            delete masterData;
        if(slaveData[0])
            delete slaveData[0];
        if(slaveData[1])
            delete slaveData[1];
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
        
        for(int i=0;i<2;i++){
            slaveData[i] = new SlaveData(&slaveDev[i]);
            slaveDev[i].init(&protocol,registerTable_DRIVE,i+1);
            slaveData[i]->init();
        }
        
        valid =true;
    }
    
    void update(){
        if(valid){
            masterData->update();
            slaveData[0]->update();
            slaveData[1]->update();
        }
    }
    
    void attachCommsListener(StatusListener *s){
        comms.setStatusListener(s);
    }
                                                
    
    MasterData *getMasterData(){
        return masterData;
    }
    SlaveData *getSlaveData(int n){
        return slaveData[n];
    }
    
    void setMasterDebug(int v){
        masterDev.startWrites();
        masterDev.writeInt(REGMASTER_DEBUG,v);
        masterDev.endWrites();
        
    }
          
};

#endif /* __ROVER_H */
