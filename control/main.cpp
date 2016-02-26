/**
 * @file main.cpp
 * @brief  Brief description of file.
 *
 */
#include "rover.h"

class MyStatusListener: public StatusListener {
    virtual void onMessage(const char *str){
        printf("MESSAGE: %s\n",str);
    }
};

MyStatusListener stat;

int main(int argc,char *argv[]){
    Rover r;
    r.attachCommsListener(&stat);
    r.init(argv[1]);
    sleep(1);
    r.setMasterDebug(10000);
    r.update();
    
    printf("%d\n",r.getMasterData()->debug);
    printf("%d\n",r.getSlaveData1()->timer);
    printf("%f\n",r.getSlaveData1()->reqspeed1);
}
