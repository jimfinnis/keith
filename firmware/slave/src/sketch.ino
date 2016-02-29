#include "pidMotor.h"

#include "pwm.h"
#include <stdint.h>
#include <avr/wdt.h>

#define I2C_ADDR 2

#include "config.h"
#include "../../common/regsauto.h"
#include "slave_i2c.h"

/// last time we got a message from the master
volatile unsigned long lastmsgtime=0;

PIDMotor m1,m2;

// the encoder interrupts. ENC1A is on pin 7 (PD7)
// and ENC2A is on pin 10 (PB2)

void setupEncoderISR(){
    cli();
    PCMSK2 |= 1<<PCINT23; // unmask interrupt for PD7
    PCMSK0 |= 1<<PCINT2; // unmask interrupt for PB2
    PCICR |= (1<<PCIE2)|(1<<PCIE0); // turn on ints for masks 0 and 2
    sei();
}
ISR(PCINT2_vect){
    if(PIND & (1<<PIND7)){ // rising edge on D7 (pin 7)
        // check pin B0 (pin 8) and pass it in.
        m1.tickEncoder(PINB & (1<<PINB0)); // tick, passing in other pin state
    }
}
ISR(PCINT0_vect){
    if(PINB & (1<<PINB2)){ // rising edge on B2 (pin 10)
        // check pin PB1 (pin 9) and pass it in
        m2.tickEncoder(PINB & (1<<PINB1));
    }
}

bool inException=true; // we always start in exception state

int ledCounts[]={0,0}; // debugging LED(s)

class MyI2CListener : public I2CSlaveListener {
    /// called when i2c changes a register
    virtual void changeEvent(int reg,uint16_t val) {
        float t;
        const Register *regs=registerTable_DRIVE;
        switch(reg){
            // general purpose registers
        case REG_DEBUGLED:
            if(val<2)
                ledCounts[val]=100;
            break;
        case REG_EXCEPTIONDATA:
            // notification from the master that another slave
            // has a problem
            inException=true;
            break;
        case REG_PING:
            ledCounts[0]=2;
            break;
        case REG_RESET:
            I2CSlave::setRegisterInt(REG_RESET,0); // clear it back!
            if(val & RESET_HARD){ // HARD RESET
                // basically we just spin around this loop
                // until the watchdog notices!
                while(1){
                    delay(1);
                }
            }
            if(val & RESET_EXCEPTIONS) // reset exceptions
            {
                inException=false;
            }
            if(val & RESET_ODO1)
                m1.resetOdometry();
            if(val & RESET_ODO2)
                m2.resetOdometry();
            break;
            
            // drive motor registers
            
        case REGDRIVE_M1_REQSPEED:
            m1.required=regs[reg].unmap(val);
            break;
        case REGDRIVE_M1_PGAIN:
            m1.pGain=regs[reg].unmap(val);
            break;
        case REGDRIVE_M1_IGAIN:
            m1.iGain=regs[reg].unmap(val);
            break;
        case REGDRIVE_M1_DGAIN:
            m1.dGain=regs[reg].unmap(val);
            break;
        case REGDRIVE_M1_INTEGRALCAP:
            m1.integralCap=regs[reg].unmap(val);
            break;
        case REGDRIVE_M1_INTEGRALDECAY:
            m1.integralDecay=regs[reg].unmap(val);
            break;
        case REGDRIVE_M1_DEADZONE:
            m1.deadZone=regs[reg].unmap(val);
            break;
            
        case REGDRIVE_M2_REQSPEED:
            m2.required=regs[reg].unmap(val);
            break;
        case REGDRIVE_M2_PGAIN:
            m2.pGain=regs[reg].unmap(val);
            break;
        case REGDRIVE_M2_IGAIN:
            m2.iGain=regs[reg].unmap(val);
            break;
        case REGDRIVE_M2_DGAIN:
            m2.dGain=regs[reg].unmap(val);
            break;
        case REGDRIVE_M2_INTEGRALCAP:
            m2.integralCap=regs[reg].unmap(val);
            break;
        case REGDRIVE_M2_INTEGRALDECAY:
            m2.integralDecay=regs[reg].unmap(val);
            break;
        case REGDRIVE_M2_DEADZONE:
            m2.deadZone=regs[reg].unmap(val);
            break;
            
        default:break;
        }
    }
};

static MyI2CListener listener;

/// set the registers for parameters which may have been set in
/// initialisation!
void setInitRegsParams(){
    I2CSlave::setRegisterFloat(REGDRIVE_M1_REQSPEED,m1.required);
    I2CSlave::setRegisterFloat(REGDRIVE_M1_PGAIN,m1.pGain);
    I2CSlave::setRegisterFloat(REGDRIVE_M1_IGAIN,m1.iGain);
    I2CSlave::setRegisterFloat(REGDRIVE_M1_DGAIN,m1.dGain);
    I2CSlave::setRegisterFloat(REGDRIVE_M1_INTEGRALCAP,m1.integralCap);
    I2CSlave::setRegisterFloat(REGDRIVE_M1_INTEGRALDECAY,m1.integralDecay);
    I2CSlave::setRegisterFloat(REGDRIVE_M1_DEADZONE,m1.deadZone);
    
    I2CSlave::setRegisterFloat(REGDRIVE_M2_REQSPEED,m2.required);
    I2CSlave::setRegisterFloat(REGDRIVE_M2_PGAIN,m2.pGain);
    I2CSlave::setRegisterFloat(REGDRIVE_M2_IGAIN,m2.iGain);
    I2CSlave::setRegisterFloat(REGDRIVE_M2_DGAIN,m2.dGain);
    I2CSlave::setRegisterFloat(REGDRIVE_M2_INTEGRALCAP,m2.integralCap);
    I2CSlave::setRegisterFloat(REGDRIVE_M2_INTEGRALDECAY,m2.integralDecay);
    I2CSlave::setRegisterFloat(REGDRIVE_M2_CURRENT,m2.getCurrent());
    I2CSlave::setRegisterFloat(REGDRIVE_M2_DEADZONE,m2.deadZone);
}

/// set the registers for values read from the motors, and other values,
/// to be sent to the master
void setRegsValues(){
    I2CSlave::setRegisterFloat(REGDRIVE_M1_CURRENT,m1.getCurrent());
    I2CSlave::setRegisterFloat(REGDRIVE_M1_ACTUALSPEED,m1.actual);
    I2CSlave::setRegisterFloat(REGDRIVE_M1_ERROR,m1.error);
    I2CSlave::setRegisterFloat(REGDRIVE_M1_CONTROL,m1.ctl);
    I2CSlave::setRegisterFloat(REGDRIVE_M1_ERRORINTEGRAL,m1.errorIntegral);
    I2CSlave::setRegisterFloat(REGDRIVE_M1_ERRORDERIV,m1.errorDerivative);
    I2CSlave::setRegisterInt(REGDRIVE_M1_ODO,m1.getOdometry());

    I2CSlave::setRegisterFloat(REGDRIVE_M2_CURRENT,m2.getCurrent());
    I2CSlave::setRegisterFloat(REGDRIVE_M2_ACTUALSPEED,m2.actual);
    I2CSlave::setRegisterFloat(REGDRIVE_M2_ERROR,m2.error);
    I2CSlave::setRegisterFloat(REGDRIVE_M2_CONTROL,m2.ctl);
    I2CSlave::setRegisterFloat(REGDRIVE_M2_ERRORINTEGRAL,m2.errorIntegral);
    I2CSlave::setRegisterFloat(REGDRIVE_M2_ERRORDERIV,m2.errorDerivative);
    I2CSlave::setRegisterInt(REGDRIVE_M2_ODO,m2.getOdometry());
    
    // status and exception just tell us the same thing right now
    uint16_t status=0;
    if(inException)status|=ST_EXCEPTION;
    I2CSlave::setRegisterInt(REG_STATUS,status);
    I2CSlave::setRegisterInt(REG_EXCEPTIONDATA,inException?1:0);
    I2CSlave::setRegisterInt(REG_TIMER,millis());
    I2CSlave::setRegisterInt(REG_DEBUG,123);
}


    
void initI2C(int addr){
    // all registers are reset by default to zero as an unsigned int, i.e. the bottom
    // of the range for registers which are float-mapped.
    // Here we have to make sure the register maps the actual value.
    I2CSlave::init(addr,registerTable_DRIVE);
    setInitRegsParams();
    I2CSlave::addListener(&listener);
}


void setup()
{
    Serial.begin(9600);
    Serial.println("reboot");
    
    MCUSR=0; // reset any enabled WDT
    wdt_enable(WDTO_1S); // start WDT
    
    // set pwm to 31K for inaudible motors!
//    setPwmFrequency(PWm2,1); 
//    setPwmFrequency(PWM2,1);
    
    // motors: the two pins, if direction is reversed, and if encoders
    // are reversed.
    
#if (I2C_ADDR==2)
    m1.init(PWM1,DIR1,true,true); 
    m2.init(PWM2,DIR2,true,true); 
#else
    m1.init(PWM1,DIR1,false,true); 
    m2.init(PWM2,DIR2,false,true); 
#endif
    pinMode(ENC1A,INPUT_PULLUP); // the two encoder pins
    pinMode(ENC1B,INPUT_PULLUP);
    
    pinMode(ENC2A,INPUT_PULLUP); // the two encoder pins
    pinMode(ENC2B,INPUT_PULLUP);
    
//    Serial.println("I2C init");
    initI2C(I2C_ADDR); // takes address
//    Serial.println("I2C init done");
    
    pinMode(LED1,OUTPUT);
    pinMode(LED2,OUTPUT);
    
    // flash LED
    wdt_reset();
    digitalWrite(LED1,HIGH);
    delay(200);
    wdt_reset();
    digitalWrite(LED1,LOW);

    setupEncoderISR();
//    Serial.println("READY");
}

void loop()
{
    // read I2C data and handle register changes
//    Serial.println("i2c poll");
    I2CSlave::poll();
    
    // update motors
//    Serial.println("motor update");
    m1.update();
    m2.update();
//    Serial.println("updated");
    
    wdt_reset();
    
    // update the register values for reading by master
    setRegsValues();
//    Serial.println("regset");
    
    unsigned long q = 0;
    unsigned long t = micros();
    if(t>q)
        I2CSlave::setRegisterInt(REG_INTERVALI2C,t-q);
    q=t;
    
    // internal heartbeat
    static int qqq=0;
    if(qqq++==100){
        ledCounts[1]=1;
        qqq=0;
    }
    
    for(int i=0;i<2;i++){
        int out;
        if(ledCounts[i] || inException)
            out=HIGH;
        else
            out=LOW;
        if(ledCounts[i])ledCounts[i]--;
        digitalWrite(i==0?LED1:LED2,out);
    }
}
