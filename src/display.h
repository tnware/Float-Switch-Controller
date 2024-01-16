#include <U8g2lib.h>
#include "globals.h"

extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2;

void drawStatusScreen(bool allClosed, int openSwitchCount)
{
    // Clear the buffer
    u8g2.clearBuffer();

    // Draw the top status bar
    for (int i = 0; i < numSwitches; ++i)
    {
        if (lastSwitchStates[i])
        {
            // Draw a filled rectangle for closed switch
            u8g2.drawBox(i * 25 + 1, 0, 24, 8);
        }
        else
        {
            // Draw an unfilled rectangle for open switch
            u8g2.drawFrame(i * 25 + 1, 0, 24, 8);
        }
    }

    // Set the base line y-coordinate for the status text
    int baseY = 24;

    // Choose a suitable font for status message
    u8g2.setFont(u8g2_font_ncenB08_tr); // Choose a suitable font

    // Display the system status as text
    if (allClosed)
    {
        u8g2.drawStr(0, baseY, "OKAY");
    }
    else
    {
        char alertMsg[30];
        sprintf(alertMsg, "ALERT: %d OPEN", openSwitchCount);
        u8g2.drawStr(0, baseY, alertMsg);
    }

    // Choose a suitable font for the icons
    u8g2.setFont(u8g2_font_unifont_t_77);

    // Position for the icon in the bottom right corner
    int iconX = u8g2.getDisplayWidth() - 18; // 18 pixels from the right edge
    int iconY = u8g2.getDisplayHeight() - 2; // 2 pixels from the bottom edge

    // Draw the appropriate icon
    if (allClosed)
    {
        // Draw water droplet icon
        u8g2.drawGlyph(iconX, iconY, 0x26c6);
    }
    else
    {
        // Draw alert icon
        u8g2.drawGlyph(iconX, iconY, 0x26a0);
    }

    // Send buffer to the display
    u8g2.sendBuffer();
}