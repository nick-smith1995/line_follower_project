#include "mbed.h"
#include "QTRSensors.h"

#define NUM_SENSORS 8
#define TIMEOUT 2000
#define avgSpeed 255

#define NUM_ANALOG_SENSORS 1
#define ANALOG_SAMPLE_RATE 4

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

//instantiate line following QTR sensor object and declare setup variables  
QTRSensorsRC qtrc;
PinName qtrc_pins[] = {(PTA1),(PTA2),(PTD4),(PTA12),(PTA4),(PTA5),(PTC8),(PTC9)};   
PinName emitterPin = NC;
unsigned int sensorValues[NUM_SENSORS];



//instantiate barcode sensor object
QTRSensorsAnalog qtra;
PinName qtra_pin = (PTC2);
unsigned int analog_sensorValues[NUM_ANALOG_SENSORS];


//readvalue function assigns 1 or 0 to sensor value depending on whether it is reading white or black
//TODO: test value at which sensor differentiates white or black
void readValues(){
    qtrc.read(sensorValues);
    for (int i=0; i<NUM_SENSORS; i++){
        if(sensorValues[i]>400)
        sensorValues[i]=1;
        else
        sensorValues[i]=0;
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
    
    //TODO: write speed and pwm to each motor 
    
    lastError=inst_error;
    
}


void barc0de_scan(){   
    qtra.read(analog_sensorValues);
    for (int i=0; i<NUM_SENSORS; i++){
        if(analog_sensorValues[i]>400)
        analog_sensorValues[i]=1;
        else
        analog_sensorValues[i]=0;
    }
}


int main(){
    
    //initialise line following sensor IC
    qtrc.init(&qtrc_pins[8], NUM_SENSORS, TIMEOUT, emitterPin);  // 800 us timeout, no emitter pin
    
    //initialise barcode sensor IC
    qtra.init(&qtra_pin, NUM_ANALOG_SENSORS, ANALOG_SAMPLE_RATE, emitterPin);
    
    // optional: wait for some input from the user, such as  a button press
 
    // then start calibration phase and move the sensors over both
    // reflectance extremes they will encounter in your application:
    
    int i;
    for (i = 0; i < 250; i++)  // make the calibration take about 5 seconds
    {
        qtrc.calibrate(QTR_EMITTERS_ON);
        wait(20);
    }
 
    // optional: signal that the calibration phase is now over and wait for further
    // input from the user, such as a button press
    
    
    while(1){
 
        while (analog_sensorValues == 0){ //while bar code scanner reads no line
        
        readValues();
        calculateProportional();
        follow_line();
        barc0de_scan();
       
        }
        
        //stop wheel motors
        //start solenoid motor
        //load ball
        //wait till ball is shot
        //stop solenoid motor
        //start wheel motor
        barc0de_scan();
    }        
    return 0;
}
