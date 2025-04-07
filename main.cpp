#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>
#include <FEHRCS.h>
#include <FEHServo.h>
#include <FEHBattery.h>

// Can change the following if one motor is slower than another.
#define LEFT_MOTOR_CORRECTION_FACTOR 1
#define RIGHT_MOTOR_CORRECTION_FACTOR 1

// Set a motor percentage of no more than 50% power
#define MOTOR_PERCENTAGE 25 /* TODO: Insert formula for motor percent based on controller battery power */
#define COUNTS_PER_INCH 32.5 // TODO: Calibrate this value
//Declarations for encoders & motors
DigitalEncoder right_encoder(FEHIO::P0_7);
DigitalEncoder left_encoder(FEHIO::P0_1);
FEHMotor right_motor(FEHMotor::Motor1,9.0);
FEHMotor left_motor(FEHMotor::Motor0,9.0);
//optosensor
AnalogInputPin optoLeft(FEHIO::P1_0);
AnalogInputPin optoMiddle(FEHIO::P1_1);
AnalogInputPin optoRight(FEHIO::P1_2);
//cds
DigitalInputPin cdsCell(FEHIO::P3_0); // Light sensor
//servos
FEHServo servo(FEHServo::Servo3); // Servo for composter
FEHMotor forkliftServo(FEHMotor::Motor2,9.0); // Forklift hacked servo

void turn(int percent, int degrees, int dir) //using encoders
{
    int counts = degrees*(COUNTS_PER_INCH/360);
    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();
    //Set both motors to desired percent
    if (dir == 0) {
        right_motor.SetPercent(percent);
        left_motor.SetPercent(percent);
    } else {
        right_motor.SetPercent(-percent);
        left_motor.SetPercent(-percent);
    }
    // While the average of the left and right encoder is less than counts,
    // keep running motors
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);

    //Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}
void turnOnlyOneMotor(int percent, int degrees, char dir, char motor) //using encoders
{
    //get encoder counts
    int counts = degrees*(COUNTS_PER_INCH/360);
    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();
    //Set both motors to desired percent
    if(motor == 'l')
    {
        if (dir == 'l') {
            left_motor.SetPercent(percent);
        } else if(dir== 'r'){ 
            left_motor.SetPercent(-percent);
        }
        while((left_encoder.Counts()) < counts);
    }
    
    if (motor== 'r') {
        if (dir == 'l') {
            right_motor.SetPercent(percent);
        } else if(dir== 'r'){
            right_motor.SetPercent(-percent);
        }
        while((right_encoder.Counts()) < counts);
    }
    //Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}

void driveUntilSensorDetected()
{
    // Drive backwards until an object is detected
    left_motor.SetPercent(-1 * LEFT_MOTOR_CORRECTION_FACTOR * MOTOR_PERCENTAGE);
    right_motor.SetPercent(-1 * RIGHT_MOTOR_CORRECTION_FACTOR * MOTOR_PERCENTAGE);

    /* TODO: Drive until a wall is detected */

    left_motor.Stop();
    right_motor.Stop();
}

void drive(float distance, char dir)
{
    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    int counts = distance * COUNTS_PER_INCH;
    
    if (dir == 'f'){
        //Set both motors to desired percent
        right_motor.SetPercent(MOTOR_PERCENTAGE+RIGHT_MOTOR_CORRECTION_FACTOR);
        left_motor.SetPercent(-MOTOR_PERCENTAGE);
    } else if(dir == 'b'){
        //Set both motors to desired percent
        right_motor.SetPercent(-MOTOR_PERCENTAGE+RIGHT_MOTOR_CORRECTION_FACTOR);
        left_motor.SetPercent(MOTOR_PERCENTAGE);
    }

    //While the average of the left and right encoder is less than counts,
    //keep running motors
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts ){
       
        Sleep(500);
    }

    //Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}
void driveVariableSpeedALittleRight(float distance, char dir, float motor_percentage)
{
    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    int counts = distance * COUNTS_PER_INCH;
    
    if (dir == 'f'){
        //Set both motors to desired percent
        right_motor.SetPercent(motor_percentage+RIGHT_MOTOR_CORRECTION_FACTOR);
        left_motor.SetPercent(-motor_percentage-60);
    } else if(dir == 'b'){
        //Set both motors to desired percent
        right_motor.SetPercent(-motor_percentage+RIGHT_MOTOR_CORRECTION_FACTOR);
        left_motor.SetPercent(motor_percentage+60);
    }

    //While the average of the left and right encoder is less than counts,
    //keep running motors
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts ){
       
        Sleep(500);
    }

    //Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}
void driveVariableSpeed(float distance, char dir, float motor_percentage)
{
    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    int counts = distance * COUNTS_PER_INCH;
    
    if (dir == 'f'){
        //Set both motors to desired percent
        right_motor.SetPercent(motor_percentage+RIGHT_MOTOR_CORRECTION_FACTOR);
        left_motor.SetPercent(-motor_percentage);
    } else if(dir == 'b'){
        //Set both motors to desired percent
        right_motor.SetPercent(-motor_percentage+RIGHT_MOTOR_CORRECTION_FACTOR);
        left_motor.SetPercent(motor_percentage);
    }

    //While the average of the left and right encoder is less than counts,
    //keep running motors
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts ){
        Sleep(500);
    }

    //Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}
void ramp(){
    //assume robot is at front of ramp facing forward
    //turn robot 180 degrees
    turn(MOTOR_PERCENTAGE/2, 190, 1); //turn 180 
    drive(9,'b');
    Sleep(0.5);
    driveVariableSpeed(0.5,'f', MOTOR_PERCENTAGE/2);
    turn(MOTOR_PERCENTAGE/2, 93, 1); //turn 90, clockwise
}
void startSequence(){
    //clear screen
    LCD.Clear(BLACK);
    LCD.SetFontColor(WHITE);
    //set servo parameters
    servo.SetMin(800);
    servo.SetMax(2200);
    servo.SetDegree(0);
    //start on light
    LCD.WriteLine("Starting");
    
    LCD.WriteAt("Battery: ", 160, 220);
    LCD.WriteAt(((Battery.Voltage()/11.5)*100),270,220);


    while(cdsCell.Value() > 0.3){
        Sleep(0.5);
        LCD.WriteLine(cdsCell.Value());
    }
}
int main(void)
{

    
    //wait for light
         //startSequence();
   //go to composter
   //pick up apples
   //go to ramp
   //go up ramp
         //ramp();
   //drop off apples
   //center over second horizontal line
   //turn 30 degrees left
   //drive 19 in
   //turn 120 right

   //fertilizer
   //go 6 in back
   //turn 135 left

   // humidifier
   //go to window loc
   //open window
   //go to ramp
   //return to start
    
   
    

}