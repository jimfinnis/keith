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
    
    for(int i=0;i<100;i++){
        r.update();
        printf("%d\n",r.getMasterData()->debug);
        SlaveData *s = r.getSlaveData(0);
        printf("Timer:%d status:%d except:%d debug:%d\n",s->timer,
               s->status,s->exceptionData,s->debug);
        sleep(1);
        r.resetSlaveExceptions();
    }
}
