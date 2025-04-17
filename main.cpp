#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>
#include <FEHRCS.h>
#include <FEHBattery.h>
#include <FEHServo.h>

// Can change the following if one motor is slower than another.
#define LEFT_MOTOR_CORRECTION_FACTOR 1.01
#define RIGHT_MOTOR_CORRECTION_FACTOR 0.99

// Set a motor percentage of no more than 50% power
#define MOTOR_PERCENTAGE 11.5 / (Battery.Voltage()) * 25

//Declarations for encoders & motors
DigitalEncoder right_encoder(FEHIO::P0_0);
DigitalEncoder left_encoder(FEHIO::P0_1);
FEHMotor right_motor(FEHMotor::Motor1,9.0);
FEHMotor left_motor(FEHMotor::Motor0,9.0);
FEHMotor servoFork(FEHMotor::Motor2, 9.0);
FEHServo servo(FEHServo::Servo3);
AnalogInputPin sensorFront(FEHIO::P1_0); 
AnalogInputPin cdsCell(FEHIO::P3_7);
AnalogInputPin right_opto(FEHIO::P1_0);
AnalogInputPin middle_opto(FEHIO::P1_2);
AnalogInputPin left_opto(FEHIO::P1_4);

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
    double start = TimeNow();
    while((left_encoder.Counts() < counts)  && (TimeNow() - start) < 4.0);

    //Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}

void turnVariable(int counts, int dir, float motorFactorLeft, float motorFactorRight) //using encoders
{
    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();
    //Set both motors to desired percent
    if (dir == 0) {
        right_motor.SetPercent(MOTOR_PERCENTAGE*motorFactorRight);
        left_motor.SetPercent(MOTOR_PERCENTAGE*motorFactorLeft);
    } else {
        right_motor.SetPercent(-MOTOR_PERCENTAGE*motorFactorRight);
        left_motor.SetPercent(-MOTOR_PERCENTAGE*motorFactorLeft);
    }
    // While the average of the left and right encoder is less than counts,
    // keep running motors
    double start = TimeNow();
    while((left_encoder.Counts() < counts)  && (TimeNow() - start) < 4.0);

    //Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}

// 0 for left, 1 for right
void turnOneOnly(int counts, int dir) //using encoders
{
    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();
    //Set both motors to desired percent
    if (dir == 0) {
        right_motor.SetPercent(MOTOR_PERCENTAGE);
        while((right_encoder.Counts()) < counts);
    } else {
        left_motor.SetPercent(-MOTOR_PERCENTAGE);
        while((left_encoder.Counts()) < counts);
    }
    // While the average of the left and right encoder is less than counts,
    // keep running motors
    

    //Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}

void drive(float distance, char dir)
{
    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    int counts = distance * 40.5;
    
    if (dir == 'f'){
        //Set both motors to desired percent
        right_motor.SetPercent(MOTOR_PERCENTAGE * 0.98);
        left_motor.SetPercent(-MOTOR_PERCENTAGE * LEFT_MOTOR_CORRECTION_FACTOR);
    } else if(dir == 'b'){
        //Set both motors to desired percent
        right_motor.SetPercent(-MOTOR_PERCENTAGE * RIGHT_MOTOR_CORRECTION_FACTOR);
        left_motor.SetPercent(MOTOR_PERCENTAGE * LEFT_MOTOR_CORRECTION_FACTOR);
    }

    //While the average of the left and right encoder is less than counts,
    //keep running motors
    double start = TimeNow();
    while(((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts) && (TimeNow() - start) < 8.0){}

    //Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}

void driveVariableSpeed(float distance, char dir, float percentage)
{
    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    int counts = distance * 40.5;
    
    if (dir == 'f'){
        //Set both motors to desired percent
        right_motor.SetPercent(percentage * RIGHT_MOTOR_CORRECTION_FACTOR);
        left_motor.SetPercent(-percentage * LEFT_MOTOR_CORRECTION_FACTOR);
    } else if(dir == 'b'){
        //Set both motors to desired percent
        right_motor.SetPercent(-percentage * RIGHT_MOTOR_CORRECTION_FACTOR);
        left_motor.SetPercent(percentage * LEFT_MOTOR_CORRECTION_FACTOR);
    }

    //While the average of the left and right encoder is less than counts,
    //keep running motors
    double start = TimeNow();
    while(((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts) && (TimeNow() - start) < 8.0){
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

void driveUsingLine(float distance){

    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    int counts = distance * 40.5;

    int state = MIDDLE; // Set the initial state

    while ((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts) { // I will follow this line for distance!
        switch(state) {
            // If I am in the middle of the line...
            case MIDDLE:
                left_motor.SetPercent(-15);
                right_motor.SetPercent(15);
                /* Drive */
                /* Code for if left sensor is on the line */
                if (left_opto.Value() < 2.5 && left_opto.Value() > 2.4) {
                    state = LEFT; // update a new state
                }
                // else if (right_opto.Value() < 2.9) {
                //     state = RIGHT; // update a new state
                // }
                break;
            // If the right sensor is on the line...
            // case RIGHT:
            //     // Set motor powers for right turn
            //     left_motor.SetPercent(15);
            //     right_motor.SetPercent(0);
            //     /* Drive */
            //     if(middle_opto.Value() < 2.9) {
            //         state = MIDDLE;
            //     }
            //     else if(left_opto.Value() < 2.9) {
            //         state = LEFT;
            //     }
            //     break;
            // If the left sensor is on the line...
            case LEFT:
                /* Mirror operation of RIGHT state */
                left_motor.SetPercent(0);
                right_motor.SetPercent(15);
                if(middle_opto.Value() < 3.05 && middle_opto.Value() > 2.95) {
                    state = MIDDLE;
                }
                // else if(right_opto.Value() < 2.9) {
                //     state = RIGHT;
                // }
                break;
            default: // Error. Something is very wrong.
                break;
        }
        Sleep(0.2);
    }
    left_motor.Stop();
    right_motor.Stop();
}

void pressHumidifier(){
    // 0.26 for red, 0.5 for blue
    char light = 's';
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
    turn(427, 1);

    left_motor.SetPercent(20);
    right_motor.SetPercent(-20);
    Sleep(3.0);
    left_motor.Stop();
    right_motor.Stop();

}

void flipLever(int l){
    if (l == 0){
        turn(100, 0);  // turn left
        Sleep(0.5);
        drive(2, 'f'); // allign with left lever
        Sleep(0.5);
        turn(100, 1);  // turn back
        Sleep(0.5);
        drive(3, 'f');
        Sleep(1.0);
        driveUsingLine(3.5);
        // INSERT MOVE SERVO DOWN
        servoFork.SetPercent(-15);
        Sleep(2.0);
        servoFork.Stop();
        drive(2, 'b');
        // INSERT MOVE servoFork MAX DOWN
        servoFork.SetPercent(-15);
        Sleep(1.0);
        servoFork.Stop();
        Sleep(5.0);
        drive(2, 'f');
        // INSERT MOVE servoFork UP
        servoFork.SetPercent(15);
        Sleep(2.0);
        servoFork.Stop();
    } else if (l == 2){
        turn(100, 1);  // turn right
        Sleep(0.5);
        drive(2, 'f'); // allign with right lever
        Sleep(0.5);
        turn(100, 0);  // turn back
        Sleep(0.5);
        drive(3, 'f');
        Sleep(1.0);
        driveUsingLine(3.5);
        // INSERT MOVE servoFork DOWN
        servoFork.SetPercent(-15);
        Sleep(2.0);
        servoFork.Stop();
        drive(2, 'b');
        // INSERT MOVE servoFork MAX DOWN
        servoFork.SetPercent(-15);
        Sleep(1.0);
        servoFork.Stop();
        Sleep(5.0);
        drive(2, 'f');
        // INSERT MOVE servoFork UP
        servoFork.SetPercent(15);
        Sleep(2.0);
        servoFork.Stop();
    } else {
        drive(3, 'f');
        Sleep(1.0);
        driveUsingLine(3.5);
        // INSERT MOVE servoFork DOWN
        servoFork.SetPercent(-15);
        Sleep(2.0);
        servoFork.Stop();
        drive(2, 'b');
        // INSERT MOVE servoFork MAX DOWN
        servoFork.SetPercent(-15);
        Sleep(1.0);
        servoFork.Stop();

        Sleep(5.0);
        drive(2, 'f');
        // INSERT MOVE servoFork UP
        servoFork.SetPercent(15);
        Sleep(2.0);
        servoFork.Stop();
    }
}

void appleBucket(char opr){
    if (opr == 'd') {
        drive(5, 'f');   // go to line
        Sleep(1.0);
        driveUsingLine(3);
        Sleep(0.5);
        // MOVE servoFork DOWN a little
        Sleep(1.0);
        drive(2, 'b');
        Sleep(1.0);
    } else {
        //
    }
}

void spinServo(int n){
    
    servo.SetDegree(n);
    
}

void compost(){
    Sleep(1.0);
    servoFork.SetPercent(-25);
    Sleep(2.9);
    servoFork.SetPercent(25);
    Sleep(2.9);
    servoFork.Stop();
}

int main(void)
{
    // RCS.InitializeTouchMenu("0150F7IJN");

    float x, y; //for touch screen
    int l = RCS.GetLever();

    LCD.WriteLine("Milestone 5");
    LCD.WriteLine("Touch the screen");
    
    servo.SetMin(822);
    servo.SetMax(2208);

    servo.SetDegree(170);

    // wait for starting light
    while(cdsCell.Value() > 0.5);

    // press start button
    left_motor.SetPercent(20);
    right_motor.SetPercent(-20);
    Sleep(0.8);
    left_motor.Stop();
    right_motor.Stop();

    // turn toward compost bin
    right_motor.SetPercent(20);
    Sleep(2.05);
    right_motor.Stop();

    // drive there
    drive(2.3, 'f');

    // correct and straigthen
    left_motor.SetPercent(-20);
    Sleep(0.9);
    left_motor.Stop();

    compost();

    // drive back
    Sleep(1.0);
    drive(2.5, 'b');

    // turn toward apple bucket and drive there
    turn(130, 1);
    drive(6.65, 'f');
    turn(135, 0);
    servo.SetDegree(74);
    Sleep(2.0);
    driveVariableSpeed(2.1, 'f', 15);
    servo.SetDegree(137);
    Sleep(1.0);

    // turn and drive to hit wall then allign with ramp
    drive(3.9, 'b');
    driveVariableSpeed(6, 'b', 35);
    Sleep(0.5);
    driveVariableSpeed(1.3, 'f', 20);
    Sleep(0.5);
    turn(212, 1);

    // drive up ramp
    driveVariableSpeed(9.8, 'f', 40);

    // turn and drive to drop off apple bucket
    Sleep(1.0);
    
    turn(85, 0);
    drive(7.1, 'f');
    turn(83, 1);
    // drive toward crate
    right_motor.SetPercent(15);
    left_motor.SetPercent(-15);
    Sleep(3.3);
    right_motor.Stop();
    left_motor.Stop();
    // drop off
    servo.SetDegree(45);
    Sleep(1.0);
    drive(5.3, 'b');

    // turn and flip levers
    Sleep(1.0);
    servo.SetDegree(170);
    turn(80, 0);

    // go and smack lever down
    left_motor.SetPercent(-20);
    right_motor.SetPercent(20);
    Sleep(2.4);
    left_motor.Stop();
    right_motor.Stop();
    servo.SetDegree(20);
    Sleep(3.0);

    drive(2.5, 'b');
    Sleep(1.0);

    //if the lever hasn't been flipped, go back and flip it
    // if(RCS.isLeverFlipped()==0){
    //     drive(2.5, 'f');
    //     servo.SetDegree(170);
    //     turn(20, 0);
    //     //go and smack lever down
    //     left_motor.SetPercent(-20);
    //     right_motor.SetPercent(20);
    //     Sleep(2.4);
    //     left_motor.Stop();
    //     right_motor.Stop();
    //     servo.SetDegree(20);
    //     Sleep(1.0);
    
    // }

    // go smack it back up
    Sleep(4.0);
    servo.SetDegree(0);
    left_motor.SetPercent(-20);
    right_motor.SetPercent(20);
    Sleep(1.5);
    left_motor.Stop();
    right_motor.Stop();
    servo.SetDegree(60);
    Sleep(1.0);
    drive(4.3, 'b');
    

    // drive back and align with humidifier
    servo.SetDegree(170);
    Sleep(1.0);
    turn(95, 0);
    drive(3, 'f');
    pressHumidifier();

    Sleep(2.0);

    // // turn a little to press button
    // left_motor.SetPercent(15);
    // Sleep(1.0);
    // left_motor.Stop();
    // Sleep(0.5);
    // left_motor.SetPercent(-15);
    // Sleep(1.0);
    // left_motor.Stop();


    drive(2.7, 'f');
    
    turn(205, 1);
    
    left_motor.SetPercent(-20);
    right_motor.SetPercent(20);
    Sleep(2.0);
    left_motor.Stop();
    right_motor.Stop();

    turnVariable(215, 1, 1.7, 1.7);

    // left_motor.SetPercent(-30);
    // right_motor.SetPercent(30);
    // Sleep(2.0);
    // left_motor.Stop();
    // right_motor.Stop();
    // turn(205, 1);
    
    // turn toward wall
    driveVariableSpeed(1.3, 'b', 20);
    Sleep(1.0);
    turn(70, 1);

    
    Sleep(1.0);
    //go back to final button
    drive(11, 'b');
    drive(1.7, 'f');
    //turn right so robot is backward
    turn(228, 1);
    //go allll the way back
    drive(16, 'b');
    turn(50, 0);
    drive(2, 'b');

}

// 232