#ifndef _LEDMATRIX_H
#define _LEDMATRIX_H

#include <Arduino.h>
#include <TimerOne.h>

//
// Hardware Constants.
//
#define NUM_ROWS                8
#define NUM_COLS                8
#define FRAME_RATE              60
//
// Scroll directions.
//
#define SCROLL_NONE             0
#define SCROLL_LEFT             1
#define SCROLL_RIGHT            2
#define SCROLL_UP               3
#define SCROLL_DOWN             4
//
// LED matrix orientations.
//
#define ORIENTATION_UP_TOP      0
#define ORIENTATION_RIGHT_TOP   1
#define ORIENTATION_DOWN_TOP    2
#define ORIENTATION_LEFT_TOP    3
//
// Color definitions and macros.
//
#define COLOR_BLACK             0x00
#define COLOR_BLUE              0x01
#define COLOR_GREEN             0x02
#define COLOR_RED               0x04
#define COLOR_CYAN              (COLOR_BLUE | COLOR_GREEN)
#define COLOR_MAGENTA           (COLOR_BLUE | COLOR_RED)
#define COLOR_YELLOW            (COLOR_GREEN | COLOR_RED)
#define COLOR_WHITE             (COLOR_BLUE | COLOR_GREEN | COLOR_RED)

#define RED_VALUE(d)            (((d) >> 16) & 0xff)
#define GREEN_VALUE(d)          (((d) >> 8) & 0xff)
#define BLUE_VALUE(d)           ((d) & 0xff)
#define RGB(r,g,b)              (((unsigned long)(r) << 16) |  \
                                 ((unsigned long)(g) << 8) |   \
                                 (b))
//
// Bit maps for 8x8 font.
//
static const unsigned char s_font8x8[] =
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //00
    0x7E, 0x81, 0xA5, 0x81, 0xBD, 0x99, 0x81, 0x7E, //01 white smiley
    0x7E, 0xFF, 0xDB, 0xFF, 0xC3, 0xE7, 0xFF, 0x7E, //02 black smiley
    0x6C, 0xFE, 0xFE, 0xFE, 0x7C, 0x38, 0x10, 0x00, //03 heart
    0x10, 0x38, 0x7C, 0xFE, 0x7C, 0x38, 0x10, 0x00, //04 diamond
    0x38, 0x7C, 0x38, 0xFE, 0xFE, 0x7C, 0x38, 0x7C, //05 club
    0x10, 0x10, 0x38, 0x7C, 0xFE, 0x7C, 0x38, 0x7C, //06 spade
    0x00, 0x00, 0x18, 0x3C, 0x3C, 0x18, 0x00, 0x00, //07 dot
    0xFF, 0xFF, 0xE7, 0xC3, 0xC3, 0xE7, 0xFF, 0xFF, //08 black dot
    0x00, 0x3C, 0x66, 0x42, 0x42, 0x66, 0x3C, 0x00, //09 circle
    0xFF, 0xC3, 0x99, 0xBD, 0xBD, 0x99, 0xC3, 0xFF, //0a black circle
    0x0F, 0x07, 0x0F, 0x7D, 0xCC, 0xCC, 0xCC, 0x78, //0b male
    0x3C, 0x66, 0x66, 0x66, 0x3C, 0x18, 0x7E, 0x18, //0c female
    0x3F, 0x33, 0x3F, 0x30, 0x30, 0x70, 0xF0, 0xE0, //0d one note
    0x7F, 0x63, 0x7F, 0x63, 0x63, 0x67, 0xE6, 0xC0, //0e two notes
    0x99, 0x5A, 0x3C, 0xE7, 0xE7, 0x3C, 0x5A, 0x99, //0f star
    0x80, 0xE0, 0xF8, 0xFE, 0xF8, 0xE0, 0x80, 0x00, //10 right arrow
    0x02, 0x0E, 0x3E, 0xFE, 0x3E, 0x0E, 0x02, 0x00, //11 left arrow
    0x18, 0x3C, 0x7E, 0x18, 0x18, 0x7E, 0x3C, 0x18, //12 up-down arrow
    0x66, 0x66, 0x66, 0x66, 0x66, 0x00, 0x66, 0x00, //13 double exclamation
    0x7F, 0xDB, 0xDB, 0x7B, 0x1B, 0x1B, 0x1B, 0x00, //14 PI
    0x3E, 0x63, 0x38, 0x6C, 0x6C, 0x38, 0xCC, 0x78, //15 Section
    0x00, 0x00, 0x00, 0x00, 0x7E, 0x7E, 0x7E, 0x00, //16 dash?
    0x18, 0x3C, 0x7E, 0x18, 0x7E, 0x3C, 0x18, 0xFF, //17 ???
    0x18, 0x3C, 0x7E, 0x18, 0x18, 0x18, 0x18, 0x00, //18 up arrow
    0x18, 0x18, 0x18, 0x18, 0x7E, 0x3C, 0x18, 0x00, //19 down arrow
    0x00, 0x18, 0x0C, 0xFE, 0x0C, 0x18, 0x00, 0x00, //1a right arrow
    0x00, 0x30, 0x60, 0xFE, 0x60, 0x30, 0x00, 0x00, //1b left arrow
    0x00, 0x00, 0xC0, 0xC0, 0xC0, 0xFE, 0x00, 0x00, //1c lower left corner
    0x00, 0x24, 0x66, 0xFF, 0x66, 0x24, 0x00, 0x00, //1d left-right arrow
    0x00, 0x18, 0x3C, 0x7E, 0xFF, 0xFF, 0x00, 0x00, //1e up triangle
    0x00, 0xFF, 0xFF, 0x7E, 0x3C, 0x18, 0x00, 0x00, //1f down triangle
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //20 space
    0x30, 0x78, 0x78, 0x30, 0x30, 0x00, 0x30, 0x00, //21 !
    0x6C, 0x6C, 0x6C, 0x00, 0x00, 0x00, 0x00, 0x00, //22 "
    0x6C, 0x6C, 0xFE, 0x6C, 0xFE, 0x6C, 0x6C, 0x00, //23 #
    0x30, 0x7C, 0xC0, 0x78, 0x0C, 0xF8, 0x30, 0x00, //24 $
    0x00, 0xC6, 0xCC, 0x18, 0x30, 0x66, 0xC6, 0x00, //25 %
    0x38, 0x6C, 0x38, 0x76, 0xDC, 0xCC, 0x76, 0x00, //26 &
    0x60, 0x60, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, //27 '
    0x18, 0x30, 0x60, 0x60, 0x60, 0x30, 0x18, 0x00, //28 (
    0x60, 0x30, 0x18, 0x18, 0x18, 0x30, 0x60, 0x00, //29 )
    0x00, 0x66, 0x3C, 0xFF, 0x3C, 0x66, 0x00, 0x00, //2a *
    0x00, 0x30, 0x30, 0xFC, 0x30, 0x30, 0x00, 0x00, //2b +
    0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x60, //2c ,
    0x00, 0x00, 0x00, 0xFC, 0x00, 0x00, 0x00, 0x00, //2d -
    0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x00, //2e .
    0x06, 0x0C, 0x18, 0x30, 0x60, 0xC0, 0x80, 0x00, //2f /
    0x7C, 0xC6, 0xCE, 0xDE, 0xF6, 0xE6, 0x7C, 0x00, //30 0
    0x30, 0x70, 0x30, 0x30, 0x30, 0x30, 0xFC, 0x00, //31 1
    0x78, 0xCC, 0x0C, 0x38, 0x60, 0xCC, 0xFC, 0x00, //32 2
    0x78, 0xCC, 0x0C, 0x38, 0x0C, 0xCC, 0x78, 0x00, //33 3
    0x1C, 0x3C, 0x6C, 0xCC, 0xFE, 0x0C, 0x1E, 0x00, //34 4
    0xFC, 0xC0, 0xF8, 0x0C, 0x0C, 0xCC, 0x78, 0x00, //35 5
    0x38, 0x60, 0xC0, 0xF8, 0xCC, 0xCC, 0x78, 0x00, //36 6
    0xFC, 0xCC, 0x0C, 0x18, 0x30, 0x30, 0x30, 0x00, //37 7
    0x78, 0xCC, 0xCC, 0x78, 0xCC, 0xCC, 0x78, 0x00, //38 8
    0x78, 0xCC, 0xCC, 0x7C, 0x0C, 0x18, 0x70, 0x00, //39 9
    0x00, 0x30, 0x30, 0x00, 0x00, 0x30, 0x30, 0x00, //3a :
    0x00, 0x30, 0x30, 0x00, 0x00, 0x30, 0x30, 0x60, //3b ;
    0x18, 0x30, 0x60, 0xC0, 0x60, 0x30, 0x18, 0x00, //3c <
    0x00, 0x00, 0xFC, 0x00, 0x00, 0xFC, 0x00, 0x00, //3d =
    0x60, 0x30, 0x18, 0x0C, 0x18, 0x30, 0x60, 0x00, //3e >
    0x78, 0xCC, 0x0C, 0x18, 0x30, 0x00, 0x30, 0x00, //3f ?
    0x7C, 0xC6, 0xDE, 0xDE, 0xDE, 0xC0, 0x78, 0x00, //40 @
    0x30, 0x78, 0xCC, 0xCC, 0xFC, 0xCC, 0xCC, 0x00, //41 A
    0xFC, 0x66, 0x66, 0x7C, 0x66, 0x66, 0xFC, 0x00, //42 B
    0x3C, 0x66, 0xC0, 0xC0, 0xC0, 0x66, 0x3C, 0x00, //43 C
    0xF8, 0x6C, 0x66, 0x66, 0x66, 0x6C, 0xF8, 0x00, //44 D
    0xFE, 0x62, 0x68, 0x78, 0x68, 0x62, 0xFE, 0x00, //45 E
    0xFE, 0x62, 0x68, 0x78, 0x68, 0x60, 0xF0, 0x00, //46 F
    0x3C, 0x66, 0xC0, 0xC0, 0xCE, 0x66, 0x3E, 0x00, //47 G
    0xCC, 0xCC, 0xCC, 0xFC, 0xCC, 0xCC, 0xCC, 0x00, //48 H
    0x78, 0x30, 0x30, 0x30, 0x30, 0x30, 0x78, 0x00, //49 I
    0x1E, 0x0C, 0x0C, 0x0C, 0xCC, 0xCC, 0x78, 0x00, //4a J
    0xE6, 0x66, 0x6C, 0x78, 0x6C, 0x66, 0xE6, 0x00, //4b K
    0xF0, 0x60, 0x60, 0x60, 0x62, 0x66, 0xFE, 0x00, //4c L
    0xC6, 0xEE, 0xFE, 0xFE, 0xD6, 0xC6, 0xC6, 0x00, //4d M
    0xC6, 0xE6, 0xF6, 0xDE, 0xCE, 0xC6, 0xC6, 0x00, //4e N
    0x38, 0x6C, 0xC6, 0xC6, 0xC6, 0x6C, 0x38, 0x00, //4f O
    0xFC, 0x66, 0x66, 0x7C, 0x60, 0x60, 0xF0, 0x00, //50 P
    0x78, 0xCC, 0xCC, 0xCC, 0xDC, 0x78, 0x1C, 0x00, //51 Q
    0xFC, 0x66, 0x66, 0x7C, 0x6C, 0x66, 0xE6, 0x00, //52 R
    0x78, 0xCC, 0xE0, 0x70, 0x1C, 0xCC, 0x78, 0x00, //53 S
    0xFC, 0xB4, 0x30, 0x30, 0x30, 0x30, 0x78, 0x00, //54 T
    0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xFC, 0x00, //55 U
    0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0x78, 0x30, 0x00, //56 V
    0xC6, 0xC6, 0xC6, 0xD6, 0xFE, 0xEE, 0xC6, 0x00, //57 W
    0xC6, 0xC6, 0x6C, 0x38, 0x38, 0x6C, 0xC6, 0x00, //58 X
    0xCC, 0xCC, 0xCC, 0x78, 0x30, 0x30, 0x78, 0x00, //59 Y
    0xFE, 0xC6, 0x8C, 0x18, 0x32, 0x66, 0xFE, 0x00, //5a Z
    0x78, 0x60, 0x60, 0x60, 0x60, 0x60, 0x78, 0x00, //5b [
    0xC0, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x02, 0x00, //5c backslash
    0x78, 0x18, 0x18, 0x18, 0x18, 0x18, 0x78, 0x00, //5d ]
    0x10, 0x38, 0x6C, 0xC6, 0x00, 0x00, 0x00, 0x00, //5e ^
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, //5f _
    0x30, 0x30, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, //60 `
    0x00, 0x00, 0x78, 0x0C, 0x7C, 0xCC, 0x76, 0x00, //61 a
    0xE0, 0x60, 0x60, 0x7C, 0x66, 0x66, 0xDC, 0x00, //62 b
    0x00, 0x00, 0x78, 0xCC, 0xC0, 0xCC, 0x78, 0x00, //63 c
    0x1C, 0x0C, 0x0C, 0x7C, 0xCC, 0xCC, 0x76, 0x00, //64 d
    0x00, 0x00, 0x78, 0xCC, 0xFC, 0xC0, 0x78, 0x00, //65 e
    0x38, 0x6C, 0x60, 0xF0, 0x60, 0x60, 0xF0, 0x00, //66 f
    0x00, 0x00, 0x76, 0xCC, 0xCC, 0x7C, 0x0C, 0xF8, //67 g
    0xE0, 0x60, 0x6C, 0x76, 0x66, 0x66, 0xE6, 0x00, //68 h
    0x30, 0x00, 0x70, 0x30, 0x30, 0x30, 0x78, 0x00, //69 i
    0x0C, 0x00, 0x0C, 0x0C, 0x0C, 0xCC, 0xCC, 0x78, //6a j
    0xE0, 0x60, 0x66, 0x6C, 0x78, 0x6C, 0xE6, 0x00, //6b k
    0x70, 0x30, 0x30, 0x30, 0x30, 0x30, 0x78, 0x00, //6c l
    0x00, 0x00, 0xCC, 0xFE, 0xFE, 0xD6, 0xC6, 0x00, //6d m
    0x00, 0x00, 0xF8, 0xCC, 0xCC, 0xCC, 0xCC, 0x00, //6e n
    0x00, 0x00, 0x78, 0xCC, 0xCC, 0xCC, 0x78, 0x00, //6f o
    0x00, 0x00, 0xDC, 0x66, 0x66, 0x7C, 0x60, 0xF0, //70 p
    0x00, 0x00, 0x76, 0xCC, 0xCC, 0x7C, 0x0C, 0x1E, //71 q
    0x00, 0x00, 0xDC, 0x76, 0x66, 0x60, 0xF0, 0x00, //72 r
    0x00, 0x00, 0x7C, 0xC0, 0x78, 0x0C, 0xF8, 0x00, //73 s
    0x10, 0x30, 0x7C, 0x30, 0x30, 0x34, 0x18, 0x00, //74 t
    0x00, 0x00, 0xCC, 0xCC, 0xCC, 0xCC, 0x76, 0x00, //75 u
    0x00, 0x00, 0xCC, 0xCC, 0xCC, 0x78, 0x30, 0x00, //76 v
    0x00, 0x00, 0xC6, 0xD6, 0xFE, 0xFE, 0x6C, 0x00, //77 w
    0x00, 0x00, 0xC6, 0x6C, 0x38, 0x6C, 0xC6, 0x00, //78 x
    0x00, 0x00, 0xCC, 0xCC, 0xCC, 0x7C, 0x0C, 0xF8, //79 y
    0x00, 0x00, 0xFC, 0x98, 0x30, 0x64, 0xFC, 0x00, //7a z
    0x1C, 0x30, 0x30, 0xE0, 0x30, 0x30, 0x1C, 0x00, //7b {
    0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x00, //7c |
    0xE0, 0x30, 0x30, 0x1C, 0x30, 0x30, 0xE0, 0x00, //7d }
    0x76, 0xDC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //7e ~
    0x00, 0x10, 0x38, 0x6C, 0xC6, 0xC6, 0xFE, 0x00, //7f pyramid
};

//
// Function prototypes.
//
void TimerIsr(void);

/**
 *  This class defines and implements the LEDMatrix class for an 8x8 LED matrix
 *  array. The class is designed to be instantiated multiple times one for
 *  each LED matrix module. The modules can be chained horizontally and/or
 *  vertically allowing the message to be scrolled across neighboring
 *  modules with relative ease.
 */
class CLEDMatrix
{
private:
    const int          *m_rowPins;
    const int          *m_redColPins;
    const int          *m_greenColPins;
    const int          *m_blueColPins;
    const unsigned char*m_font8x8;
    unsigned char       m_redBuff[NUM_ROWS];
    unsigned char       m_greenBuff[NUM_ROWS];
    unsigned char       m_blueBuff[NUM_ROWS];
    int                 m_pixelRow;
    int                 m_pixelCol;
    int                 m_orientation;
    char               *m_message;
    int                 m_msgLen;
    int                 m_msgIdx;
    int                 m_scrollRow;
    int                 m_scrollCol;
    int                 m_prevScrollDir;
    boolean             m_scrollStarted;

    /**
     *  This function copies the font pixel map of the given character into
     *  the pixel buffer.
     *
     *  @param ch Specifies the character to put into the pixel buffer.
     *  @param color Specifies the color of the character.
     *  @param font8x8 Points to the font table.
     */
    void
    CopyCharToBuffer(
        char ch,
        int color,
        const unsigned char *font8x8
        )
    {
        for (int row = 0; row < NUM_ROWS; row++)
        {
            m_redBuff[row] = ((m_redColPins != NULL) &&
                              (color & COLOR_RED))?
                                font8x8[ch*8 + row]: 0x00;
            m_greenBuff[row] = ((m_greenColPins != NULL) &&
                                (color & COLOR_GREEN))?
                                font8x8[ch*8 + row]: 0x00;
            m_blueBuff[row] = ((m_blueColPins != NULL) &&
                               (color & COLOR_BLUE))?
                                font8x8[ch*8 + row]: 0x00;
        }
    }   //CopyCharToBuffer

public:
    /**
     *  This function is called by the ISR to turn on/off the next pixel.
     */
    void
    DrawNextPixel(
        void
        )
    {
        //
        // Turn off the current pixel.
        //
        if (m_rowPins != NULL)
        {
            pinMode(m_rowPins[m_pixelRow], INPUT);
        }
        if (m_redColPins != NULL)
        {
            pinMode(m_redColPins[m_pixelCol], INPUT);
        }
        if (m_greenColPins != NULL)
        {
            pinMode(m_greenColPins[m_pixelCol], INPUT);
        }
        if (m_blueColPins != NULL)
        {
            pinMode(m_blueColPins[m_pixelCol], INPUT);
        }
        //
        // Move to the next pixel.
        //
        m_pixelCol++;
        if (m_pixelCol >= NUM_COLS)
        {
            m_pixelCol = 0;
            m_pixelRow++;
            if (m_pixelRow >= NUM_ROWS)
            {
                m_pixelRow = 0;
            }
        }
        //
        // Calculate the buffer row and column according to orientation.
        //
        int buffRow, buffCol;
        switch (m_orientation)
        {
            case ORIENTATION_UP_TOP:
                buffRow = m_pixelRow;
                buffCol = m_pixelCol;
                break;

            case ORIENTATION_RIGHT_TOP:
                buffRow = NUM_COLS - m_pixelCol - 1;
                buffCol = m_pixelRow;
                break;

            case ORIENTATION_DOWN_TOP:
                buffRow = NUM_ROWS - m_pixelRow - 1;
                buffCol = NUM_COLS - m_pixelCol - 1;
                break;

            case ORIENTATION_LEFT_TOP:
                buffRow = m_pixelCol;
                buffCol = NUM_ROWS - m_pixelRow - 1;
                break;
        }

#ifdef _MIRRORED_
        boolean redPixelOn = (m_redBuff[buffRow] & (0x01 << buffCol)) != 0;
        boolean greenPixelOn = (m_greenBuff[buffRow] & (0x01 << buffCol)) != 0;
        boolean bluePixelOn = (m_blueBuff[buffRow] & (0x01 << buffCol)) != 0;
#else
        boolean redPixelOn = (m_redBuff[buffRow] & (0x80 >> buffCol)) != 0;
        boolean greenPixelOn = (m_greenBuff[buffRow] & (0x80 >> buffCol)) != 0;
        boolean bluePixelOn = (m_blueBuff[buffRow] & (0x80 >> buffCol)) != 0;
#endif

        if (m_rowPins != NULL)
        {
            digitalWrite(m_rowPins[m_pixelRow], HIGH);
            pinMode(m_rowPins[m_pixelRow], OUTPUT);
        }
        if (m_redColPins != NULL)
        {
            digitalWrite(m_redColPins[m_pixelCol], redPixelOn? LOW: HIGH);
            pinMode(m_redColPins[m_pixelCol], OUTPUT);
        }
        if (m_greenColPins != NULL)
        {
            digitalWrite(m_greenColPins[m_pixelCol], greenPixelOn? LOW: HIGH);
            pinMode(m_greenColPins[m_pixelCol], OUTPUT);
        }
        if (m_blueColPins != NULL)
        {
            digitalWrite(m_blueColPins[m_pixelCol], bluePixelOn? LOW: HIGH);
            pinMode(m_blueColPins[m_pixelCol], OUTPUT);
        }
    }   //DrawNextPixel

    /**
     *  This function clears the LED matrix.
     */
    void
    Erase(
        void
        )
    {
        CopyCharToBuffer('\0', COLOR_BLACK, s_font8x8);
    }   //Erase

    /**
     *  This function sets the color of the specified pixel.
     *
     *  @param row Specifies the row number of the pixel.
     *  @param col Specifies the column number of the pixel.
     *  @param color Specifies the color of the pixel.
     */
    void
    SetPixelColor(
        int row,
        int col,
        int color
        )
    {
        unsigned char colMask = (0x01 << (NUM_COLS - col - 1));
        //
        // Clear the pixel.
        //
        m_redBuff[row] &= ~colMask;
        m_greenBuff[row] &= ~colMask;
        m_blueBuff[row] &= ~colMask;
        //
        // Set the corresponding color LED on or off.
        //
        m_redBuff[row] |= ((m_redColPins != NULL) &&
                           (color & COLOR_RED))? colMask: 0;
        m_greenBuff[row] |= ((m_greenColPins != NULL) &&
                             (color & COLOR_GREEN))? colMask: 0;
        m_blueBuff[row] |= ((m_blueColPins != NULL) &&
                            (color & COLOR_BLUE))? colMask: 0;
    }   //SetPixel

    /**
     *  This function sets the message to be scrolled on the LED matrix.
     *
     *  @param msg Points to the message string to be scrolled onto the LED
     *         matrix.
     *  @param scrollDir Specifies the scroll direction.
     *  @param font8x8 Optionally points to the font table for the message.
     */
    void
    SetMessage(
        char *msg,
        int scrollDir,
        const unsigned char *font8x8 = s_font8x8
        )
    {
        m_font8x8 = font8x8;
        m_message = msg;
        m_msgLen = (msg == NULL)? 0: strlen(msg);
        m_msgIdx = ((scrollDir == SCROLL_RIGHT) || (scrollDir == SCROLL_DOWN))?
                        m_msgLen - 1: 0;
        m_scrollRow = m_scrollCol = 0;
        if (scrollDir == SCROLL_RIGHT)
        {
            m_scrollCol = NUM_COLS - 1;
        }
        else if (scrollDir == SCROLL_DOWN)
        {
            m_scrollRow = NUM_ROWS - 1;
        }
        m_prevScrollDir = SCROLL_NONE;
        m_scrollStarted = false;
    }   //SetMessage

    /**
     *  This function initializes the LEDMatrix object.
     *
     *  @param rowPins Points to the row pin table.
     *  @param redColPins Points to the red column pin table.
     *  @param greenColPins Points to the green column pin table.
     *  @param blueColPins Points to the blue column pin table.
     *
     *  @return Returns true if successful, false otherwise.
     */
    boolean
    Init(
        const int *rowPins,
        const int *redColPins,
        const int *greenColPins,
        const int *blueColPins
        )
    {
        boolean fSuccess = false;
        //
        // Must have the rowPins and at least one color colPins.
        //
        if ((rowPins != NULL) &&
            ((redColPins != NULL) ||
             (greenColPins != NULL) ||
             (blueColPins != NULL)))
        {
            m_rowPins = rowPins;
            m_redColPins = redColPins;
            m_greenColPins = greenColPins;
            m_blueColPins = blueColPins;
            m_font8x8 = s_font8x8;
            //
            // Turn off row drivers.
            //
            for (int row = 0; row < NUM_ROWS; row++)
            {
                pinMode(m_rowPins[row], INPUT);
            }
            //
            // Turn off column drivers.
            //
            for (int col = 0; col < NUM_COLS; col++)
            {
                if (m_redColPins != NULL)
                {
                    pinMode(m_redColPins[col], INPUT);
                }
                if (m_greenColPins != NULL)
                {
                    pinMode(m_greenColPins[col], INPUT);
                }
                if (m_blueColPins != NULL)
                {
                    pinMode(m_blueColPins[col], INPUT);
                }
            }
            m_pixelRow = m_pixelCol = 0;
            m_orientation = ORIENTATION_UP_TOP;
            //
            // Set the frames per second. One interrupt per pixel.
            //
            Timer1.initialize(1000000/(NUM_ROWS*NUM_COLS*FRAME_RATE));
            Timer1.attachInterrupt(TimerIsr);
    
            Erase();
            SetMessage(NULL, SCROLL_NONE);
            fSuccess = true;
        }

        return fSuccess;
    }   //Init

    /**
     *  This functions draws a given matrix buffer.
     *
     *  @param redBuff Specifies the buffer for red pixels.
     *  @param greenBuff Specifies the buffer for green pixels.
     *  @param blueBuff Specifies the buffer for blue pixels.
     */
    void
    DrawBuffer(
        unsigned char *redBuff,
        unsigned char *greenBuff,
        unsigned char *blueBuff
        )
    {
        for (int row = 0; row < NUM_ROWS; row++)
        {
            m_redBuff[row] = (redBuff != NULL)? redBuff[row]: 0x00;
            m_greenBuff[row] = (greenBuff != NULL)? greenBuff[row]: 0x00;
            m_blueBuff[row] = (blueBuff != NULL)? blueBuff[row]: 0x00;
        }
    }   //DrawBuffer

    /**
     *  This functions draws a character on the LED matrix.
     *
     *  @param ch Specifies the character to be drawn on the LED matrix.
     *  @param color Optionally specifies the color of the character.
     *  @param orientation Optionally specifies the orientation of the LED
     *         matrix module.
     *  @param font8x8 Optionally points to the font table for the character.
     */
    void
    DrawChar(
        char ch,
        int color = COLOR_GREEN,
        int orientation = ORIENTATION_UP_TOP,
        const unsigned char *font8x8 = s_font8x8
        )
    {
        m_orientation = orientation;
        CopyCharToBuffer(ch, color, font8x8);
    }   //DrawChar

    /**
     *  This function scrolls the pixels in the buffer one position with
     *  the given scroll direction.
     *
     *  @param scrollDir Specifies the scroll direction.
     *  @param scrollInBits Specifies the bits to be scrolled in.
     *  @param orientation Optionally specifies the orientation of the LED
     *         matrix module.
     *
     *  @return Returns the bits being scrolled out. This is useful for
     *          chaining several LED matrix modules together so that the
     *          scrolled out bits from one module will become the scrolled
     *          in bits for the adjacent module.
     */
    unsigned long
    Scroll(
        int scrollDir,
        unsigned long scrollInBits,
        int orientation = ORIENTATION_UP_TOP
        )
    {
        unsigned char scrollOutRedBits = 0;
        unsigned char scrollInRedBits = RED_VALUE(scrollInBits);
        unsigned char scrollOutGreenBits = 0;
        unsigned char scrollInGreenBits = GREEN_VALUE(scrollInBits);
        unsigned char scrollOutBlueBits = 0;
        unsigned char scrollInBlueBits = BLUE_VALUE(scrollInBits);
        int row;

        m_orientation = orientation;
        switch (scrollDir)
        {
            case SCROLL_LEFT:
                for (row = 0; row < NUM_ROWS; row++)
                {
                    if (m_redColPins != NULL)
                    {
                        scrollOutRedBits |= ((m_redBuff[row] & 0x80) >>
                                             row);
                        m_redBuff[row] <<= 1;
                        m_redBuff[row] |= (scrollInRedBits >>
                                           (NUM_ROWS - row - 1)) & 0x01;
                    }
                    if (m_greenColPins != NULL)
                    {
                        scrollOutGreenBits |= ((m_greenBuff[row] & 0x80) >>
                                               row);
                        m_greenBuff[row] <<= 1;
                        m_greenBuff[row] |= (scrollInGreenBits >>
                                             (NUM_ROWS - row - 1)) & 0x01;
                    }
                    if (m_blueColPins != NULL)
                    {
                        scrollOutBlueBits |= ((m_blueBuff[row] & 0x80) >>
                                              row);
                        m_blueBuff[row] <<= 1;
                        m_blueBuff[row] |= (scrollInBlueBits >>
                                            (NUM_ROWS - row - 1)) & 0x01;
                    }
                }
                break;

            case SCROLL_RIGHT:
                for (row = 0; row < NUM_ROWS; row++)
                {
                    if (m_redColPins != NULL)
                    {
                        scrollOutRedBits |= ((m_redBuff[row] & 0x01) <<
                                             (NUM_ROWS - row - 1));
                        m_redBuff[row] >>= 1;
                        m_redBuff[row] |= ((scrollInRedBits >>
                                            (NUM_ROWS - row - 1)) <<
                                           (NUM_COLS - 1));
                    }
                    if (m_greenColPins != NULL)
                    {
                        scrollOutGreenBits |= ((m_greenBuff[row] & 0x01) <<
                                               (NUM_ROWS - row - 1));
                        m_greenBuff[row] >>= 1;
                        m_greenBuff[row] |= ((scrollInGreenBits >>
                                              (NUM_ROWS - row - 1)) <<
                                             (NUM_COLS - 1));
                    }
                    if (m_blueColPins != NULL)
                    {
                        scrollOutBlueBits |= ((m_blueBuff[row] & 0x01) <<
                                              (NUM_ROWS - row - 1));
                        m_blueBuff[row] >>= 1;
                        m_blueBuff[row] |= ((scrollInBlueBits >>
                                             (NUM_ROWS - row - 1)) <<
                                            (NUM_COLS - 1));
                    }
                }
                break;

            case SCROLL_UP:
                scrollOutRedBits = (m_redColPins != NULL)?
                                        m_redBuff[0]: 0x00;
                scrollOutGreenBits = (m_greenColPins != NULL)?
                                        m_greenBuff[0]: 0x00;
                scrollOutBlueBits = (m_blueColPins != NULL)?
                                        m_blueBuff[0]: 0x00;
                for (row = 0; row < NUM_ROWS - 1; row++)
                {
                    if (m_redColPins != NULL)
                    {
                        m_redBuff[row] = m_redBuff[row + 1];
                    }
                    if (m_greenColPins != NULL)
                    {
                        m_greenBuff[row] = m_greenBuff[row + 1];
                    }
                    if (m_blueColPins != NULL)
                    {
                        m_blueBuff[row] = m_blueBuff[row + 1];
                    }
                }
                if (m_redColPins != NULL)
                {
                    m_redBuff[NUM_ROWS - 1] = scrollInRedBits;
                }
                if (m_greenColPins != NULL)
                {
                    m_greenBuff[NUM_ROWS - 1] = scrollInGreenBits;
                }
                if (m_redColPins != NULL)
                {
                    m_blueBuff[NUM_ROWS - 1] = scrollInBlueBits;
                }
                break;

            case SCROLL_DOWN:
                scrollOutRedBits = (m_redColPins != NULL)?
                                        m_redBuff[NUM_ROWS - 1]: 0x00;
                scrollOutGreenBits = (m_greenColPins != NULL)?
                                        m_greenBuff[NUM_ROWS - 1]: 0x00;
                scrollOutBlueBits = (m_blueColPins != NULL)?
                                        m_blueBuff[NUM_ROWS - 1]: 0x00;
                for (row = NUM_ROWS - 1; row > 0; row--)
                {
                    if (m_redColPins != NULL)
                    {
                        m_redBuff[row] = m_redBuff[row - 1];
                    }
                    if (m_greenColPins != NULL)
                    {
                        m_greenBuff[row] = m_greenBuff[row - 1];
                    }
                    if (m_blueColPins != NULL)
                    {
                        m_blueBuff[row] = m_blueBuff[row - 1];
                    }
                }
                if (m_redColPins != NULL)
                {
                    m_redBuff[0] = scrollInRedBits;
                }
                if (m_greenColPins != NULL)
                {
                    m_greenBuff[0] = scrollInGreenBits;
                }
                if (m_blueColPins != NULL)
                {
                    m_blueBuff[0] = scrollInBlueBits;
                }
                break;
        }

        return RGB(scrollOutRedBits, scrollOutGreenBits, scrollOutBlueBits);
    }   //Scroll

    /**
     *  This function scrolls the message set the SetMessage by one pixel in
     *  the specified direction.
     *
     *  @param scrollDir Specifies the scroll direction.
     *  @param color Optionally specifies the color of the message pixel.
     *  @param orientation Optionally specifies the orientation of the LED
     *         matrix module.
     *
     *  @return Returns false when the scrolling is completed (reaches the
     *          end of the message.
     */
    #define IsHoriScroll(d)     (((d) == SCROLL_LEFT) || ((d) == SCROLL_RIGHT))
    #define IsVertScroll(d)     (((d) == SCROLL_UP) || ((d) == SCROLL_DOWN))
    boolean
    ScrollMsgPixel(
        int scrollDir,
        int color = COLOR_GREEN,
        int orientation = ORIENTATION_UP_TOP
        )
    {
        m_orientation = orientation;
        if (scrollDir != SCROLL_NONE)
        {
            if (scrollDir != m_prevScrollDir)
            {
                if (((m_prevScrollDir == SCROLL_LEFT) &&
                     (m_scrollCol != NUM_COLS - 1)) ||
                    ((m_prevScrollDir == SCROLL_RIGHT) &&
                     (m_scrollCol != 0)) ||
                    ((m_prevScrollDir == SCROLL_UP) &&
                     (m_scrollRow != NUM_ROWS - 1)) ||
                    ((m_prevScrollDir == SCROLL_DOWN) &&
                     (m_scrollRow != 0)))
                {
                    //
                    // Change scroll direction only at character boundary.
                    //
                    scrollDir = m_prevScrollDir;
                }
                else if (m_prevScrollDir != SCROLL_NONE)
                {
                    m_scrollRow = m_scrollCol = 0;
                    if (scrollDir == SCROLL_LEFT)
                    {
                        m_scrollCol = NUM_COLS - 1;
                    }
                    else if (scrollDir == SCROLL_UP)
                    {
                        m_scrollRow = NUM_ROWS - 1;
                    }
                }
                m_prevScrollDir = scrollDir;
            }

            if (!m_scrollStarted)
            {
                m_scrollStarted = true;
            }
            else
            {
                switch (scrollDir)
                {
                    case SCROLL_LEFT:
                        m_scrollCol++;
                        if (m_scrollCol >= NUM_COLS)
                        {
                            m_scrollCol = 0;
                            m_msgIdx++;
                            if (m_msgIdx >= m_msgLen)
                            {
                                m_scrollStarted = false;
                                m_msgIdx = 0;
                            }
                        }
                        break;

                    case SCROLL_RIGHT:
                        m_scrollCol--;
                        if (m_scrollCol < 0)
                        {
                            m_scrollCol = NUM_COLS - 1;
                            m_msgIdx--;
                            if (m_msgIdx < 0)
                            {
                                m_scrollStarted = false;
                                m_msgIdx = m_msgLen - 1;
                            }
                        }
                        break;

                    case SCROLL_UP:
                        m_scrollRow++;
                        if (m_scrollRow >= NUM_ROWS)
                        {
                            m_scrollRow = 0;
                            m_msgIdx++;
                            if (m_msgIdx >= m_msgLen)
                            {
                                m_scrollStarted = false;
                                m_msgIdx = 0;
                            }
                        }
                        break;

                    case SCROLL_DOWN:
                        m_scrollRow--;
                        if (m_scrollRow < 0)
                        {
                            m_scrollRow = NUM_ROWS - 1;
                            m_msgIdx--;
                            if (m_msgIdx < 0)
                            {
                                m_scrollStarted = false;
                                m_msgIdx = m_msgLen - 1;
                            }
                        }
                        break;
                }
            }

            char ch = m_message[m_msgIdx];
            unsigned char fontBits = 0;

            if (IsHoriScroll(scrollDir))
            {
                for (int row = 0; row < NUM_ROWS; row++)
                {
                    fontBits |= ((m_font8x8[ch*8 + row] >>
                                  (NUM_COLS - m_scrollCol - 1)) & 0x01) <<
                                (NUM_ROWS - row - 1);
                }
            }
            else
            {
                fontBits = m_font8x8[ch*8 + m_scrollRow];
            }

            unsigned long scrollBits = RGB(
                                ((m_redColPins != NULL) &&
                                 (color & COLOR_RED))?
                                    fontBits: 0,
                                ((m_greenColPins != NULL) &&
                                 (color & COLOR_GREEN))?
                                    fontBits: 0,
                                ((m_blueColPins != NULL) &&
                                 (color & COLOR_BLUE))?
                                    fontBits: 0);
            Scroll(scrollDir, scrollBits, orientation);
        }

        return m_scrollStarted;
    }   //ScrollMsgPixel

    /**
     *  This function scrolls a message accross the LED matrix in the given
     *  direction and with the given delay between each pixel movement.
     *
     *  @param msg Specifies the message to be scrolled across the LED matrix.
     *  @param scrollDir Specifies the scroll direction.
     *  @param color Optionally specifies the color of the message.
     *  @param pixelDelay Optionally specifies the delay in msec between each
     *         pixel scrolled.
     *  @param orientation Optionally specifies the orientation of the LED
     *         matrix module.
     *  @param Optionally points to the font table for the message.
     */
    void
    ScrollMessage(
        char *msg,
        int scrollDir,
        int color = COLOR_GREEN,
        int pixelDelay = 100,
        int orientation = ORIENTATION_UP_TOP,
        const unsigned char *font8x8 = s_font8x8
        )
    {
        m_font8x8 = font8x8;
        m_orientation = orientation;
        SetMessage(msg, scrollDir);

        while (ScrollMsgPixel(scrollDir, color, orientation))
        {
            delay(pixelDelay);
        }
    }   //ScrollMessage

};  //class CLEDMatrix

extern CLEDMatrix       g_LEDMatrix;

/**
 *  This is the timer interrupt service handler. It is called at a rate of
 *  60Hz*NumOfPixels. It calls a function to turn on/off the next pixel.
 */
void
TimerIsr(
    void
    )
{
    g_LEDMatrix.DrawNextPixel();
}   //TimerIsr

#endif  //ifndef _LEDMATRIX_H
