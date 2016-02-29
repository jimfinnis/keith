/**
 * @file proputils.h
 * @brief  Brief description of file.
 *
 */

#ifndef __PROPUTILS_H
#define __PROPUTILS_H


struct StringProperty : public Property {
    void set(const char *s){
        Types::tString->set(&v,s);
    }
    const char *get(){
        return v.toString().get();
    }
};
struct FloatProperty : public Property {
    FloatProperty(float *d){dd=d;}
    virtual void postSet(){
        *dd = v.toFloat();
    }
    virtual void preGet(){
        Types::tFloat->set(&v,*dd);  
    }
    float *dd;
};


inline void reg(Angort *a,const char *s,Property *v){
    a->registerProperty(s,v,"exp");
}


#endif /* __PROPUTILS_H */
