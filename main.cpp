#include "mbed.h"
#include "motordriver.h"
#include "solenoiddriver.h"

#define NUM_SENSORS 8
#define avgSpeed 255


//initialise wheel motors 
//TODO: check pins
Motor A(PTA4, PTC6, PTC5, 1); // pwm, fwd, rev, can break
Motor B(PTA12, PTC7, PTC0, 1); // pwm, fwd, rev, can break

//declare PID variables and motor direction variables
//TODO: test pwm, direction values, check average speed value
//int time = 0;
int pwmA = 3;
int pwmB = 11;

int dirA = 12;
int dirB = 13;
  
int kp = 1;
int kd = 1;
int ki = 1;
  
int inst_error = 0;
int lastError = 0;
int proportional = 0;
int derivative = 0;
int integral = 0;


//setup sensor in from arduino
DigitalIn sensorValues[] = {(PTA1),(PTA2),(PTD4),(PTA12),(PTA4),(PTA5),(PTC8),(PTC9)};

//setup barcode scanner from arduino
DigitalIn analog_sensorValues (PTC2);

//TODO: write solenoid motor driver function


//read incoming arduino value
void readValues(){
    
    for (int i=0; i<NUM_SENSORS; i++){
        sensorValues[i] = sensorValues[i];
        }
}

void calculateProportional(){
    int sum = 0;
    int posLeft = 10;
    int posRight = 10;
    
    for (int i=0; i<NUM_SENSORS/2; i++){
        sum=sum+sensorValues[i];
        if(sensorValues[i]==1){
            posRight=i-3;
            }
    }
    
    for (int i=NUM_SENSORS-1; i>=NUM_SENSORS/2; i--){
        sum=sum+sensorValues[i];
        if(sensorValues[i]==1){
            posLeft=i-4;
        }
    }
    
    if(sum>=3)sum=2;
    
    if(sum==0){
        if(lastError<0){
            inst_error=-8;
        }else{
            inst_error=8;
        }
    }else if((posLeft!=10)&&(posRight!=10)){
        inst_error=0;
        
    }else if(posLeft!=10){
        inst_error=posLeft*2+sum;
    
    }else if(posRight!=10){
        inst_error=posRight*2-sum;
    }
    
    proportional = inst_error;

}


void follow_line(){
    
    derivative = inst_error-lastError;
    integral += proportional;
    
    if(integral > 255){
        integral=255;
    };
    if(integral < -255){
        integral=-255;
    };
    
    int turn = proportional*kp + derivative*kd + integral*ki;
    
    if(turn>=255) turn=255;
    
    if(turn<=-255) turn=-255;
    
    int speedA=0;
    int speedB=0;
    
    if(turn>=0){
        speedA=avgSpeed;
        speedB=avgSpeed-turn;
    }else{
        speedA=avgSpeed+turn;
        speedB=avgSpeed;
    }
    
    //normalise speed to 1.0; -1.0
    speedA = speedA/255;
    speedB = speedB/255;
    
    //write speed to each motor
    A.speed(speedA);
    B.speed(speedB);
     
    lastError=inst_error;
    
}

//read barcode value from arduino
void barc0de_scan(){   
    analog_sensorValues = analog_sensorValues;
}



int main(){
    
    //initialise stopping duty cycle (ie: how hard the motor brakes)
    //TODO: test values
    float stop_duty = 0.5;
    
    while(1){
 
        while (analog_sensorValues == 0){ //while bar code scanner reads no line
        
        readValues();
        calculateProportional();
        follow_line();
        barc0de_scan();
       
        }
        
        //stop wheel motors TODO: figure out duty cycle??
        A.stop(stop_duty);
        B.stop(stop_duty);
        
        //start solenoid motor
        //load ball
        //wait till ball is shot
        //stop solenoid motor
        
        //start wheel motor        
        readValues();
        calculateProportional();
        follow_line();
    }        
    return 0;
}
