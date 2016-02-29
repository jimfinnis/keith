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

#define NUMSLAVES 2

class Rover {
    SerialComms comms;
    SlaveProtocol protocol;
    
    SlaveDevice masterDev; // actually the master device, not a slave
    SlaveDevice slaveDev[NUMSLAVES];
    
    MasterData *masterData;
    SlaveData *slaveData[NUMSLAVES];
    bool valid;
public:
    Rover(){
        valid = false;
        masterData=NULL;
        for(int i=0;i<NUMSLAVES;i++)
            slaveData[i]=NULL;
    }
    
    ~Rover(){
        if(masterData)
            delete masterData;
        for(int i=0;i<NUMSLAVES;i++){
            if(slaveData[i])
                delete slaveData[i];
        }
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
        
        for(int i=0;i<NUMSLAVES;i++){
            slaveData[i] = new SlaveData(&slaveDev[i]);
            slaveDev[i].init(&protocol,registerTable_DRIVE,i+1);
            slaveData[i]->init();
        }
        
        valid =true;
        return valid;
    }
    
    void update(){
        if(valid){
            masterData->update();
            for(int i=0;i<NUMSLAVES;i++){
                slaveData[i]->update();
            }
        }
    }
    
    void attachCommsListener(StatusListener *s){
        comms.setStatusListener(s);
    }
    
    MasterData *getMasterData(){
        return masterData;
    }
    SlaveData *getSlaveData(int n){
        if(n<NUMSLAVES)
            return slaveData[n];
        else 
            throw RoverException().set("slave number out of range: %d",n);
    }
    
    void setMasterDebug(int v){
        masterDev.startWrites();
        masterDev.writeInt(REGMASTER_DEBUG,v);
        masterDev.endWrites();
        
    }
    
    void resetSlaveExceptions(){
        for(int i=0;i<NUMSLAVES;i++){
            slaveData[i]->reset(RESET_EXCEPTIONS);
        }
    }
          
};

#endif /* __ROVER_H */
