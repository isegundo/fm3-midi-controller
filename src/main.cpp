#include "Arduino.h"
#include "U8glib.h"
#include "ezButton.h"
#include "MIDI.h"

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);

MIDI_CREATE_DEFAULT_INSTANCE();

ezButton button1(2);
ezButton button2(3);

ezButton button3(4);

bool state1 = false;
bool state2 = false;
bool modeSet = false;

// the mode the controller will operate
const int MODE_SCENE = 0;
const int MODE_FX = 1;
int currentMode = MODE_SCENE;

String displayText = "Start";

void setupDisplay(void)
{
  if (u8g.getMode() == U8G_MODE_R3G3B2)
  {
    u8g.setColorIndex(255); // white
  }
  else if (u8g.getMode() == U8G_MODE_GRAY2BIT)
  {
    u8g.setColorIndex(3); // max intensity
  }
  else if (u8g.getMode() == U8G_MODE_BW)
  {
    u8g.setColorIndex(1); // pixel on
  }
  else if (u8g.getMode() == U8G_MODE_HICOLOR)
  {
    u8g.setHiColorByRGB(255, 255, 255);
  }
}

void draw(void)
{
  int str_len = displayText.length() + 1;
  char char_array[str_len];
  displayText.toCharArray(char_array, str_len);

  // Scene
  u8g.setFont(u8g_font_fub30);
  u8g.drawStr(10, 57, char_array);

  // Outline
  u8g.drawRFrame(0, 18, 128, 46, 4);
}

void setButtonsDebounceTime(unsigned long timeInMilliseconds)
{
  button1.setDebounceTime(timeInMilliseconds);
  button2.setDebounceTime(timeInMilliseconds);
  button3.setDebounceTime(timeInMilliseconds);
}

void loopButtons()
{
  button1.loop();
  button2.loop();
  button3.loop();
}

void setup(void)
{
  Serial.begin(9600);
  setupDisplay();

  //  MIDI.begin(MIDI_CHANNEL_OMNI);  // Listen to all incoming messages

  setButtonsDebounceTime(50L);
}

void processInput()
{
  if (button1.isPressed())
  {
    Serial.println("The button 1 is pressed");
    state1 = true;
  }

  if (button2.isPressed())
  {
    Serial.println("The button 2 is pressed");
    state2 = true;
  }

  if (button3.isPressed())
  {
    Serial.println("The button 3 is pressed");
  }

  if (state1 && state2 && !modeSet)
  {
    modeSet = true;

    if (currentMode == MODE_FX)
    {
      currentMode = MODE_SCENE;
      displayText = "Scn";
    }
    else if (currentMode == MODE_SCENE)
    {
      currentMode = MODE_FX;
      displayText = "FX";
    }

    // Send note 42 with velocity 127 on channel 1
    //    MIDI.sendNoteOn(42, 127, 1);
  }

  if (button1.isReleased())
  {
    Serial.println("The button 1 is released");
    state1 = false;
    modeSet = false;
  }

  if (button2.isReleased())
  {
    Serial.println("The button 2 is released");
    state2 = false;
    modeSet = false;
  }

  if (button3.isReleased())
  {
    Serial.println("The button 3 is released");
  }
}

void loop(void)
{
  loopButtons();

  processInput();

  // Read incoming messages
  //  MIDI.read();

  // Draw
  u8g.firstPage();
  do
  {
    draw();
  } while (u8g.nextPage());

  delay(50);
}
