#include <TimerOne.h>
#include <LEDMatrix.h>

//#define _ADJUSTABLE_DELAY

//#define _INCLUDE_RED
#define _INCLUDE_GREEN
//#define _INCLUDE_BLUE

#define BUTTON_PORT             0
#define SPEED_POT               A0
#define RANDOM_SEED_PORT        A1

#define STATE_STOPPED           0
#define STATE_BUTTON_PRESSED    1
#define STATE_BUTTON_RELEASED   2

#define MAX_DICE_VALUE          9
#define NUM_DICE_VALUE          6
#define DEF_LOOP_DELAY          20
#define DEF_COLOR               COLOR_GREEN
#define KP                      16
#define NUM_SNAPSHOTS           8

//
// LED matrix pinouts.
//
const int g_rowPins[] =
{
    2, 3, 4, 5, A2, A3, A4, A5
};
#ifdef _INCLUDE_RED
const int g_redColPins[] =
{
    22, 23, 24, 25, 26, 27, 28, 29
};
#endif
#ifdef _INCLUDE_GREEN
const int g_greenColPins[] =
{
    13, 12, 11, 10, 9, 8, 7, 6
};
#endif
#ifdef _INCLUDE_BLUE
const int g_blueColPins[] =
{
    30, 31, 32, 33, 34, 35, 36, 37
};
#endif

//
// Bit maps for the dice faces.
//
unsigned char g_diceBitmaps[MAX_DICE_VALUE][NUM_ROWS] =
{
    0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00,  //Dice value 1
    0xc0, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03,  //Dice value 2
    0xc0, 0xc0, 0x00, 0x18, 0x18, 0x00, 0x03, 0x03,  //Dice value 3
    0xc3, 0xc3, 0x00, 0x00, 0x00, 0x00, 0xc3, 0xc3,  //Dice value 4
    0xc3, 0xc3, 0x00, 0x18, 0x18, 0x00, 0xc3, 0xc3,  //Dice value 5
    0xdb, 0xdb, 0x00, 0x00, 0x00, 0x00, 0xdb, 0xdb,  //Dice value 6
    0xdb, 0xdb, 0x00, 0x18, 0x18, 0x00, 0xdb, 0xdb,  //Dice value 7
    0xdb, 0xdb, 0x00, 0xc3, 0xc3, 0x00, 0xdb, 0xdb,  //Dice value 8
    0xdb, 0xdb, 0x00, 0xdb, 0xdb, 0x00, 0xdb, 0xdb,  //Dice value 9
};

CLEDMatrix      g_LEDMatrix;
int             g_color = DEF_COLOR;
int             g_state = STATE_STOPPED;
int             g_diceIndex = 0;
int             g_loopDelay = 0;
int             g_loopCnt = 0;
int             g_snapshotCnt = 0;
int             g_snapshotDelay = 0;

int NextDiceValue(int currValue)
{
    int nextValue = currValue + 1;

    if (nextValue >= NUM_DICE_VALUE)
    {
        nextValue = 0;
    }

    return nextValue;
}   //NextDiceValue

/**
 *  This function is called once at the beginning of the program for
 *  initializing the system.
 */
void setup(void)
{
    randomSeed(analogRead(RANDOM_SEED_PORT));
    pinMode(BUTTON_PORT, INPUT);
    g_LEDMatrix.Init(g_rowPins,
#ifdef _INCLUDE_RED
                     g_redColPins,
#else
                     NULL,
#endif
#ifdef _INCLUDE_GREEN
                     g_greenColPins,
#else
                     NULL,
#endif
#ifdef _INCLUDE_BLUE
                     g_blueColPins);
#else
                     NULL);
#endif
    g_LEDMatrix.DrawBuffer((g_color & COLOR_RED)?
                            &g_diceBitmaps[g_diceIndex][0]: NULL,
                           (g_color & COLOR_GREEN)?
                            &g_diceBitmaps[g_diceIndex][0]: NULL,
                           (g_color & COLOR_BLUE)?
                            &g_diceBitmaps[g_diceIndex][0]: NULL);
}   //setup

/**
 *  This function is called periodically when the program is running.
 */
void loop(void)
{
    switch (g_state)
    {
        case STATE_STOPPED:
            //
            // Waiting for button pressed.
            //
#ifdef _ADJUSTABLE_DELAY
            g_loopDelay = analogRead(SPEED_POT);
#else
            g_loopDelay = DEF_LOOP_DELAY;
#endif
            if (digitalRead(BUTTON_PORT) == LOW)
            {
                g_state++;
            }
            break;

        case STATE_BUTTON_PRESSED:
            if (digitalRead(BUTTON_PORT) == LOW)
            {
                g_diceIndex = NextDiceValue(g_diceIndex);
                g_LEDMatrix.DrawBuffer((g_color & COLOR_RED)?
                                        &g_diceBitmaps[g_diceIndex][0]: NULL,
                                       (g_color & COLOR_GREEN)?
                                        &g_diceBitmaps[g_diceIndex][0]: NULL,
                                       (g_color & COLOR_BLUE)?
                                        &g_diceBitmaps[g_diceIndex][0]: NULL);
            }
            else
            {
                g_diceIndex = random(0, NUM_DICE_VALUE);
                g_snapshotCnt = NUM_SNAPSHOTS;
                g_snapshotDelay = 10;
                g_loopCnt = 0;
                g_state++;
            }
            break;

        case STATE_BUTTON_RELEASED:
            if (g_snapshotCnt == 0)
            {
                g_state = STATE_STOPPED;
            }
            else
            {
                g_loopCnt++;
                if (g_loopCnt >= g_snapshotDelay/10)
                {
                    g_diceIndex = NextDiceValue(g_diceIndex);
                    g_LEDMatrix.DrawBuffer((g_color & COLOR_RED)?
                                            &g_diceBitmaps[g_diceIndex][0]:
                                            NULL,
                                           (g_color & COLOR_GREEN)?
                                            &g_diceBitmaps[g_diceIndex][0]:
                                            NULL,
                                           (g_color & COLOR_BLUE)?
                                            &g_diceBitmaps[g_diceIndex][0]:
                                            NULL);
                    g_snapshotCnt--;
                    g_snapshotDelay = g_snapshotDelay*KP/10;
                    g_loopCnt = 0;
                }
            }
            break;
    }
    delay(g_loopDelay);
}   //loop

