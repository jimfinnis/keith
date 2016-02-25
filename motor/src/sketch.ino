#include "pidMotor.h"

static char serialbuf[256];
static char *serialptr;
static float stack[8];
static int stackct=0;


float pop(){
    if(stackct>0)
        return stack[--stackct];
    else
        return 0;
}

PIDMotor m;

void setupEncoderISR(){
    cli();
    PCMSK2 |= 1<<PCINT18; // unmask interrupt for PD2 = digital pin 2
    PCICR |= 1<<PCIE2; // turn on ints for port D
    sei();
}
ISR(PCINT2_vect){
    if(PIND & 4){ // rising edge on in 2
        m.tickEncoder(PIND & 8); // tick, passing in other pin state
    }
}
    

void setup()
{
    Serial.begin(9600);
    
    m.init(5,4,true); // pins and whether I've done the encoder backwards
    pinMode(2,INPUT_PULLUP); // the two encoder pins
    pinMode(3,INPUT_PULLUP);
    
    setupEncoderISR();
    Serial.println("READY");
}

void processSerial(){
    char *str = serialbuf;
    while(char *s = strtok(str," ")){
        Serial.println(s);
        str = NULL;
        if((*s == '-' || *s == '.' || (*s>='0' && *s<='9')) && stackct<8){
            stack[stackct++]=atof(s);
            Serial.print("Stacking ");Serial.println(s);
        } else switch(*s){
        case 's':{
            float s = pop();
            m.required = s;
        }break;
        case 'p':
            m.pGain = pop();break;
        case 'i':
            m.iGain = pop();break;
        case 'd':
            m.dGain = pop();break;
        default:break;
        }
    }
}

void loop()
{
    m.update();
    while(Serial.available()>0){
        char c = Serial.read();
        Serial.println(c);
        if(serialptr-serialbuf == 256)serialptr=serialbuf;
        if(c=='\r'){
            *serialptr=0;
            Serial.print("SP: ");Serial.println(m.actual);
            processSerial();
            serialptr=serialbuf;
        } else {
            *serialptr++ = c;
        }
    }
    
}
