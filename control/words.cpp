/**
 * @file words.cpp
 * @brief  Brief description of file.
 *
 */
#include <stdio.h>
#include <angort/angort.h>

#include "../rover.h"
#include "../hashgets.h"
#include "../proputils.h"

using namespace angort;

%name rover
%shared

Rover r;
class MyStatusListener: public StatusListener {
    virtual void onMessage(const char *str){
        printf("MESSAGE: %s\n",str);
    }
};
static MyStatusListener stat;

%wordargs connect s (device -- valid/bool) connect to rover
{
    a->pushInt(r.init(p0)?1:0);
}

%word update
{
    r.update();
}

%wordargs params hii (hash motor slave --) set parameters
{
    if(p1<2 && p2<2 && p1>=0 && p2>=0){
        SlaveData *sd = r.getSlaveData(p2);
        MotorParams params;
        params.pgain = hgetfloatdef(p0,"pgain",0);
        params.igain = hgetfloatdef(p0,"igain",0);
        params.dgain = hgetfloatdef(p0,"dgain",0);
        params.icap = hgetfloatdef(p0,"icap",0);
        params.idecay = hgetfloatdef(p0,"idecay",0);
        params.deadzone = hgetfloatdef(p0,"deadzone",0);
        sd->sendMotorParams(p1,&params);
    }
}

%wordargs getslavetimer i (slave -- t)
{
    a->pushInt(r.getSlaveData(p0)->timer);
}

%wordargs getdata ii (motor slave -- hash) get motor params
{
    if(p0<2 && p1<2 && p0>=0 && p1>=0){
        SlaveData *sd = r.getSlaveData(p1);
        MotorData *ptr = sd->getData(p0);
        
        Hash *h = Types::tHash->set(a->pushval());
        h->setSymFloat("actual",ptr->actual);
        h->setSymFloat("error",ptr->error);
        h->setSymFloat("errorintegral",ptr->errorIntegral);
        h->setSymFloat("errorderiv",ptr->errorDeriv);
        h->setSymFloat("control",ptr->control);
        h->setSymInt("current",ptr->current);
        h->setSymInt("odo",ptr->odo);
    } else
        a->pushNone();
}
    
    

%init
{
    fprintf(stderr,"Initialising rover library %s %s\n",__DATE__,__TIME__);
    r.attachCommsListener(&stat);
}    
