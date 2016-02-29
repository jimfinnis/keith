/**
 * \file
 * Rover exceptions - this are thrown when there is an error;
 * there are several subclasses.
 * 
 */

#ifndef __ROVEREXCEPT_H
#define __ROVEREXCEPT_H

#include <exception>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

/// an exception thrown when there is an error in rover code - typically
/// a subclass such as SlaveException is thrown.
class RoverException : public std::exception {
protected:
    char msg[1024];
        
public:
    RoverException(){}
    RoverException(const char *m){
        if(m)
            strcpy(msg,m);
    }
    virtual const char *what(){
        return msg;
    }
    /// a variadic fluent modifier to set a better string with sprintf
    RoverException& set(const char *s,...){
        va_list args;
        va_start(args,s);
        
        vsnprintf(msg,1024,s,args);
        va_end(args);
        return *this;
    }
};



#endif /* __ROVEREXCEPT_H */
