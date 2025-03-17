#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>
#include <FEHRCS.h>
#include <FEHBattery.h>

// Can change the following if one motor is slower than another.
#define LEFT_MOTOR_CORRECTION_FACTOR 1.01
#define RIGHT_MOTOR_CORRECTION_FACTOR 1

// Set a motor percentage of no more than 50% power
#define MOTOR_PERCENTAGE 11.5 / (Battery.Voltage()) * 25

//Declarations for encoders & motors
DigitalEncoder right_encoder(FEHIO::P0_0);
DigitalEncoder left_encoder(FEHIO::P0_1);
FEHMotor right_motor(FEHMotor::Motor1,9.0);
FEHMotor left_motor(FEHMotor::Motor0,9.0);
AnalogInputPin sensorFront(FEHIO::P1_0);
DigitalInputPin distanceSensor(FEHIO::P3_7);
AnalogInputPin cdsCell(FEHIO::P3_0);

enum LineStates {
    MIDDLE,
    RIGHT,
    LEFT
};


// 0 for left, 1 for right
void turn(int counts, int dir) //using encoders
{
    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();
    //Set both motors to desired percent
    if (dir == 0) {
        right_motor.SetPercent(MOTOR_PERCENTAGE);
        left_motor.SetPercent(MOTOR_PERCENTAGE);
    } else {
        right_motor.SetPercent(-MOTOR_PERCENTAGE);
        left_motor.SetPercent(-MOTOR_PERCENTAGE);
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

    while (distanceSensor.Value()){}

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
        right_motor.SetPercent(MOTOR_PERCENTAGE * RIGHT_MOTOR_CORRECTION_FACTOR);
        left_motor.SetPercent(-MOTOR_PERCENTAGE * LEFT_MOTOR_CORRECTION_FACTOR);
    } else if(dir == 'b'){
        //Set both motors to desired percent
        right_motor.SetPercent(-MOTOR_PERCENTAGE * RIGHT_MOTOR_CORRECTION_FACTOR);
        left_motor.SetPercent(MOTOR_PERCENTAGE * LEFT_MOTOR_CORRECTION_FACTOR);
    }

    //While the average of the left and right encoder is less than counts,
    //keep running motors
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts ){
        // LCD.WriteLine("LEFT:");
        // LCD.WriteLine(left_encoder.Counts());
        // LCD.WriteLine("RIGHT:");
        // LCD.WriteLine(right_encoder.Counts());
        // Sleep(0.5);
    }

    //Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}

void pressHumidifier(){
    // 0.26 for red, 0.5 for blue
    char light = 'r';
    int i;
    Sleep(2.0);
    LCD.SetFontColor(RED);
    LCD.DrawRectangle(0, 0, 355, 255);
    LCD.FillRectangle(0, 0, 355, 255);
    for (i = 0; i < 20; i++){
        LCD.SetFontColor(WHITE);
        LCD.WriteLine(cdsCell.Value());
    }
    if (cdsCell.Value() < 0.43 && cdsCell.Value() > 0.19){   // change to check for red light
        light = 'r';
        LCD.Clear();
        LCD.SetFontColor(RED);
        LCD.DrawRectangle(0, 0, 355, 255);
        LCD.FillRectangle(0, 0, 355, 255);
    } else if(cdsCell.Value() < 0.85 && cdsCell.Value() > 0.4){ // change to check for blue light
        light = 'b';
        LCD.Clear();
        LCD.SetFontColor(BLUE);
        LCD.DrawRectangle(0, 0, 355, 255);
        LCD.FillRectangle(0, 0, 355, 255);
    }

    if (light == 'r'){
        turn(191, 1);
        drive(1.1, 'f');
        turn(191, 0);
        drive(5.2, 'f');
        drive(1.0, 'b');
        turn(191, 1);
        drive(2.4, 'b');
        turn(191, 0);
    } else if (light == 'b'){
        turn(191, 0);
        drive(1.1, 'f');
        turn(191, 1);
        drive(5.2, 'f');
        drive(1.0, 'b');
        turn(191, 0);
        drive(2.4, 'b');
        turn(205, 1);
    }

}

int main(void)
{
    float x, y; //for touch screen

    //Initialize the screen
    // LCD.Clear(BLACK);
    // LCD.SetFontColor(WHITE);

    // RCS.InitializeTouchMenu("0150F7IJN");
    // int lever = RCS.GetLever();
    // LCD.WriteLine(RCS.Time());
    // LCD.Clear();
    // LCD.WriteLine("Touch to Start");
    // LCD.Clear();
    LCD.WriteLine(cdsCell.Value());
    while(!LCD.Touch(&x, &y)){}
    turn(184, 1);
    // // orient to go forward
    // drive(3, 'f');
    // turn(95, 0);
    // drive(16, 'f');
    // // turn right toward window
    // turn(210, 1);
    // drive(12, 'f');
    // // right to be paralell with window
    // turn(210, 1);
    // drive(3, 'f');
    // // go back and turn to move out
    // drive(2, 'b');
    // turn(210, 1);
    // drive(2, 'f');
    // turn(210, 0);
    // drive(4, 'f');
    // turn(210, 0);
    // drive(2, 'f');
    // turn(210, 1);
    // // now alligned with window
    // drive(3, 'b');



    
    
    // 0.26 for red, 0.4-0.5 for blue

}

// 232