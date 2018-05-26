#include <TimerOne.h>
#include <LEDMatrix.h>

//#define _INCLUDE_RED
#define _INCLUDE_GREEN
//#define _INCLUDE_BLUE

//#define _ALLOW_USER_INPUT
#define _CHANGE_ORIENTATION
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
int             g_scrollDir = SCROLL_LEFT;
int             g_color = COLOR_GREEN;
int             g_pixelDelay = 100;
int             g_orientation = ORIENTATION_UP_TOP;

/**
 *  This function check for button press that will be used to change scrolling
 *  direction.
 *
 *  @return Returns the scroll direction state.
 */
int CheckDir(void)
{
    static int prevButton = HIGH;
    int currButton = digitalRead(BUTTON);

    if (currButton != prevButton)
    {
        if (currButton == LOW)
        {
          #ifdef _CHANGE_ORIENTATION
            g_orientation++;
            if (g_orientation > ORIENTATION_LEFT_TOP)
            {
                if (g_scrollDir == SCROLL_NONE)
                {
                    g_scrollDir = SCROLL_LEFT;
                    g_orientation = ORIENTATION_UP_TOP;
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
                else
                {
                    g_scrollDir = SCROLL_NONE;
                    g_orientation--;
                }
            }
          #else
            g_scrollDir++;
            if (g_scrollDir > SCROLL_DOWN)
            {
                g_scrollDir = SCROLL_NONE;
            }
          #endif
        }
        prevButton = currButton;
        // debounce delay.
        delay(100);
    }

    return g_scrollDir;
}   //CheckDir

/**
 *  This function pauses the scrolling message if it is in PAUSE mode.
 *  Otherwise, it reads the analog pot to determine the delay between each
 *  pixel move. In effect, the pot is controlling the scrolling speed.
 */
void DelayWait(void)
{
    while (CheckDir() == SCROLL_NONE)
    {
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
#ifdef _ALLOW_USER_INPUT
    g_LEDMatrix.SetMessage("Hello World! ", g_scrollDir);
#endif
}   //setup

/**
 *  This function is called periodically when the program is running.
 *  There are actually two program modes in the following code. One is to
 *  scroll the message set in the setup() function one pixel each time the
 *  loop is called, check the button press for direction change and read
 *  the pot for controlling the delay of each loop that in turn controls
 *  the scrolling speed of the message.
 *  The other program mode is to scroll the message across the LED matrix
 *  in one direction, then changing the direction of scrolling once the
 *  message has completed scrolling in one direction.
 */
void loop(void)
{
#ifdef _ALLOW_USER_INPUT
    g_LEDMatrix.ScrollMsgPixel(g_scrollDir, g_color, g_orientation);
    CheckDir();
    DelayWait();
#else
    g_LEDMatrix.ScrollMessage(" Hello World! ",
                              g_scrollDir,
                              g_color,
                              g_pixelDelay,
                              g_orientation);
  #ifdef _CHANGE_ORIENTATION
    g_orientation++;
    if (g_orientation > ORIENTATION_LEFT_TOP)
    {
        g_orientation = ORIENTATION_UP_TOP;
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
  #else
    g_scrollDir++;
    if (g_scrollDir > SCROLL_DOWN)
    {
        g_scrollDir = SCROLL_LEFT;
    }
  #endif
#endif
}   //loop
