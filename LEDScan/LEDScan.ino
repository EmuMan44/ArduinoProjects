#include <TimerOne.h>
#include <LEDMatrix.h>

//#define _INCLUDE_RED
#define _INCLUDE_GREEN
//#define _INCLUDE_BLUE

#define BUTTON                  0
#define SPEED_POT               A0

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

CLEDMatrix      g_LEDMatrix;

/**
 *  This function checks for button press that will be used to pause and
 *  unpaused the program.
 *
 *  @return Returns true if paused, false otherwise.
 */
boolean CheckPause(void)
{
    static boolean fPaused = false;
    static int prevButton = HIGH;
    int currButton = digitalRead(BUTTON);

    if (currButton != prevButton)
    {
        if (currButton == LOW)
        {
            fPaused = !fPaused;
        }
        prevButton = currButton;
    }

    return fPaused;
}   //CheckPause

/**
 *  This function checks for the button press to pause and unpasue the program.
 *  If it is unpasued, it also checks the pot value to  determine the delay of
 *  the spiral. In other words, the pot value will be controlling the spiral
 *  speed.
 */
void DelayWait(void)
{
    while (CheckPause())
    {
        // debounce delay.
        delay(100);
    }

    // Slow it down so it's visible.
    int pot = analogRead(SPEED_POT);
    delay(pot);
}   //DelayWait

/**
 *  This function is called once at the beginning of the program for
 *  initializing the system.
 */
void setup(void)
{
    pinMode(BUTTON, INPUT);
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
}   //setup

/**
 *  This function is called periodically when the program is running.
 */
void loop(void)
{
#ifdef _INCLUDE_RED
    for (int color = COLOR_GREEN; color <= COLOR_YELLOW; color += 2)
#else
    int color = COLOR_GREEN;
#endif
    {
        for (int row = 0; row < NUM_ROWS; row++)
        {
            for (int col = 0; col < NUM_COLS; col++)
            {
                g_LEDMatrix.SetPixelColor(row, col, color);
                DelayWait();
                g_LEDMatrix.SetPixelColor(row, col, COLOR_BLACK);
            }
        }
        
        for (int col = 0; col < NUM_COLS; col++)
        {
            for (int row = 0; row < NUM_ROWS; row++)
            {
                g_LEDMatrix.SetPixelColor(row, col, color);
                DelayWait();
                g_LEDMatrix.SetPixelColor(row, col, COLOR_BLACK);
            }
        }
    }
}   //loop
