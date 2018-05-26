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
int             g_rowLo = 0;
int             g_rowHi = NUM_ROWS - 1;
int             g_colLo = 0;
int             g_colHi = NUM_COLS - 1;
int             g_rowInc = 1;
int             g_colInc = 1;
int             g_spiralDir = 1;
int             g_color = COLOR_GREEN;
int             g_row = 0;
int             g_col = -1;

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
    // Erase the old pixel position.
    if (g_col != -1)
    {
        g_LEDMatrix.SetPixelColor(g_row, g_col, COLOR_BLACK);
    }

    g_col += g_colInc;
    if (g_col > g_colHi)
    {
        g_col--;
        g_rowInc = g_spiralDir;
        g_row += g_rowInc;
    }
    else if (g_col < g_colLo)
    {
        g_col++;
        g_rowInc = -g_spiralDir;
        g_row += g_rowInc;
    }

    if (g_row > g_rowHi)
    {
        g_row--;
        g_rowInc = -1;
        g_colInc = -g_spiralDir;
        g_col += g_colInc;
    }
    else if (g_row < g_rowLo)
    {
        g_row++;
        g_rowInc = 1;
        g_colInc = g_spiralDir;
        g_col += g_colInc;
    }

    g_LEDMatrix.SetPixelColor(g_row, g_col, g_color);

    if (((g_spiralDir == 1) &&
         (g_col == g_colLo) &&
         (g_row + g_rowInc == g_rowLo)) ||
        ((g_spiralDir == -1) &&
         (g_row == g_rowLo) &&
         (g_col + g_colInc < g_colLo)))
    {
        g_colLo += g_spiralDir;
        g_rowLo += g_spiralDir;
        g_colHi -= g_spiralDir;
        g_rowHi -= g_spiralDir;
        if (g_colLo < 0)
        {
            g_colLo = 0;
            g_colHi = NUM_COLS - 1;
            g_rowLo = 0;
            g_rowHi = NUM_ROWS - 1;
            g_spiralDir = 1;
            g_color += 2;
#ifdef _INCLUDE_RED
            if (g_color > COLOR_YELLOW)
            {
                g_color = COLOR_GREEN;
            }
#else
            if (g_color > COLOR_GREEN)
            {
                g_color = COLOR_GREEN;
            }
#endif
        }
        else if (g_colLo >= g_colHi)
        {
            g_colLo--;
            g_rowLo--;
            g_colHi++;
            g_rowHi++;
            g_spiralDir = -1;
        }
    }
    DelayWait();
}   //loop

