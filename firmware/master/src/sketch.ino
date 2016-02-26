/**
 * @file This is the code for the Arduino I2C master for the rover,
 * controlling up to nine serial driver boards.
 */

#include <Wire.h>
#include <stdint.h>
#include <avr/wdt.h>
#include <util/twi.h>


#include "i2c.h"
#include "master.h"

#ifndef cbi
/// handy macro for clearing register bits (turning off pullups)
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

/// the master device representing registers actually on the arduino
/// master itself
MasterDevice master;
Register Device::reg; //!< static for temp reg spec store


uint8_t Device::readSet[READSETS][READSETSIZE];
uint8_t Device::readSetCt[READSETS];

/// handy debug method  - displays a sequence of N flashes with a pause
/// between them, m times. If m is -100, does it forever.

void flashdeb(int n,int m){
    while(m==-100 || m--){
        for(int i=0;i<n;i++){
            delay(200);
            digitalWrite(13,LOW);
            delay(200);
            digitalWrite(13,HIGH);
        }
        delay(1000);
    }
}

/// panic method - flashes n times, forever
void halt(int n){
    wdt_disable();
    flashdeb(n,-100);
}

// panic on slave - flash code is 4,address,exception
void panic(int address,int exception){
    for(;;){
        flashdeb(4,1);
        flashdeb(address,1);
        flashdeb(exception,1);
        delay(1000);
    }
}
              

/// an array of slave devices, one for each I2C device. 

#define NUMSLAVES 1
I2CDevice slaves[] = {
    I2CDevice(1,registerTable_DRIVE),
//    I2CDevice(2,registerTable_DRIVE),
};

class Device *getDeviceByAddr(int addr){
    if(addr==0)
        return &master;
    else if(addr<=NUMSLAVES)
        return slaves+(addr-1);
    else halt(10);
    
    return NULL; //never happens
    
}


/// when we last got a USB message
unsigned long lastmsgtime=0;

/// debugging light counter
int debledct=0;


/// a binary reader which listens for serial messages whose first byte
/// is their length, and then passes them into a process() method.

class BinarySerialReader {
    int ct;
    uint8_t buf[256];
    
protected:
    /// takes command block sans first (count) character and parses it.
    /// Also takes the payload length (length of message AFTER the 
    /// command byte)
    virtual void process(int ct,uint8_t *p) = 0;
    
public:
    /// initialise, zeroing the byte count - meaning we're waiting
    /// for length byte
    BinarySerialReader(){
        ct=0;
    }
    /// build up a buffer - once the length of the buffer is equal to
    /// the first byte read, process it.
    void poll(){
        if(Serial.available()){
            buf[ct++]=Serial.read();
            if(buf[0]==ct){
                // send the buffer without the size byte,
                // and the payload length (message size minus
                // size and command bytes)
                process(ct-2,buf+1);
                ct=0;
            }
        }
    }
};

int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

/// an implementation of BinarySerialReader which processes read
/// and write messages and reads and writes SlaveDevice registers
/// appropriately

class MySerialReader : public BinarySerialReader {
    /// process a set of register writes
    void dowrite(Device *s, uint8_t *p){
        int writes = *p++; // get the number of writes
        for(int i=0;i<writes;i++){ // for each write
            int r = *p++; // get the register number
            uint16_t v = *p++; // get the value (low byte)
            if(s->getRegisterSize(r)==2){ // if the value is 16-bit
                v|= *p++<<8; // get the value (high byte)
            }
            // and write the value over I2C
            wdt_reset();
            s->writeRegister(r,v);
        }
        wdt_reset();
        Serial.write((uint8_t)0); // just to say it's done
    }
    
    /// process a set of register reads
    void doread(Device *s,uint8_t *p){
        uint8_t buf[128];
        int ct=0;
        
        uint8_t set = *p++; // get the read set index
        
        // get the read set itself
        uint8_t readSetCt;
        const uint8_t *readSet = Device::getReadSet(set,&readSetCt);
        
        for(int i=0;i<readSetCt;i++){ // for each read
            uint16_t v;
            uint8_t r = readSet[i];
            wdt_reset();
            s->readRegister(r,&v); // get the register value from I2C
            buf[ct++]=v & 0xff; // store the bottom byte in the buffer
            if(s->getRegisterSize(r)==2) // if the value is 16-bit
                buf[ct++]=v>>8; // store the top byte
        }
        wdt_reset();
        Serial.write(buf,ct); // write the buffer
    }
    
    void doreadset(uint8_t *p,int ct){
        uint8_t set = *p++;
        ct--; // subtracting 1 because of the readset index
        Device::clearReadSet(set);
        for(int i=0;i<ct;i++){
            Device::addReadSet(set,*p++);
        }
        wdt_reset();
        Serial.write(ct);
    }
    
protected:
    /// takes command block sans first (count) character and parses it, reading
    /// the command number and device ID, and passing the rest of the block into
    /// appropriate command methods.
    virtual void process(int ct,uint8_t *p){
        int id = *p>>4; // get the addr/id
        
        // now get the device from the addr
        Device *s = getDeviceByAddr(id);
        
        switch(*p++&0xf){ // get the command and increment the pointer
        case 2: //write command
            dowrite(s,p);
            break;
        case 3: //read command
            doread(s,p);
            break;
        case 4:// read set command
            doreadset(p,ct);
            break;
        default:break;
        }
        lastmsgtime = millis();
    }
};

MySerialReader serialReader;

/// start I2C and serial, and wait a little while for the
/// other units to start up before we can command them

void setup()
{
    Serial.begin(115200); // start the serial IO
    Serial.println("Starting");
    
    Wire.begin(); // join I2C bus as the master
    pinMode(13,OUTPUT);
    digitalWrite(13,HIGH);
    digitalWrite(13,LOW);
    
    
//    cbi(PORTC,4); // disable internal pullups for I2C
//    cbi(PORTC,5);
    
    master.init(); // start up master device
    
    wdt_enable(WDTO_1S);
    for(int i=0;i<NUMSLAVES;i++){
        slaves[i].reset();
    }
    wdt_enable(WDTO_120MS);
    
    //    TWSR = 3; // slow down
    //    TWBR = 0x20;
    
    wdt_reset();
    Serial.print(__DATE__);Serial.print(" ");Serial.println(__TIME__);
    Serial.println(freeRam());
    Serial.println("Ready");
}

/// main loop - just handle characters coming in on
/// the serial port.

void loop()
{
    static unsigned long lastt1=0;
    static unsigned long lastt2=0;
    
    // every now and then, ping each device.
    // This will timeout if the device has died.
    // Only do this once we've waited long enough
    // for the devices to come up.
    
    unsigned long t = millis();
    if(t>5000){
        if(t>lastt1 && (t-lastt1)>1000){
            lastt1=t;
            digitalWrite(13,HIGH);
            debledct=100;
            for(int i=0;i<NUMSLAVES;i++){
                slaves[i].update();
            }
        }
        if(t>lastt2 && (t-lastt2)>2000){
            lastt2=t;
            master.tick();
        }
    }
    
    if(debledct && !--debledct){
        digitalWrite(13,LOW);
    }
    wdt_reset();
    serialReader.poll();
}
