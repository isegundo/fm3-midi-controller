#include "Arduino.h"
#include "U8glib.h"
#include "ezButton.h"
#include "MIDI.h"

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);

MIDI_CREATE_DEFAULT_INSTANCE();

const int BUTTON_NUM = 4;

const int BUTTON_01_PIN = 2;
const int BUTTON_02_PIN = 3;
const int BUTTON_03_PIN = 4;
const int BUTTON_04_PIN = 5;
const int BUTTON_05_PIN = 6;
const int BUTTON_06_PIN = 7;
const int BUTTON_07_PIN = 8;
const int BUTTON_08_PIN = 9;
const int BUTTON_09_PIN = 10;
const int BUTTON_10_PIN = 11;


ezButton buttons[]= {
	ezButton(BUTTON_01_PIN), 
	ezButton(BUTTON_02_PIN), 
	ezButton(BUTTON_03_PIN), 
	ezButton(BUTTON_04_PIN), 
	// ezButton(BUTTON_05_PIN),
  // ezButton(BUTTON_06_PIN),
  // ezButton(BUTTON_07_PIN),
  // ezButton(BUTTON_08_PIN),
  // ezButton(BUTTON_09_PIN),
  // ezButton(BUTTON_10_PIN),
};

bool state[] = {false, false};
bool modeSet = false;

// the mode the controller will operate
const int MODE_SCENE = 0;
const int MODE_FX = 1;
int currentMode = MODE_SCENE;

String displayText = "Start";

void setupDisplay()
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

void draw()
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

void setupButtons(unsigned long timeInMilliseconds)
{
  for (int i = 0; i < BUTTON_NUM; i++)
  {
    buttons[i] = ezButton(i + 2);
    buttons[i].setDebounceTime(timeInMilliseconds);
  }
}

void loopButtons()
{
  for (int i = 0; i < BUTTON_NUM; i++)
  {
    buttons[i].loop();
  }
}

void updateDisplay()
{
  u8g.firstPage();
  do
  {
    draw();
  } while (u8g.nextPage());
}

void setup(void)
{
  Serial.begin(9600);
  setupButtons(50L);

  setupDisplay();
  updateDisplay();

  //  MIDI.begin(MIDI_CHANNEL_OMNI);  // Listen to all incoming messages
}

void processInput()
{
  bool someButtonPressed = false;
  loopButtons();

  for (int i = 0; i < BUTTON_NUM; i++)
  {

    if (buttons[i].isPressed())
    {
      someButtonPressed = true;

      // Buttons 0 and 1 (pin 2 and 3 respectively are special:
      // Preset up and down when in SCENE mode
      // Toggle an effect block when in FX mode)
      if (i == 0 || i == 1)
      {
        state[i] = true;
      }
      else
      {
        if (currentMode == MODE_SCENE)
        {
          displayText = "Scn " + String(i - 1);
        }
        else if (currentMode == MODE_FX)
        {
          displayText = "FX " + String(i - 1);
        }
      }
    }

    if (buttons[i].isReleased())
    {
      if (i == 0 || i == 1)
      {
        Serial.println("The button " + String(i + 1) + " is released");
        state[i] = false;
        modeSet = false;
      }
      else
      {
        Serial.println("The button " + String(i - 1) + " is released");
      }
    }
  }

  if (state[0] && state[1] && !modeSet)
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
    someButtonPressed = true;
    // Send note 42 with velocity 127 on channel 1
    //    MIDI.sendNoteOn(42, 127, 1);
  }

  if (someButtonPressed == true)
  {
    updateDisplay();
  }
}

void loop(void)
{

  processInput();

  // Read incoming messages
  //  MIDI.read();
}
