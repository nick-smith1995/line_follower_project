#ifndef MBED_MOTOR_H
#define MBED_MOTOR_H
 
#include "mbed.h"
 
/** Interface to control a standard DC motor 
* with an H-bridge using a PwmOut and 2 DigitalOuts
*/
class Motor {
    public:
 
/** Create a motor control interface    
*
* @param pwm A PwmOut pin, driving the H-bridge enable line to control the speed
* @param fwd A DigitalOut, set high when the motor should go forward
* @param rev A DigitalOut, set high when the motor should go backwards
* @param set if the motor driver is able to do braking 0 false 1 true.
*/
        Motor(PinName pwm, PinName fwd, PinName rev, int brakeable);
  
/** Set the speed of the motor 
* 
* @param speed The speed of the motor as a normalised value between -1.0 and 1.0.
* @return the applied speed to the motor after checking to ensure motor doesn't switch from forward to reverse without stopping.
*/
        float speed(float speed);
        
/** Set the the motor to coast
* 
* @param void 
* @return motor coasts until another instruction is recived.
*/        
  
        void coast(void);

/** Set the motor to dynamicaly brake
* 
* @param float 0 - 1.0 provides some control over how hard the motor brakes. 
* @return duty applied to motor driver. -1 is error, motor driver can't brake.
*/

        float stop(float duty);
/** return the current state of the motor
*
* @param void
* @return state of motor, -1 to 1 is speed, -2 is braking, 2 is coasting. -3 is error. 
*/
        float state(void);
        
    protected:
        PwmOut _pwm;
        DigitalOut _fwd;
        DigitalOut _rev;
        int Brakeable; // cna the motor driver break
        int sign; //prevents throwing the motor from full foward to full reverse and stuff melting.
 
};





#endif
