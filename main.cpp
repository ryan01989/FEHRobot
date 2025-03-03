#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>
#include <FEHRCS.h>

// Can change the following if one motor is slower than another.
#define LEFT_MOTOR_CORRECTION_FACTOR 1
#define RIGHT_MOTOR_CORRECTION_FACTOR 1

// Set a motor percentage of no more than 50% power
#define MOTOR_PERCENTAGE 25 /* TODO: Insert formula for motor percent based on controller battery power */

//Declarations for encoders & motors
DigitalEncoder right_encoder(FEHIO::P0_0);
DigitalEncoder left_encoder(FEHIO::P0_1);
FEHMotor right_motor(FEHMotor::Motor1,9.0);
FEHMotor left_motor(FEHMotor::Motor0,9.0);
AnalogInputPin sensorFront(FEHIO::P1_0);
DigitalInputPin distanceSensor(FEHIO::P3_7);

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

int main(void)
{
    int motor_percent = 22; //Input power level here
    int expected_counts = 243; //Input theoretical counts here
    float perInch = 40.5;

    float x, y; //for touch screen

    //Initialize the screen
    LCD.Clear(BLACK);
    LCD.SetFontColor(WHITE);

    RCS.InitializeTouchMenu("0150F7IJN");
    int lever = RCS.GetLever();
    LCD.WriteLine(RCS.Time());

}