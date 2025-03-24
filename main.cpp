#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>
#include <FEHRCS.h>

// Can change the following if one motor is slower than another.
#define LEFT_MOTOR_CORRECTION_FACTOR 1
#define RIGHT_MOTOR_CORRECTION_FACTOR 1

// Set a motor percentage of no more than 50% power
#define MOTOR_PERCENTAGE 50 /* TODO: Insert formula for motor percent based on controller battery power */

//Declarations for encoders & motors
DigitalEncoder right_encoder(FEHIO::P0_0);
DigitalEncoder left_encoder(FEHIO::P0_1);
FEHMotor right_motor(FEHMotor::Motor1,9.0);
FEHMotor left_motor(FEHMotor::Motor0,9.0);
AnalogInputPin sensorFront(FEHIO::P1_0);
DigitalInputPin cdsCell(FEHIO::P3_0); // Light sensor

void turn(int percent, int counts, int dir) //using encoders
{
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
void turnOnlyLeftMotor(int percent, int counts, int dir) //using encoders
{
    //Reset encoder counts
  
    left_encoder.ResetCounts();
    //Set both motors to desired percent
    if (dir == 0) {
       
        left_motor.SetPercent(percent);
    } else {
        
        left_motor.SetPercent(-percent);
    }
    // While the average of the left and right encoder is less than counts,
    // keep running motors
    while((left_encoder.Counts()) < counts);

    //Turn off motors
    
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

    int counts = distance * 40.5;
    
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
        LCD.WriteLine(sensorFront.Value());
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

    int counts = distance * 40.5;
    
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
        LCD.WriteLine(sensorFront.Value());
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

    int counts = distance * 40.5;
    
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
        LCD.WriteLine(sensorFront.Value());
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
int main(void)
{
    float x,y;

    LCD.Clear(BLACK);
    LCD.SetFontColor(WHITE);
    //start on light
    LCD.WriteLine("Starting");
    while(cdsCell.Value() > 0.3){
        Sleep(0.5);
        LCD.WriteLine(cdsCell.Value());
    }
    
    turn(MOTOR_PERCENTAGE/2, 50, 1); //turn a little right
   
    Sleep(1.0);
    //drive to front of ramp
    LCD.Clear(BLACK);
    LCD.WriteLine("Going to Ramp");
    drive(4, 'f');
    Sleep(0.5);

    LCD.Clear(BLACK);
    LCD.WriteLine("Going up Ramp");
    ramp();

    LCD.Clear(BLACK);
    LCD.WriteLine("Opening Window");
    //drive forward and smack
    driveVariableSpeed(4, 'f', MOTOR_PERCENTAGE/2);
    Sleep(0.5);
    driveVariableSpeedALittleRight(4.6, 'f', MOTOR_PERCENTAGE/2);
    Sleep(0.5);
    turn(MOTOR_PERCENTAGE/2, 5,1);
    driveVariableSpeed(3, 'f', MOTOR_PERCENTAGE/2);
    Sleep(1.0);
    //turn to go around
    driveVariableSpeed(0.5,'b',MOTOR_PERCENTAGE/2);
    turn(MOTOR_PERCENTAGE/2, 25, 1); //turn a little right
    driveVariableSpeed(1.7,'f',MOTOR_PERCENTAGE/2);
    turn(MOTOR_PERCENTAGE/2, 40, 0); //turn a little left
    Sleep(0.5);
    //bring window back
    drive(4.5, 'b');


    // driveVariableSpeed(1,'b',MOTOR_PERCENTAGE/2);
    // turn(MOTOR_PERCENTAGE/2, 213, 1); //turn 90, clockwise
    // driveVariableSpeed(2,'f',MOTOR_PERCENTAGE/2);
    // turn(MOTOR_PERCENTAGE/2, 213, 0); //turn 90, clockwise
    // driveVariableSpeed(3,'f',MOTOR_PERCENTAGE/2);
    // turn(MOTOR_PERCENTAGE/2, 213, 0); //turn 90, clockwise
    // driveVariableSpeed(1,'f',MOTOR_PERCENTAGE/2);
    // turn(MOTOR_PERCENTAGE/2, 213, 1); //turn 90, clockwise
    // driveVariableSpeed(5, 'b', MOTOR_PERCENTAGE);


}