#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>

//Declarations for encoders & motors
DigitalEncoder right_encoder(FEHIO::P0_0);
DigitalEncoder left_encoder(FEHIO::P0_1);
DigitalInputPin frontBump(FEHIO::P0_2);
DigitalInputPin backBump(FEHIO::P0_3);
FEHMotor right_motor(FEHMotor::Motor1,9.0);
FEHMotor left_motor(FEHMotor::Motor0,9.0);

void moveForward(int percent, int counts) //using encoders
{
    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    //Set both motors to desired percent
    right_motor.SetPercent(percent);
    left_motor.SetPercent(percent);

    //While the average of the left and right encoder is less than counts,
    //keep running motors
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);

    //Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}

// 0 for left, 1 for right
void turn(int percent, int counts, int dir) //using encoders
{
    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();
    //Set both motors to desired percent
    if (dir == 0) {
        right_motor.SetPercent(percent);
        left_motor.SetPercent(-percent);
    } else {
        right_motor.SetPercent(-percent);
        left_motor.SetPercent(percent);
    }
    // While the average of the left and right encoder is less than counts,
    // keep running motors
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);

    //Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}

int main(void)
{
    int motor_percent = 22; //Input power level here
    int expected_counts = 243; //Input theoretical counts here

    float x, y; //for touch screen

    //Initialize the screen
    LCD.Clear(BLACK);
    LCD.SetFontColor(WHITE);

    LCD.WriteLine("Performance Test 1");
    LCD.WriteLine("Touch the screen");
    while(!LCD.Touch(&x,&y)); //Wait for screen to be pressed
    while(LCD.Touch(&x,&y)); //Wait for screen to be unpressed

    while (frontBump.Value()){
        right_motor.SetPercent(motor_percent);
        left_motor.SetPercent(motor_percent);
    }
    right_motor.Stop();
    left_motor.Stop();
    Sleep(0.5);
    moveForward(-motor_percent, 500);
    turn(motor_percent, 220, 1);
    moveForward(motor_percent, 405);
    Sleep(0.5);
    moveForward(-motor_percent, 405);
    


    // Sleep(2.0); //Wait for counts to stabilize
    // //Print out data
    // LCD.Write("Theoretical Counts: ");
    // LCD.WriteLine(expected_counts);
    // LCD.Write("Motor Percent: ");
    // LCD.WriteLine(motor_percent);
    // LCD.Write("Actual LE Counts: ");
    // LCD.WriteLine(left_encoder.Counts());
    // LCD.Write("Actual RE Counts: ");
    // LCD.WriteLine(right_encoder.Counts());

    return 0;
}

