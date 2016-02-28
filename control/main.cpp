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
    r.update();
    printf("%d\n",r.getMasterData()->debug);
    printf("%d\n",r.getSlaveData(0)->timer);
}
