#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>

//Declarations for encoders & motors
DigitalEncoder right_encoder(FEHIO::P0_0);
DigitalEncoder left_encoder(FEHIO::P0_1);
FEHMotor right_motor(FEHMotor::Motor1,9.0);
FEHMotor left_motor(FEHMotor::Motor0,9.0);
AnalogInputPin sensorFront(FEHIO::P1_0);

//testing
void move_forward(int percent, int counts) //using encoders
{
    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    //Set both motors to desired percent
    right_motor.SetPercent(percent+5);
    left_motor.SetPercent(-percent);

    //While the average of the left and right encoder is less than counts,
    //keep running motors
    bool wallHit = false;
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts && !wallHit){
        LCD.WriteLine(sensorFront.Value());
        Sleep(500);
        if(sensorFront.Value()<=0.2){
            //stop when a wall is hit
            right_motor.SetPercent(0);
            left_motor.SetPercent(0);
            LCD.Clear(BLACK);
            LCD.WriteLine("Wall hit"); 
            wallHit = true;
        }
    }

    //Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}
void move_backward(int percent, int counts) //using encoders
{
    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    //Set both motors to desired percent
    right_motor.SetPercent(-percent+5);
    left_motor.SetPercent(percent);

    //While the average of the left and right encoder is less than counts,
    //keep running motors
    bool wallHit = false;
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts && !wallHit){
        LCD.WriteLine(sensorFront.Value());
        Sleep(500);
        if(sensorFront.Value()<=0.2){
            //stop when a wall is hit
            right_motor.SetPercent(0);
            left_motor.SetPercent(0);
            LCD.Clear(BLACK);
            LCD.WriteLine("Wall hit");
            wallHit=true;
        }
    }

    //Turn off motors
    right_motor.SetPercent(0);
    left_motor.SetPercent(0);
}

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

int main(void)
{
    int motor_percent = 22; //Input power level here
    int expected_counts = 243; //Input theoretical counts here
    float perInch = 40.5;

    float x, y; //for touch screen

    //Initialize the screen
    LCD.Clear(BLACK);
    LCD.SetFontColor(WHITE);

    
    LCD.WriteLine("Touch the screen");
    while(!LCD.Touch(&x,&y)); //Wait for screen to be pressed
    
    LCD.Clear(BLACK);
    LCD.WriteLine("Moving Forward");
    move_forward(motor_percent, 30*perInch); // drive 14 inches and stop at wall
    //wait for user to reposition robot 
    while(!LCD.Touch(&x,&y)); //Wait for screen to be pressed
    
    LCD.Clear(BLACK);
    LCD.WriteLine("Moving Forwards up ramp");
    move_forward(motor_percent*1.33, ((25)*perInch)); // drive up ramp and stop
    LCD.Clear(BLACK);
    LCD.WriteLine("Moving Backwards down ramp");
    move_backward(motor_percent, ((25)*perInch)); // drive back down ramp and stop

    return 0;
}