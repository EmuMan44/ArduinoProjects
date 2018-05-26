#define _MOTORMETER_V2

#include <SPI.h>
#include <Wire.h>
//http://www.adafruit.com/products/938
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Servo.h>
//http://playground.arduino.cc/Code/Button
#include <Button.h>
//http://playground.arduino.cc/Code/LED
//#include <LED.h>
//http://playground.arduino.cc/Code/Potentiometer
#include <Potentiometer.h>

//CSK 10/31/2013 Misc. menu alignment cleanup before product release

//*** I/0 Setup ***//
//    Analog I/O   //
#define SPEED_POT                  A0
#define MODE_SELECT_SWITCH         A1
//Available                        A2
#define CURRENT_SENSOR             A3
//I2C_DATA                         4
//I2C_CLOCK                        5

//    Digital I/0   //
//Digital I/O Pin Definitions
//USB RX                           0
//USB TX                           1
//Available                        2
//#define QTR_DETECTOR_PIN           3
//Pins 3, 5, 6, 9, 10, 11 are PWM
//Available                        4
#define OLED_RESET                 5

#ifdef _MOTORMETER_V2
#define MOTORDIRSW_FORWARD         6
#define MOTORDIRSW_REVERSE         7
#else
//Available                        6
//Available                        7
#endif

//Available                        8
//Available                        9
//CSK 10/14/2013 Changed to match Arduino Pro Mini custom interface PCB
//#define MODE_INCREMENT_SWITCH      6 //10
//#define Digital_servo_control      11
#define Digital_servo_control      10
#define MODE_INCREMENT_SWITCH      11
#define SERVO_RANGE_SELECT         12
//Available                        13
//*** END I/O Setup ***//

Servo myservo;  // create servo object to control a servo
//CSK 9/6/2013 This is an alias for the servo range selection.  Standard means 1000ms to 2000ms
#define STANDARD_SERVO_RANGE HIGH
#define POT_VALUE_IS_SERVO_POSITION 1
#define POT_VALUE_IS_SERVO_SPEED    0
//CSK 9/5/2013 These were measured from with HiTech Servo Test pulse measure feature
#define ARDUINO_SERVO_START 543
#define ARDUINO_SERVO_END   2395
int glb_servo_start  =  map(1000, ARDUINO_SERVO_START, ARDUINO_SERVO_END, 0, 180);
int glb_servo_end    =  map(2000, ARDUINO_SERVO_START, ARDUINO_SERVO_END, 0, 180);
int glb_servo_center =  90; //map(1500, ARDUINO_SERVO_START, ARDUINO_SERVO_END, 0, 180);
#define LARGE_SPEED_CHANGE_THRESHOLD	100

//http://playground.arduino.cc/Code/Button
Button mode_select_button = Button(MODE_SELECT_SWITCH,BUTTON_PULLUP);
Button mode_increment_button = Button(MODE_INCREMENT_SWITCH,BUTTON_PULLUP);
//http://playground.arduino.cc/Code/Potentiometer
Potentiometer Speed_Pot = Potentiometer(SPEED_POT);

//LED center_requested_LED = LED(CENTER_REQUESTED_LED);
//LED sweep_requested_LED  = LED(SWEEP_REQUESTED_LED);

////PWM timing 1000ms
//#define SERVO_START               (int)0
////PWM timing 2000ms
//#define SERVO_END                 (int)180
//#define SERVO_CENTER              (int)90

byte glb_byte_present_pwm_setting = glb_servo_center;

float glb_smooth_current_in_amps;
float glb_previous_current_in_amps;
float glb_current_calibration_offset = 0.25;

//Macro "Functions"
//-100A to 100A sensor
//CSK 6/25/2013 For the Allegro -100 to 100A ACS785LCB-100B-PFF Sensor
//float value = 200.0/1023.0 * analogRead(CURRENT_SENSOR) + (-100);
//#define A2D_TO_AMPS(X) ( (200.0/1023.0 * X) - 100)

//0A to 50A sensor
//CSK 6/25/2013 For the Allegro 0-50A 785LCB-050U-PFF Sensor
//Theoretical formula, 0Amps is ~0.6mV(125Counts), 50Amps is 1023Counts
// Amps = 50.0/(float)(1023-125) * analogRead(CURRENT_SENSOR) - 50.0/(float)(1023-125) * 125
// Amps = (0.0557 * analogRead(CURRENT_SENSOR)) - 6.9599
// When I compare to a Fluke 115 with an amprobe CT237A I ended up with this equation
//CSK 10/25/2013 Modified so it returns zero when result is .3 or less to eliminate annoying non-zero values when motor is off.
//#define A2D_TO_AMPS(X) ( ( constrain( (0.0837 * X)  - 10.436 , 0, 50 ) <= 0.3)?0:( constrain( (0.0837 * X)  - 10.436 , 0, 50 ) ) )
//CSK 11/26/2013 New equation for 100A sensor
#define A2D_TO_AMPS(X) ( ( ( ( ( 100.0/(1023.0 - 122.76) ) * X ) - 13.63 )  <= 0.3 )?0:( ( ( 100.0/(1023.0 - 122.76) ) * X ) - 13.63 ) )

#define INSTRUCTION_MODE   0
#define SET_SPEED          1
//#define CENTER             2
#define SWEEP              2
#define TALON_CALIBRATION  3
#ifdef _MOTORMETER_V2
#define SET_DIRSPEED       4
#define HIGHEST_MODE       4
#else
#define HIGHEST_MODE       3
#endif

int Mode_State = 0;

////***Polou QTR Sensor Setup***//
////http://www.pololu.com/catalog/product/2455
//#define NUM_SENSORS   1     // number of sensors used
//#define TIMEOUT       2500  // waits for 2500 microseconds for sensor outputs to go low
//QTRSensorsRC qtrrc((unsigned char[]) {QTR_L_1RC_RPM_SENSOR}, NUM_SENSORS, TIMEOUT, QTR_EMITTER_PIN);
unsigned int glb_RPM_Value = 0;
Adafruit_SSD1306 SSD1306_display(OLED_RESET);
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 96
//CSK 9/4/2013 Note: Fonts are not fixed width so width numbers are approx.
//Things like "i" and "l" take up less space that say "B".  This makes my centering function less than perfect.
#define CHAR_WIDTH_IN_PIXELS_FOR_FONTSIZE1  6
#define CHAR_WIDTH_FOR_FONTSIZE1            21
#define CHAR_HEIGHT_IN_PIXELS_FOR_FONTSIZE1 8
#define CHAR_WIDTH_IN_PIXELS_FOR_FONTSIZE2  12
#define CHAR_WIDTH_FOR_FONTSIZE2            12
#define CHAR_HEIGHT_IN_PIXELS_FOR_FONTSIZE2 18

void setup()
{
    //***USB serial config***//
    Serial.begin(9600);

    //***Servo setup***/
    myservo.attach(Digital_servo_control);  // attaches the servo object to PWM pin 3.

    //***IO Setup***//
    pinMode(MODE_SELECT_SWITCH, INPUT);
    digitalWrite(MODE_SELECT_SWITCH, HIGH);
    pinMode(MODE_INCREMENT_SWITCH, INPUT);
    digitalWrite(MODE_INCREMENT_SWITCH, HIGH);
    pinMode(SERVO_RANGE_SELECT, INPUT_PULLUP);
#ifdef _MOTORMETER_V2
    pinMode(MOTORDIRSW_FORWARD, INPUT_PULLUP);
    pinMode(MOTORDIRSW_REVERSE, INPUT_PULLUP);
#endif
    //CSK 10/30/2013 Added lines to disable pullup on current sensor
    pinMode(CURRENT_SENSOR, INPUT);
    digitalWrite(CURRENT_SENSOR, LOW);

    //***Adafruit display setup***//
    // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
    SSD1306_display.begin(SSD1306_SWITCHCAPVCC, 0x3D);  // initialize with the I2C addr 0x3D (for the 128x64)
    //SSD1306_display.begin(SSD1306_SWITCHCAPVCC);
    //With display.setTextSize(2) you can fit 10 characters in a row, good line spacing is 18pixels (e.g. Row 1 > 0,0; Row 2 > 0,18)
    //
    // init done

    //CSK 8/20/2013 Get really weird behavior if I don't show the splash screen, so just make it fast
    //
    SSD1306_display.display(); // show splashscreen
    SSD1306_display.setTextWrap(false);
    //delay(1);
    SSD1306_display.setTextSize(2);
    SSD1306_display.setTextColor(WHITE);
    for (int position = 128; position >= 24;position -= 8)
    {
        SSD1306_display.clearDisplay(); // clears the screen and buffer
        //Serial.println(F("Servo Tester started"));
        #define ANDYMARK_STARTING_ROW 10
        SSD1306_display.setCursor(position - 3, ANDYMARK_STARTING_ROW);
        SSD1306_display.print(F("AndyMark"));
        SSD1306_display.setCursor(position + 14, ANDYMARK_STARTING_ROW + CHAR_HEIGHT_IN_PIXELS_FOR_FONTSIZE2);
        SSD1306_display.print(F("Motor"));
        SSD1306_display.setCursor(position - 9, ANDYMARK_STARTING_ROW + (CHAR_HEIGHT_IN_PIXELS_FOR_FONTSIZE2 * 2));
#ifdef _MOTORMETER_V2
        SSD1306_display.print(F("Tester V2"));
#else
        SSD1306_display.print(F("Tester V1"));
#endif
        // Looks like LCD_generator tool doesn't work with the chipset in the 128X96 module???
        //   http://learn.adafruit.com/adafruit-gfx-graphics-library/graphics-primitives
        //   display.drawBitmap(0,0,AMlogo_Cropped_BW_26X32,26,32,WHITE);
        SSD1306_display.display();
    }
    delay(1000);
    SSD1306_display.clearDisplay();
}

void loop()
{
    //CSK 9/6/2013 Read the servo range jumper and set the value to corresponding degrees
    static boolean printed = false;
    if (digitalRead(SERVO_RANGE_SELECT) == STANDARD_SERVO_RANGE)
    {
        //CSK 9/30/2013 Standard servo range is 1ms - 2ms
        glb_servo_start  =  map(1000, ARDUINO_SERVO_START, ARDUINO_SERVO_END, 0, 180);
        glb_servo_end    =  map(2000, ARDUINO_SERVO_START, ARDUINO_SERVO_END, 0, 180);
    }
    else
    {
        //Arduino full range is ~0.5ms to ~2.3
        glb_servo_start  =  0;
        glb_servo_end    =  180;
    }

    //    Serial.print(glb_servo_start);
    //    Serial.print(" ");
    //    Serial.println(glb_servo_end);

    switch (Mode_State)
    {
        case INSTRUCTION_MODE:
            //Serial.println("main menu");
            print_mode_dialogs();
            instruction_mode();
            break;

#ifdef _MOTORMETER_V2
        case SET_DIRSPEED:
#endif
        case SET_SPEED:
            //Serial.println("set pwm menu");
            print_mode_dialogs();
            set_speed_requested();
            //CSK 8/23/2013 Set PWM to Center so motor stops.
            motor_wind_down();
            break;

#if 0
        case CENTER:
            //Serial.println("center menu");
            print_mode_dialogs();
            center_requested();
            //CSK 8/23/2013 Set PWM to Center so motor stops.
            motor_wind_down();
            break;
#endif

        case SWEEP:
            //Serial.println("sweep menu");
            print_mode_dialogs();
            sweep_requested();
            //CSK 8/23/2013 Set PWM to Center so motor stops.
            motor_wind_down();
            break;

        case TALON_CALIBRATION:
            print_mode_dialogs();
            talon_calibration();
            //Serial.println("talon menu");
            //break;

        default:
            /* Your code here */
            //CSK 8/23/2013 Set PWM to Center so motor stops.
            motor_wind_down();
            break;
    }

    if (!printed)
    {
        Serial.println(freeRam());
        printed = true;
    }
    //delay(10);
}

void instruction_mode()
{
    if (mode_increment_is_pressed())
    {
        increment_mode_state();
        SSD1306_display.clearDisplay();
    }
    return;
}

void print_mode_dialogs()
{
    if (Mode_State == INSTRUCTION_MODE)
    {
        SSD1306_display.setTextSize(1);
        SSD1306_display.setTextColor(WHITE);
        #define INSTRUCTION_STARTING_ROW 19
        SSD1306_display.setCursor(22,INSTRUCTION_STARTING_ROW);
        //http://playground.arduino.cc/Learning/Memory
        //CSK 8/23/2013 F() macro puts strings in flash memory rather than RAM
        SSD1306_display.print(F("Press Mode for"));
        SSD1306_display.setCursor(22, CHAR_HEIGHT_IN_PIXELS_FOR_FONTSIZE1 + INSTRUCTION_STARTING_ROW );
        SSD1306_display.print(F("Next Function."));
        SSD1306_display.setCursor(22, (CHAR_HEIGHT_IN_PIXELS_FOR_FONTSIZE1 * 2) + INSTRUCTION_STARTING_ROW );
        SSD1306_display.print(F("Press Select to"));
        SSD1306_display.setCursor(22, (CHAR_HEIGHT_IN_PIXELS_FOR_FONTSIZE1 * 3) + INSTRUCTION_STARTING_ROW );
        SSD1306_display.println(F("Enter Function."));
        SSD1306_display.display();
    }
    else
    {
        SSD1306_display.setTextSize(2);
        SSD1306_display.setTextColor(WHITE);
        #define MENU_STARTING_ROW 7
        SSD1306_display.setCursor(34, MENU_STARTING_ROW);
        SSD1306_display.print(F("Press"));
        SSD1306_display.setCursor(10,CHAR_HEIGHT_IN_PIXELS_FOR_FONTSIZE2 + MENU_STARTING_ROW);
        SSD1306_display.print(F("Select To"));
        switch (Mode_State)
        {
            case SET_SPEED:
                SSD1306_display.setCursor(17, (CHAR_HEIGHT_IN_PIXELS_FOR_FONTSIZE2 * 2) + MENU_STARTING_ROW );
                SSD1306_display.print(F("Set Spd>"));
                break;

#ifdef _MOTORMETER_V2
            case SET_DIRSPEED:
                SSD1306_display.setCursor(17, (CHAR_HEIGHT_IN_PIXELS_FOR_FONTSIZE2 * 2) + MENU_STARTING_ROW );
                SSD1306_display.print(F("Dir/Spd>"));
                break;
#endif
#if 0
            case CENTER:
                SSD1306_display.print(F(" Center>"));
                break;
#endif

            case SWEEP:
                SSD1306_display.setCursor(17,(CHAR_HEIGHT_IN_PIXELS_FOR_FONTSIZE2 * 2) + MENU_STARTING_ROW );
                SSD1306_display.print(F(" Sweep>"));
                break;

            case TALON_CALIBRATION:
                SSD1306_display.setCursor(6,(CHAR_HEIGHT_IN_PIXELS_FOR_FONTSIZE2 * 2) + MENU_STARTING_ROW );
                SSD1306_display.print(F("CAL Talon>"));
                break;

            default:
                break;
        }
        SSD1306_display.display();
    }
    return;
}
void increment_mode_state()
{
    SSD1306_display.clearDisplay();
    Mode_State++;
    if (Mode_State > HIGHEST_MODE)
    {
        Mode_State = 1;
    }
    return;
}

boolean mode_increment_is_pressed(void)
{
    boolean mode_button_pressed = false;
    if (mode_increment_button.isPressed())
    {
        //sweep_requested_LED.toggle();
        //Don't actually change states till they let go of the button
        while (mode_increment_button.isPressed())
        {
        }
        delay(10);
        mode_button_pressed = true;
    }
    return mode_button_pressed;
}

boolean select_is_pressed(void)
{
    boolean button_pressed = false;
    //Check for Center State button
    if (mode_select_button.isPressed())
    {
        //center_requested_LED.toggle();
        //Don't actually change states till they let go of the button
        while (mode_select_button.isPressed())
        {
        }
        delay(10);
        button_pressed = true;
    }
    return button_pressed;
}

void set_speed_requested()
{
    //CSK 11/26/2013 Trying to speed up behavior by reducing screen writing to just what changes
    boolean bool_fixed_text_displayed = false;
    boolean bool_update_screen = true;
    SSD1306_display.clearDisplay();

    if (select_is_pressed())
    {
        while (!select_is_pressed())
        {
            static byte byte_previous_pwm_setting = 0;

            //CSK 11/22/2013 Instead of rewriting whole display use	SSD1306_display.fillRect(locationx, locationy, width, height, color) to blank out previous number
            //This could speed up the loop and allow faster response times.

            //CSK 11/26/2013 Trying to speed up behavior by reducing screen writing to just what changes
            //SSD1306_display.clearDisplay();

            //Get PWM setting, Motor Current and RPM data
            //PWM
            //For default move servo pot value is servo position
            //boolean bool_pot_value_is_servo_position           = true;
            //If the requested speed is not where we are now, then ramp to speed to reduce inertial twisting and gear slamming
            if (glb_byte_present_pwm_setting > read_potentiometer(POT_VALUE_IS_SERVO_POSITION) &&
                glb_byte_present_pwm_setting > glb_servo_start)
            {
                glb_byte_present_pwm_setting--;
            }
            if (glb_byte_present_pwm_setting < read_potentiometer(POT_VALUE_IS_SERVO_POSITION) &&
                glb_byte_present_pwm_setting < glb_servo_end)
            {
                glb_byte_present_pwm_setting++;
            }
            //CSK 12/6/2012 Only write pwm if value has changed
            if (byte_previous_pwm_setting != glb_byte_present_pwm_setting)
            {
                //Serial.println(glb_int_present_pwm_setting);
                myservo.write(glb_byte_present_pwm_setting);
                byte_previous_pwm_setting = glb_byte_present_pwm_setting;
            }

            //Read current
            double temp_current_holder = readCurrent();

            //Read rpm
            readRPM();

            #define SPEED_STARTING_ROW	14
            if (bool_update_screen)
            {
                //CSK 11/26/2013 Trying to speed up behavior by reducing screen writing to just what changes
                if (!bool_fixed_text_displayed)
                {
                    //Write fixed display values
                    SSD1306_display.setTextSize(1);
                    SSD1306_display.setTextColor(BLACK,WHITE);
                    SSD1306_display.setCursor(32,2);
                    SSD1306_display.print(F("SPEED MODE"));
                    SSD1306_display.setTextSize(2);
                    SSD1306_display.setTextColor(WHITE);
                    SSD1306_display.setCursor(7, SPEED_STARTING_ROW);
                    SSD1306_display.print(F("PWM=    uS"));
                    SSD1306_display.setCursor( 7, CHAR_HEIGHT_IN_PIXELS_FOR_FONTSIZE2 + SPEED_STARTING_ROW );
                    SSD1306_display.print(F("  I=     A"));
                    if (glb_RPM_Value > 0)
                    {
                        SSD1306_display.setCursor(3, ( CHAR_HEIGHT_IN_PIXELS_FOR_FONTSIZE2 * 2 ) + SPEED_STARTING_ROW );
                        SSD1306_display.print(F("SPD=    "));
                        SSD1306_display.setTextSize(1);
                        SSD1306_display.setCursor(100,( CHAR_HEIGHT_IN_PIXELS_FOR_FONTSIZE2 * 2 ) + SPEED_STARTING_ROW + 6);
                        SSD1306_display.print(F("RPM"));
                    }

                    bool_fixed_text_displayed = true;
                }

                //This is data that is changing
                //CSK 11/26/2013 Trying to speed up behavior by reducing screen writing to just what changes
                //Instead of clearing and rewriting whole screen, just clear a rectangle where data goes
                SSD1306_display.setCursor(7 +  strlen("PWM=") * CHAR_WIDTH_FOR_FONTSIZE2, SPEED_STARTING_ROW);
                SSD1306_display.fillRect(7 +  strlen("PWM=") * CHAR_WIDTH_FOR_FONTSIZE2, SPEED_STARTING_ROW, (4 * CHAR_WIDTH_FOR_FONTSIZE2), CHAR_HEIGHT_IN_PIXELS_FOR_FONTSIZE2, BLACK);

                //Keep value right justified
                //CSK 96/2013 Work in uS for this
                int int_present_pwm_setting_in_us = map(glb_byte_present_pwm_setting, 0, 180, ARDUINO_SERVO_START, ARDUINO_SERVO_END);

                if (int_present_pwm_setting_in_us < 1000)
                {
                    SSD1306_display.print(F(" "));
                }
#if 0
                else if (glb_int_present_pwm_setting >= 10 &&
                         glb_int_present_pwm_setting < 100)
                {
                    SSD1306_display.print(F("   "));
                }
                else
                {
                    SSD1306_display.print(F(""));
                }
#endif

                SSD1306_display.print(int_present_pwm_setting_in_us); //map(glb_byte_present_pwm_setting, 0, 180, ARDUINO_SERVO_START, ARDUINO_SERVO_END));
                //CSK 8/21/2013 This device uses this extended ascii table set http://www.asciitable.pro/ascii_table.htm
                //http://forums.adafruit.com/viewtopic.php?f=47&t=30092
                ////247 is the degree symbol from this table
                //SSD1306_display.write(247);

                //This is data that is changing
                //CSK 11/26/2013 Trying to speed up behavior by reducing screen writing to just what changes
                //Instead of clearing and rewriting whole screen, just clear a rectangle where data goes
                SSD1306_display.setCursor(7 + strlen("I  =")*CHAR_WIDTH_FOR_FONTSIZE2,
                                          CHAR_HEIGHT_IN_PIXELS_FOR_FONTSIZE2 + SPEED_STARTING_ROW);
                SSD1306_display.fillRect(7 + strlen("I  =")*CHAR_WIDTH_FOR_FONTSIZE2,
                                         CHAR_HEIGHT_IN_PIXELS_FOR_FONTSIZE2 + SPEED_STARTING_ROW,
                                         5*CHAR_WIDTH_FOR_FONTSIZE2,
                                         CHAR_HEIGHT_IN_PIXELS_FOR_FONTSIZE2,
                                         BLACK );

                //Keep value right justified
                if (temp_current_holder < 10.0)
                {
                    SSD1306_display.print(F(" "));
                }
                else if (temp_current_holder >= 10.0 && temp_current_holder < 100)
                {
                    SSD1306_display.print(F(""));
                }
                SSD1306_display.print(temp_current_holder,1);

                if (glb_RPM_Value > 0)
                {
                    SSD1306_display.setCursor(3 + strlen("SPD=")*CHAR_WIDTH_FOR_FONTSIZE2,
                                              (CHAR_HEIGHT_IN_PIXELS_FOR_FONTSIZE2*2) + SPEED_STARTING_ROW);
                    SSD1306_display.fillRect(3 + strlen("SPD=")*CHAR_WIDTH_FOR_FONTSIZE2,
                                             (CHAR_HEIGHT_IN_PIXELS_FOR_FONTSIZE2*2) + SPEED_STARTING_ROW,
                                             4*CHAR_WIDTH_FOR_FONTSIZE2,
                                             CHAR_HEIGHT_IN_PIXELS_FOR_FONTSIZE2,
                                             BLACK);

                    //Keep value right justified
                    //If value is one digit
                    if (glb_RPM_Value < 10)
                    {
                        SSD1306_display.print(F("   "));
                    }
                    //if value is 2 digits
                    else if (glb_RPM_Value >= 10 && glb_RPM_Value < 100)
                    {
                        SSD1306_display.print(F("  "));
                    }
                    //if value is 3 digits
                    else if (glb_RPM_Value >= 100 && glb_RPM_Value < 1000)
                    {
                        SSD1306_display.print(F(" "));
                    }
                    else
                    {
                        //SSD1306_display.print(F(""));
                    }
                    SSD1306_display.print(glb_RPM_Value);
                }
                SSD1306_display.display();

                bool_update_screen = false;
            }
            else
            {
                bool_update_screen = true;
            }

        }
        //CSK 12/16/2013
        SSD1306_display.clearDisplay();		
    }

    if (mode_increment_is_pressed())
    {
        increment_mode_state();
    }
    return;
}

int read_potentiometer(boolean bool_pot_value_is_servo_position)
{
    byte byte_pot_value_scaled = 0;

    //If we are in a simple servo position state then map pot A2D values to servo degrees
    if (bool_pot_value_is_servo_position)
    {
        //http://playground.arduino.cc/Code/Potentiometer
        //CSK 11/14/2013 Swapped to match potentiometer mounting on PC board.
        //byte_pot_value_scaled = map(Speed_Pot.getValue(), 0, 1023, glb_servo_start, glb_servo_end);
#ifdef _MOTORMETER_V2
        if (Mode_State == SET_DIRSPEED)
        {
            if (digitalRead(MOTORDIRSW_FORWARD) == LOW)
            {
                byte_pot_value_scaled = map(1023 - Speed_Pot.getValue(),
                                            0, 1023,
                                            glb_servo_center, glb_servo_end);
            }
            else if (digitalRead(MOTORDIRSW_REVERSE) == LOW)
            {
                byte_pot_value_scaled = map(1023 - Speed_Pot.getValue(),
                                            0, 1023,
                                            glb_servo_center, glb_servo_start);
            }
            else
            {
                byte_pot_value_scaled = glb_servo_center;
            }
            //Serial.print(Speed_Pot.getValue());
            //Serial.print(" ");
            //Serial.println((int)byte_pot_value_scaled);
        }
        else
        {
            byte_pot_value_scaled = map(Speed_Pot.getValue(),
                                        0, 1023,
                                        glb_servo_end, glb_servo_start);
        }
#else
        byte_pot_value_scaled = map(Speed_Pot.getValue(),
                                    0, 1023,
                                    glb_servo_end, glb_servo_start);
#endif
        //CSK 6/18/2013 Force readings near the endpoints to the endpoints to eliminate flutter on the speed control
        if (byte_pot_value_scaled >= glb_servo_end - 5)
        {
            byte_pot_value_scaled = glb_servo_end;
        }
        if (byte_pot_value_scaled <= glb_servo_start + 5)
        {
            byte_pot_value_scaled = glb_servo_start;
        }
        //CSK 6/25/2013 Dead band neutral zone
        if (byte_pot_value_scaled >= 86 && byte_pot_value_scaled <= 94)
        {
            byte_pot_value_scaled = 90;
        }
    }
    //If we are in servo sweep state then map pot A2D values to time Xms/degree
    else
    {
        //http://playground.arduino.cc/Code/Potentiometer
        //byte_pot_value_scaled = map(Speed_Pot.getValue(), 0, 1023, 100, 0);
        //CSK 11/18/2013 reversed to match the potentiometer mounting on PC board
        byte_pot_value_scaled = map(Speed_Pot.getValue(), 0, 1023, 0, 100);
    }
    return byte_pot_value_scaled;
}

#define NUM_READS 50
#define NUM_TO_AVERAGE 10
float readCurrent(void)
{
    //http://www.elcojacobs.com/eleminating-noise-from-sensor-readings-on-arduino-with-digital-filtering/
    // read multiple values and sort them to take the mode
    //CSK 9/9/2013 eliminate float calculations inside loop to try to speed up this function
    //float sortedValues[NUM_READS];
    int sortedValues[NUM_READS];
    for (byte read_counter = 0; read_counter < NUM_READS; read_counter++)
    {
        //CSK 9/9/2013 eliminate float calculations inside loop to try to speed up this function
        //float A2D_TO_AMPS( analogRead(CURRENT_SENSOR) );
        int value = analogRead(CURRENT_SENSOR);
        byte insert_position;
        if (value < sortedValues[0] || read_counter == 0)
        {
            insert_position = 0; //insert at first position
        }
        else
        {
            for (insert_position = 1; insert_position < read_counter; insert_position++)
            {
                if (sortedValues[insert_position-1] <= value &&
                    sortedValues[insert_position] >= value)
                {
                    break;
                }
            }
        }
        for (byte k = read_counter; k > insert_position; k--)
        {
            // move all values higher than current reading up one position
            sortedValues[k] = sortedValues[k-1];
        }
        sortedValues[insert_position] = value; //insert current reading
    }

    //return average of NUM_TO_AVERAGE values
    float filtered_current = 0;
    for (byte read_counter = NUM_READS/2 - NUM_TO_AVERAGE/2;
         read_counter < NUM_READS/2 + NUM_TO_AVERAGE/2;
         read_counter++)
    {
        filtered_current += sortedValues[read_counter];
    }

    filtered_current = filtered_current/NUM_TO_AVERAGE;

    //CSK 6/25/2013 Average this result with the last value we displayed for a little more smoothing
    filtered_current = (filtered_current + glb_previous_current_in_amps)/2;

    glb_previous_current_in_amps = filtered_current;
    //CSK 6/25/2013 Keep the display from bouncing when it should be 0.0A
    //CSK 10/25/2013 Modified so it doesn't run calculation when we force current to zero
    //CSK 11/20/2013 Modified to handle A2D from Allegro 100A sensor ACS758LCB-100U-PFF
    //Serial.println(filtered_current, 1);
    return A2D_TO_AMPS( filtered_current );
}

void readRPM(void)
{
#if 0
    //Need to make app/ for IR RPM to analog output so we don't have to suffer delays trying to read pulses here
    //http://www.pololu.com/catalog/product/2455
    // read raw sensor values
    delayMicroseconds(100);
    pinMode(QTR_DETECTOR_PIN, INPUT);
    for (int milliseconds = 0; milliseconds <= 250; milliseconds++)
    {
        glb_RPM_Value = glb_RPM_Value + digitalRead(QTR_DETECTOR_PIN);
        delayMicroseconds(800);
    }
    glb_RPM_Value = glb_RPM_Value/2;
#endif
    return;
}

#if 0
void center_requested(void)
{
   #pragma region center_selected
   if (select_is_pressed())
   {
      SSD1306_display.clearDisplay();
      while (!select_is_pressed())
      {
         //If the requested speed is far not where we are now, then ramp to speed to reduce inertial twisting and gear slamming
         while (glb_byte_present_pwm_setting != glb_servo_center)
         {
            SSD1306_display.setTextSize(1);
            SSD1306_display.setTextColor(BLACK,WHITE);
            SSD1306_display.setCursor(30,2);
            SSD1306_display.print(F("Center MODE"));
            SSD1306_display.setTextSize(2);
            SSD1306_display.setTextColor(WHITE);
            //SSD1306_display.setCursor(44,20);
            //SSD1306_display.print(F("PWM"));
            //SSD1306_display.setCursor(9,38);
            SSD1306_display.setCursor(9,20);
            SSD1306_display.print(F("Centering"));
            SSD1306_display.display();

            myservo.write(glb_byte_present_pwm_setting);

            if (glb_byte_present_pwm_setting > glb_servo_center)
            {
               glb_byte_present_pwm_setting--;
               //delay(5);
            }
            else
            {
               glb_byte_present_pwm_setting++;
               //delay(5);
            }
         }
         SSD1306_display.clearDisplay();
         SSD1306_display.setTextSize(1);
         SSD1306_display.setTextColor(BLACK,WHITE);
         SSD1306_display.setCursor(30,2);
         SSD1306_display.print(F("Center MODE"));
         SSD1306_display.setTextSize(2);
         SSD1306_display.setTextColor(WHITE);
         SSD1306_display.setCursor(18,22);
         SSD1306_display.print(F("Centered"));
         SSD1306_display.display();
         if (select_is_pressed())
         {
            break;
         }

      }
      SSD1306_display.clearDisplay();
   }
   #pragma endregion center_selected

   if (mode_increment_is_pressed())
   {
      increment_mode_state();
   }
   return;
}
#endif

void sweep_requested(void)
{
    #pragma region sweep_selected
    if (select_is_pressed())
    {
        SSD1306_display.clearDisplay();
        SSD1306_display.setTextSize(2);
        while (!select_is_pressed())
        {
            int int_requested_speed = 0;

            int_requested_speed = read_potentiometer(POT_VALUE_IS_SERVO_SPEED);

            //Sweep one way each time
            if (glb_byte_present_pwm_setting == glb_servo_end)
            {
                //If the servo is at 180 work your way back down to 0 degrees
                while (glb_byte_present_pwm_setting != glb_servo_start)
                {
                    if (select_is_pressed())
                    {
                        goto EndSweep;
                    }
                    int_requested_speed = read_potentiometer(POT_VALUE_IS_SERVO_SPEED);
                    myservo.write(glb_byte_present_pwm_setting);
                    display_pwm("SWEEP MODE");
                    delay(int_requested_speed);
                    glb_byte_present_pwm_setting--;
                }
            }
            else
            {
                while (glb_byte_present_pwm_setting != glb_servo_end)
                {
                    if (select_is_pressed())
                    {
                        goto EndSweep;
                    }
                    int_requested_speed = read_potentiometer(POT_VALUE_IS_SERVO_SPEED);
                    myservo.write(glb_byte_present_pwm_setting);
                    display_pwm("SWEEP MODE");
                    delay(int_requested_speed);
                    glb_byte_present_pwm_setting++;
                }
            }
        }
EndSweep:
        SSD1306_display.clearDisplay();
    }
    #pragma endregion sweep_selected
    if (mode_increment_is_pressed())
    {
        increment_mode_state();
    }
    return;
}

void talon_calibration(void)
{
    #define TALON_STARTING_ROW	14
    if (select_is_pressed())
    {
        //SSD1306_display.clearDisplay();
        while (!select_is_pressed())
        {
            //Set to 0 when pot value represents speed
            int int_pot_value_is_servo_position = 0;
            int int_requested_speed             = 0;

            SSD1306_display.clearDisplay();
            SSD1306_display.setTextSize(1);
            //Serial.println(center_string_in_screen_width("Press and hold Red", 1));
            //SSD1306_display.setCursor( center_string_in_screen_width("Press and hold Red", 1), CHAR_HEIGHT_IN_PIXELS_FOR_FONTSIZE1 );
            SSD1306_display.setCursor(23, TALON_STARTING_ROW);
            SSD1306_display.print(F("Press and hold"));
            //SSD1306_display.setCursor(center_string_in_screen_width("CAL Button on Talon", 1), CHAR_HEIGHT_IN_PIXELS_FOR_FONTSIZE1 * 2);
            SSD1306_display.setCursor(23, CHAR_HEIGHT_IN_PIXELS_FOR_FONTSIZE1 + TALON_STARTING_ROW);
            SSD1306_display.print(F("Red CAL Button"));
            SSD1306_display.setCursor(40, CHAR_HEIGHT_IN_PIXELS_FOR_FONTSIZE1*2 + TALON_STARTING_ROW);
            SSD1306_display.print(F("on Talon"));			
            SSD1306_display.setCursor(32, CHAR_HEIGHT_IN_PIXELS_FOR_FONTSIZE1*3 + TALON_STARTING_ROW);
            SSD1306_display.print(F("Push SEL to"));
            SSD1306_display.setCursor(38, CHAR_HEIGHT_IN_PIXELS_FOR_FONTSIZE1*4 + TALON_STARTING_ROW);
            SSD1306_display.print(F("start CAL"));
            SSD1306_display.display();

            //VVV CSK 11/18/2013 Added extra "press SEL" step to allow users to get set up
            int int_elapsed_time = 0;
            while (!select_is_pressed())
            {
                //Wait for user to press Select button again to start CAL or check for timeout
                int_elapsed_time++;
                //Use multiple of 1/2sec delays to keep response to button press good
                delay(500);
                if (int_elapsed_time >= 20)
                {
                    goto UserTimout;
                }
            }
            //^^^ CSK 11/18/2013
            //delay(3000);

            myservo.write(glb_servo_start);
            SSD1306_display.clearDisplay();
            SSD1306_display.setTextSize(2);
            //Serial.println(center_string_in_screen_width("Done", 2));
            //SSD1306_display.setCursor(center_string_in_screen_width("Setting", 2),CHAR_HEIGHT_IN_PIXELS_FOR_FONTSIZE2);
            SSD1306_display.setCursor( 22, CHAR_HEIGHT_IN_PIXELS_FOR_FONTSIZE2 );
            SSD1306_display.print(F("Setting"));
            SSD1306_display.setCursor(34,CHAR_HEIGHT_IN_PIXELS_FOR_FONTSIZE2 * 2);
            SSD1306_display.print(F("Start"));
            SSD1306_display.display();
            delay(1000);
            myservo.write(glb_servo_end);
            SSD1306_display.clearDisplay();
            SSD1306_display.setTextSize(2);
            SSD1306_display.setCursor(22, CHAR_HEIGHT_IN_PIXELS_FOR_FONTSIZE2);
            SSD1306_display.print(F("Setting"));
            SSD1306_display.setCursor(46,CHAR_HEIGHT_IN_PIXELS_FOR_FONTSIZE2 * 2);
            SSD1306_display.print(F("End"));
            SSD1306_display.display();
            delay(1000);
            ////Center the servo
            myservo.write(glb_servo_center);
            SSD1306_display.clearDisplay();
            SSD1306_display.setTextSize(2);
            SSD1306_display.setCursor(22, CHAR_HEIGHT_IN_PIXELS_FOR_FONTSIZE2);
            SSD1306_display.print(F("Setting"));
            SSD1306_display.setCursor(28,CHAR_HEIGHT_IN_PIXELS_FOR_FONTSIZE2 * 2);
            SSD1306_display.print(F("Center"));
            SSD1306_display.display();
            delay(1000);
            SSD1306_display.clearDisplay();
            SSD1306_display.setTextSize(2);
            SSD1306_display.setCursor(40,CHAR_HEIGHT_IN_PIXELS_FOR_FONTSIZE1 * 2);
            SSD1306_display.print(F("Done"));
            SSD1306_display.setTextSize(1);
            SSD1306_display.setCursor(19,CHAR_HEIGHT_IN_PIXELS_FOR_FONTSIZE1 * 5);
            SSD1306_display.print(F("Release Red Cal"));
            SSD1306_display.setCursor(19,(CHAR_HEIGHT_IN_PIXELS_FOR_FONTSIZE1 * 6) );
            SSD1306_display.print(F("Button on Talon"));
            SSD1306_display.setCursor(7,(CHAR_HEIGHT_IN_PIXELS_FOR_FONTSIZE1 * 7) );
            SSD1306_display.print(F("Press SEL to Finish"));
            SSD1306_display.display();

            SSD1306_display.display();
            while (!select_is_pressed());
            goto EndCAL;
        }
        //VVV CSK 11/18/2013 Added extra "press SEL" stuff to allow users to get set up
        UserTimout:
        SSD1306_display.clearDisplay();
        SSD1306_display.setTextSize(2);
        SSD1306_display.setCursor(12, CHAR_HEIGHT_IN_PIXELS_FOR_FONTSIZE2);
        SSD1306_display.print(F("Timed out"));
        SSD1306_display.display();
        delay(3000);
        //^^^ CSK 11/18/2013

EndCAL:
        SSD1306_display.clearDisplay();
    }
    if (mode_increment_is_pressed())
    {
        increment_mode_state();
    }
    return;
}

void display_pwm(String function_name_string)
{
    SSD1306_display.clearDisplay();
    SSD1306_display.setTextSize(1);
    SSD1306_display.setCursor(center_string_in_screen_width(function_name_string, 1), 2);
    SSD1306_display.setTextColor(BLACK,WHITE);
    SSD1306_display.print(function_name_string);
    SSD1306_display.setTextSize(2);
    SSD1306_display.setCursor(6,26);
    SSD1306_display.setTextColor(WHITE);
    SSD1306_display.print(F("PWM="));
    //Keep value right justified
    int int_present_pwm_setting_in_us = map(glb_byte_present_pwm_setting,
                                            0, 180,
                                            ARDUINO_SERVO_START, ARDUINO_SERVO_END);
    if (int_present_pwm_setting_in_us < 1000)
    {
        SSD1306_display.print(F(" "));
    }
#if 0
    else if (glb_int_present_pwm_setting >= 10 && glb_int_present_pwm_setting < 100)
    {
        SSD1306_display.print(F("   "));
    }
#endif
    else
    {
        SSD1306_display.print(F(""));
    }
    SSD1306_display.print(map(glb_byte_present_pwm_setting,
                              0, 180,
                              ARDUINO_SERVO_START, ARDUINO_SERVO_END));
#if 0
    if (glb_byte_present_pwm_setting < 10)
    {
        SSD1306_display.print(F("  "));
    }
    else if (glb_byte_present_pwm_setting >= 10 && glb_byte_present_pwm_setting < 100)
    {
        SSD1306_display.print(F(" "));
    }
    else if (glb_byte_present_pwm_setting >= 100 )
    {
        SSD1306_display.print(F(""));
    }
    SSD1306_display.print(glb_byte_present_pwm_setting);
    //CSK 8/21/2013 This device uses this extended ascii table set http://www.asciitable.pro/ascii_table.htm
    //http://forums.adafruit.com/viewtopic.php?f=47&t=30092
    SSD1306_display.write(247);
#endif
    SSD1306_display.print(F("uS"));
    SSD1306_display.display();
    return;
}

void motor_wind_down()
{
    if (glb_byte_present_pwm_setting > glb_servo_center)
    {
        //If the servo is at 180 work your way back down to 0 degrees
        while (glb_byte_present_pwm_setting != glb_servo_center)
        {
            //int_requested_speed = read_potentiometer(int_pot_value_is_servo_position);
            myservo.write(glb_byte_present_pwm_setting);
            //display_sweep_pwm();
            delay(10);
            glb_byte_present_pwm_setting--;
        }
    }
    else
    {
        while (glb_byte_present_pwm_setting != glb_servo_center)
        {
            //int_requested_speed = read_potentiometer(int_pot_value_is_servo_position);
            myservo.write(glb_byte_present_pwm_setting);
            //display_sweep_pwm();
            delay(10);
            glb_byte_present_pwm_setting++;
        }
    }
    return;
}

int center_string_in_screen_width(String string2print, int fontsize)
{
    if ((fontsize == 1) && (string2print.length() < CHAR_WIDTH_FOR_FONTSIZE1) ||
        (fontsize == 2) && (string2print.length() < CHAR_WIDTH_FOR_FONTSIZE2))
    {
        return ( SCREEN_WIDTH - ( string2print.length() * (fontsize == 1?CHAR_WIDTH_IN_PIXELS_FOR_FONTSIZE1:CHAR_WIDTH_IN_PIXELS_FOR_FONTSIZE2) ) )/2;
    }
    else
    {
        return 0;
    }
}

//Added this function to tell me how much RAM is left. CSK 3/12/2012
//http://jeelabs.org/2011/05/22/atmega-memory-use/
int freeRam ()
{
    //When freeRam reported 202 this code crashed every time.  Reporting 404 as of 8/23/2013 and crashes stopped
    extern int __heap_start, *__brkval;
    int v;
    return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

