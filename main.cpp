#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>
#include <FEHRCS.h>
#include <FEHBattery.h>
#include <FEHServo.h>

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
FEHMotor servoFork(FEHMotor::Motor2, 9.0);
FEHServo servo(FEHServo::Servo0);
AnalogInputPin sensorFront(FEHIO::P1_0);
DigitalInputPin distanceSensor(FEHIO::P3_7);
AnalogInputPin cdsCell(FEHIO::P3_0);
AnalogInputPin right_opto(FEHIO::P0_3);
AnalogInputPin middle_opto(FEHIO::P0_5);
AnalogInputPin left_opto(FEHIO::P0_7);

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
                left_motor.SetPercent(15);
                right_motor.SetPercent(15);
                /* Drive */
                /* Code for if left sensor is on the line */
                if (left_opto.Value() < 2.9) {
                    state = LEFT; // update a new state
                }
                else if (right_opto.Value() < 2.9) {
                    state = RIGHT; // update a new state
                }
                break;
            // If the right sensor is on the line...
            case RIGHT:
                // Set motor powers for right turn
                left_motor.SetPercent(15);
                right_motor.SetPercent(0);
                /* Drive */
                if(middle_opto.Value() < 2.9) {
                    state = MIDDLE;
                }
                else if(left_opto.Value() < 2.9) {
                    state = LEFT;
                }
                break;
            // If the left sensor is on the line...
            case LEFT:
                /* Mirror operation of RIGHT state */
                left_motor.SetPercent(15);
                right_motor.SetPercent(0);
                if(middle_opto.Value() < 2.9) {
                    state = MIDDLE;
                }
                else if(right_opto.Value() < 2.9) {
                    state = RIGHT;
                }
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
        servo.SetPercent(-15);
        Sleep(2.0);
        servo.Stop();
        drive(2, 'b');
        // INSERT MOVE SERVO MAX DOWN
        servo.SetPercent(-15);
        Sleep(1.0);
        servo.Stop();
        Sleep(5.0);
        drive(2, 'f');
        // INSERT MOVE SERVO UP
        servo.SetPercent(15);
        Sleep(2.0);
        servo.Stop();
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
        // INSERT MOVE SERVO DOWN
        servo.SetPercent(-15);
        Sleep(2.0);
        servo.Stop();
        drive(2, 'b');
        // INSERT MOVE SERVO MAX DOWN
        servo.SetPercent(-15);
        Sleep(1.0);
        servo.Stop();
        Sleep(5.0);
        drive(2, 'f');
        // INSERT MOVE SERVO UP
        servo.SetPercent(15);
        Sleep(2.0);
        servo.Stop();
    } else {
        drive(3, 'f');
        Sleep(1.0);
        driveUsingLine(3.5);
        // INSERT MOVE SERVO DOWN
        servo.SetPercent(-15);
        Sleep(2.0);
        servo.Stop();
        drive(2, 'b');
        // INSERT MOVE SERVO MAX DOWN
        servo.SetPercent(-15);
        Sleep(1.0);
        servo.Stop();

        Sleep(5.0);
        drive(2, 'f');
        // INSERT MOVE SERVO UP
        servo.SetPercent(15);
        Sleep(2.0);
        servo.Stop();
    }
}

void appleBucket(char opr){
    if (opr == 'd') {
        drive(5, 'f');   // go to line
        Sleep(1.0);
        driveUsingLine(3);
        Sleep(0.5);
        // MOVE SERVO DOWN a little
        Sleep(1.0);
        drive(2, 'b');
        Sleep(1.0);
    } else {
        //
    }
}

int main(void)
{
    float x, y; //for touch screen
    // while (true) { // Infinite loop to keep checking for screen presses
    //     while (!LCD.Touch(&x, &y)); // Wait for screen to be pressed
    //     while (LCD.Touch(&x, &y));  // Wait for screen to be unpressed
    //     left_encoder.ResetCounts();
    //     right_encoder.ResetCounts();
    //     LCD.WriteLine(left_encoder.Counts());
    //     LCD.WriteLine(right_encoder.Counts());

    //     int l = RCS.GetLever();
    
    //     turn(100, 0); // Perform the turn when the screen is pressed
    //     LCD.Clear();
    // }

    LCD.WriteLine("Analog Optosensor Testing");
    LCD.WriteLine("Touch the screen");
    while(!LCD.Touch(&x,&y)); //Wait for screen to be pressed
    while(LCD.Touch(&x,&y)); //Wait for screen to be unpressed

    
    // servo.SetPercent(50);
    // Sleep(2.0);
    // servo.Stop();
    // drive(2, 'b');
    // // INSERT MOVE SERVO MAX DOWN
    // servo.SetPercent(-15);
    // Sleep(1.0);
    // servo.Stop();

    // // Print end message to screen
    LCD.Clear(BLACK);
    LCD.WriteLine("Test Finished");
    


    while (true){
        LCD.WriteLine(left_opto.Value());
        LCD.WriteLine(middle_opto.Value());
        LCD.WriteLine(right_opto.Value());

        Sleep(1.0);
        LCD.Clear();
    }
    

    // turn(101, 1);


    // turn(101, 1);     // turn right
    // drive(2, 'f');
    // driveUsingLine(2);
    // Sleep(1.0);
    // drive(2.5, 'b');
    // // OPTIONAL - ram into wall to straighten
    // turn(100, 1);
    // Sleep(0.5);
    // appleBucket('d'); // drop apple bucket off
    // drive(5, 'b');    // go back from apple bucket
    // turn(100, 0);     // turn to allign with levers
    
    // INSERT SERVO MAX UP
    // flipLever(l);      // flip lever
    // while (!LCD.Touch(&x, &y)); // Wait for screen to be pressed
    // while (LCD.Touch(&x, &y));  // Wait for screen to be unpressed
    // turn(95, 1);
    // Sleep(1.0);
    // drive(3, 'f');
    // Sleep(1.0);
    // turn(95, 1);

    // while (!LCD.Touch(&x, &y)); // Wait for screen to be pressed
    // while (LCD.Touch(&x, &y));  // Wait for screen to be unpressed
    // drive(7, 'b');
    
    // 0.26 for red, 0.4-0.5 for blue

}

// 232