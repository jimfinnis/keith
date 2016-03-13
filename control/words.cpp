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

// wheel to slave ID
static int w2s(int n){return n/2;}
// wheel to motor DI
static int w2m(int n){return n%2;}

%wordargs connect s (device -- valid/bool) connect to rover
{
    a->pushInt(r.init(p0)?1:0);
}

%word update
{
    r.update();
}

%word reset reset exceptions
{
    r.resetSlaveExceptions();
}

%wordargs params hi (hash wheel --) set parameters
{
    if(p1<4 && p1>=0) {
        SlaveData *sd = r.getSlaveData(w2s(p1));
        MotorParams params;
        params.pgain = hgetfloatdef(p0,"pgain",0);
        params.igain = hgetfloatdef(p0,"igain",0);
        params.dgain = hgetfloatdef(p0,"dgain",0);
        params.icap = hgetfloatdef(p0,"icap",0);
        params.idecay = hgetfloatdef(p0,"idecay",0);
        params.deadzone = hgetfloatdef(p0,"deadzone",0);
        sd->sendMotorParams(w2m(p1),&params);
    }
}

%wordargs getslavedata i (slave -- t)
{
    if(p0<NUMSLAVES){
        Hash *h = Types::tHash->set(a->pushval());
        h->setSymInt("timer",r.getSlaveData(p0)->timer);
        h->setSymInt("status",r.getSlaveData(p0)->status);
        h->setSymInt("exception",r.getSlaveData(p0)->exceptionData);
        h->setSymInt("debug",r.getSlaveData(p0)->debug);
    } else
        a->pushNone();
        
        
}

%wordargs getdata i (wheel -- hash) get motor params
{
    if(p0<4 && p0>=0){
        SlaveData *sd = r.getSlaveData(w2s(p0));
        MotorData *ptr = sd->getData(w2m(p0));
        
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

%wordargs servo i (val --) set servo pos
{
    MasterData *m= r.getMasterData();
    m->setServo(p0);
}

%wordargs sonar i (val -- val) get sonar dist
{
    MasterData *m= r.getMasterData();
    if(p0>=0 && p0<=2)
        a->pushInt(m->sonarDists[p0]);
    else
        a->pushNone();      
}
    
    
struct SpeedProperty : public Property {
    SlaveData *slave;
    int motor;
    Angort *a;
    
    SpeedProperty(Angort *_a){
        a=_a;
    }
    
    virtual void preSet(){
        int w = a->popInt();
        slave = r.getSlaveData(w2s(w));
        motor = w2m(w);
    }
    virtual void postSet(){
        slave->setSpeed(motor,v.toFloat());
    }
    virtual void preGet(){
        preSet();
        Types::tFloat->set(&v,slave->getData(motor)->req);
    }
};

%init
{
    fprintf(stderr,"Initialising rover library %s %s\n",__DATE__,__TIME__);
    r.attachCommsListener(&stat);
    
    a->registerProperty("speed",new SpeedProperty(a),"rover");
}    
