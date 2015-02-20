// Load all of our libraries
#include "math.h"
// Set the variables globally because #YOLO
int potPin = A0;
// led 1 to indicate power is on (ard1)
int onLed = TX;
// led 2 to indicate web override (ard2)
int webLed = D2;
// led 3 for future use (ard 3)
int netLed = D0;
// Ard pin 5
int lightGreen = D1;
// Ard pin 6
int lightYellow = A7;
// Ard pin 7
int lightRed = D4;
int totalPanicLevels = 9;
int panicLevel = 0;
int panicLevelDial = 0;
int panicLevelDialOld = 0;
int panicLevelWeb = 0;
bool webOverride = false;
float potFactor = 4000;
int potLevel = 0;
float potRate = 0;
// rate in ms to blink the light when that's a thing
long blinkRate = 1500;
long blinkRatePanic = 500;

// Get the level of the dial and return a panic level based on in the defined inputs
int readPotDial()
{
    potLevel = analogRead(potPin);
    potRate = round((potLevel/potFactor) * totalPanicLevels);
    return potRate;
}

// web function for panic
int readWebPanic(String webLevelString)
{
    int webLevel;
    int validWebLevel = 0;
    webLevel = webLevelString.toInt();
    // check the web input to verify that it's in bounds
    if(webLevel >= 0)
    {
        if(webLevel <= totalPanicLevels)
        {
            webOverride = true;
            panicLevelWeb = webLevel;
            validWebLevel = 1;
        }
    }
    return validWebLevel;
}

//
void setLight()
{
    // switch statement
    switch (panicLevel)
    {
        // give two spaces to off and two to green normal
        case 0:
            offLight(lightGreen);
            offLight(lightYellow);
            offLight(lightRed);
            break;
        case 1:
            offLight(lightGreen);
            offLight(lightYellow);
            offLight(lightRed);
            break;
        case 2:
            onLight(lightGreen);
            offLight(lightYellow);
            offLight(lightRed);
            break;
        case 3:
            onLight(lightGreen);
            offLight(lightYellow);
            offLight(lightRed);
            break;
        case 4:
            onLight(lightGreen);
            blinkLight(lightYellow, blinkRate);
            offLight(lightRed);
            break;
        case 5:
            offLight(lightGreen);
            onLight(lightYellow);
            offLight(lightRed);
            break;
        case 6:
            offLight(lightGreen);
            onLight(lightYellow);
            blinkLight(lightRed, blinkRate);
            break;
        case 7:
            offLight(lightGreen);
            offLight(lightYellow);
            onLight(lightRed);
            break;
        case 8:
            offLight(lightGreen);
            offLight(lightYellow);
            blinkLight(lightRed, blinkRatePanic);
            break;
        default:
            blinkLight(lightGreen, blinkRatePanic);
            blinkLight(lightYellow, blinkRatePanic);
            blinkLight(lightRed, blinkRatePanic);
    }
}

void blinkLight(int lightToBlink, int lightBlinkRate)
{
    digitalWrite(lightToBlink, LOW);
    delay(lightBlinkRate);
    digitalWrite(lightToBlink, HIGH);
    delay(lightBlinkRate);
}


void onLight(int lightToOn)
{
    digitalWrite(lightToOn, LOW);
}

void offLight(int lightToOff)
{
    digitalWrite(lightToOff, HIGH);
}

void setup()
{
    pinMode(onLed, OUTPUT);
    pinMode(webLed, OUTPUT);
    pinMode(lightGreen, OUTPUT);
    pinMode(lightYellow, OUTPUT);
    pinMode(lightRed, OUTPUT);
    pinMode(potPin, INPUT );
    Spark.variable("panicLevel", &panicLevel, INT);
    Spark.variable("potLevel", &potLevel, INT);
    Spark.variable("panicLevelWeb", &panicLevelWeb, INT);
    Spark.function("setPanicLevel", &readWebPanic);
    digitalWrite(onLed, HIGH);
    
    // implement scott's startup idea
    onLight(lightGreen);
    delay(2000);
    onLight(lightYellow);
    delay(2000);
    onLight(lightRed);
    delay(5000);
    offLight(lightGreen);
    delay(2000);
    offLight(lightYellow);
    delay(2000);
    offLight(lightRed);
}

void loop()
{
    // check if we're net connected, if so raise the pin level
    if (Spark.connected())
    {
        digitalWrite(netLed, HIGH);
    }
    else
    {
        digitalWrite(netLed, LOW);
    }

    // copy over the panic level to see if it changed
    panicLevelDialOld = panicLevelDial;
    panicLevelDial = readPotDial();

    // if the panic level did change, override the webportion (when it's implmented obvs)
    if (panicLevelDialOld != panicLevelDial)
    {
        webOverride = false;
    }

    // if we're getting values from the web, use that input until the point where the dial changes.
    if (webOverride == true)
    {
        panicLevel = panicLevelWeb;
        digitalWrite(webLed, HIGH);
    }
    else
    {
        panicLevel = panicLevelDial;
        digitalWrite(webLed, LOW);
    }
    setLight();
}
