#include <Wire.h>
#include "rgb_lcd.h"
#include <Servo.h>

struct AnalogBranches
{
  const int pressureSensor = A0;
  const int joystickAxisX = A1;
  const int joystickAxisY = A2;
};
struct DigitalBranches
{
  const int servomotor = 4;
};

struct PreviousMillis
{
  unsigned long servo = 0;
  const unsigned int servoDelay = 100;
};

struct Symbols
{
  byte arrowUp[8] = {
      0b00100, //   ●
      0b01110, //  ●●●
      0b10101, // ● ● ●
      0b00100, //   ●
      0b00100, //   ●
      0b00100, //   ●
      0b00100, //   ●
      0b00100  //   ●
  };

  byte arrowDown[8] = {
      0b00100, //   ●
      0b00100, //   ●
      0b00100, //   ●
      0b00100, //   ●
      0b00100, //   ●
      0b10101, // ● ● ●
      0b01110, //  ●●●
      0b00100  //   ●
  };
  byte arrowLeft[8] = {
      0b00100,
      0b01000,
      0b11111,
      0b01000,
      0b00100,
      0b00000,
      0b00000,
      0b00000};

  byte arrowRight[8] = {
      0b00100,
      0b00010,
      0b11111,
      0b00010,
      0b00100,
      0b00000,
      0b00000,
      0b00000};

  byte centerDot[8] = {
      0b00000,
      0b00000,
      0b00000,
      0b00100,
      0b00100,
      0b00000,
      0b00000,
      0b00000};
  byte star[8] = {
      0b00000,
      0b00100,
      0b00100,
      0b11111,
      0b01110,
      0b00100,
      0b01010,
      0b10001};
};

struct Data
{
  int servoAlpha = 0;
  int tempoServoAlpha = -1;
  unsigned const int servoStep = 5;
  unsigned const int servoMaxAlpha = 180;
  bool blinkLED = false;
  int pressure = 0;
  int tempoPressure = -1;
  int objectDetected = false;
  unsigned const int maxRawPressureSensor = 650;
  unsigned const int maxForcePressureSensorOnObject = 15;
  unsigned const int serialBaud = 9600;
  struct maxRawJoyStickPosition
  {
    unsigned const int leftDown = 500;
    unsigned const int rightUp = 511;
  };
};

Servo servomotor;
rgb_lcd lcd;
AnalogBranches analogBranches;
DigitalBranches digitalBranches;
PreviousMillis prevMillis;
Data data;
Symbols symbols;

class JoyStickReader
{
public:
  JoyStickReader() {}

public:
  struct JoyStickPosition
  {
    bool left;
    bool right;
    bool up;
    bool down;
  };
  JoyStickPosition readPosition()
  {
    raw_joystick_axis_x = analogRead(joystickAxis_x_branch);
    raw_joystick_axis_y = analogRead(joystickAxis_y_branch);

    JoyStickPosition pos;
    pos.left = raw_joystick_axis_x < 500;
    pos.right = raw_joystick_axis_x > 511;

    pos.up = raw_joystick_axis_y > 511;
    pos.down = raw_joystick_axis_y < 500;

    return pos;
  }

  String getDirection()
  {
    JoyStickPosition pos = readPosition();

    if (pos.left && !pos.up && !pos.down)
    {
      return "left";
    }
    else if (pos.left && pos.up)
    {
      return "left-up";
    }
    else if (pos.left && pos.down)
    {
      return "left-down";
    }
    else if (pos.right && !pos.up && !pos.down)
    {
      return "right";
    }
    else if (pos.right && pos.up)
    {
      return "right-up";
    }
    else if (pos.right && pos.down)
    {
      return "right-down";
    }
    else if (pos.up)
    {
      return "up";
    }
    else if (pos.down)
    {
      return "down";
    }
    else
    {
      return "center";
    }
  }

private:
  const int joystickAxis_x_branch = analogBranches.joystickAxisX;
  const int joystickAxis_y_branch = analogBranches.joystickAxisY;
  int raw_joystick_axis_x;
  int raw_joystick_axis_y;
};

int pressureSensorReader()
{
  const int maxRaw = data.maxRawPressureSensor;
  const int rawRead = analogRead(analogBranches.pressureSensor);
  const int rawValue = rawRead > maxRaw ? maxRaw : rawRead;
  return map(rawValue, 0, maxRaw, 0, 20);
}

JoyStickReader joystick;

void setup()
{
  Serial.begin(data.serialBaud);
  servomotor.attach(digitalBranches.servomotor);
  data.servoAlpha = servomotor.read();
  lcd.begin(16, 2);
  lcd.setRGB(80, 200, 120);
  lcd.createChar(0, symbols.arrowUp);
  lcd.createChar(1, symbols.arrowDown);
  lcd.createChar(2, symbols.arrowRight);
  lcd.createChar(3, symbols.arrowLeft);
  lcd.createChar(4, symbols.centerDot);
  lcd.createChar(5, symbols.star);
  lcd.home();
}

void loop()
{
  JoyStickReader::JoyStickPosition position = joystick.readPosition();
  String direction = joystick.getDirection();
  data.pressure = pressureSensorReader();
  unsigned long currentMillis = millis();
  data.objectDetected = data.pressure >= data.maxForcePressureSensorOnObject;

  if (currentMillis - prevMillis.servo >= prevMillis.servoDelay)
  {
    prevMillis.servo = currentMillis;
    if (position.up && !data.objectDetected)
    {
      data.servoAlpha = data.servoAlpha < data.servoMaxAlpha ? data.servoAlpha + data.servoStep : data.servoMaxAlpha;
      servomotor.write(data.servoAlpha);
    }
    else if (position.down)
    {
      data.servoAlpha = data.servoAlpha > 0 ? data.servoAlpha - data.servoStep : 0;
      servomotor.write(data.servoAlpha);
    }
  }
  toggleBlinkScreen(position, data.objectDetected);
  if (data.tempoServoAlpha != data.servoAlpha)
  {
    lcd.home();
    if (position.up)
    {
      lcd.write(byte(0));
    }
    if (position.down)
    {
      lcd.write(byte(1));
    }

    lcd.print(" ");
    lcd.print(data.servoAlpha);
    lcd.print("degre ");
    data.tempoServoAlpha = data.servoAlpha;
  }

  if (direction == "center")
  {
    lcd.setCursor(0, 0);
    lcd.write(byte(4));
  }
  if (data.pressure != data.tempoPressure)
  {
    lcd.setCursor(2, 1);
    lcd.print(data.pressure);
    lcd.print("newton ");
    data.tempoPressure = data.pressure;

    if (data.objectDetected)
    {
      lcd.setCursor(0, 1);
      lcd.write(byte(5));
    }
    else
    {
      if (direction == "center")
      {
        lcd.setCursor(0, 1);
        lcd.print(" ");
      }
    }
  }
}

void toggleBlinkScreen(JoyStickReader::JoyStickPosition position, bool detected)
{
  if (detected)
  {
    lcd.setRGB(0, 255, 0);
    lcd.noBlinkLED();
    data.blinkLED = false;
  }
  else
  {
    if (!data.blinkLED && (position.up || position.down) && (data.servoAlpha != data.servoMaxAlpha && data.servoAlpha != 0))
    {
      lcd.blinkLED();
      data.blinkLED = true;
      lcd.setRGB(255, 100, 0);
    }
    if (data.blinkLED && !position.up && !position.down || (data.servoAlpha == data.servoMaxAlpha || data.servoAlpha == 0))
    {
      lcd.noBlinkLED();
      data.blinkLED = false;
      lcd.setRGB(80, 200, 120);
    }
    if (!position.up && !position.down)
    {
      lcd.setRGB(80, 200, 120);
    }
  }
}
