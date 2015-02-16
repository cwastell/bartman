// Load all of our libraries
#include "math.h"
// Set the variables globally because #YOLO
int potPin = A0;
// led 1 to indicate power is on
int onLed = D0;
// led 2 to indicate web override
int webLed = D1;
// led 3 for future use
int netLed = D2;
int lightGreen = D3;
int lightYellow = D4;
int lightRed = D5;
int totalPanicLevels = 8;
int panicLevel = 0;
int panicLevelDial = 0;
int panicLevelDialOld = 0;
int panicLevelWeb = 0;
bool webOverride = false;
float potFactor = 4000;
int potLevel = 0;
float potRate = 0;
// rate in ms to blink the light when that's a thing
int blinkRate = 5000;
int blinkRatePanic = 2500;

// Get the level of the dial and return a panic level based on in the defined inputs
int readPotDial()
{
    potLevel = analogRead(potPin);    
    potRate = round((potLevel/potFactor) * totalPanicLevels);
    return potRate;
}

// web function for panic
void readWebPanic(String webLevelString)
{
    int webLevel;
    webLevel = webLevelString.toInt();
    // check the web input to verify that it's in bounds
    if(webLevel >= 0)
    {
        if(webLevel <= totalPanicLevels)
        {
            webOverride = true;
            panicLevelWeb = webLevel;
        }
    }
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
            blinkLight(lightYellow);
            offLight(lightYellow);
            break;
        case 5:
            offLight(lightGreen);
            onLight(lightYellow);
            offLight(lightRed);
            break;
        case 6:
            offLight(lightGreen);
            onLight(lightYellow);
            blinkLight(lightRed);
            break;
        case 7:
            offLight(lightGreen);
            offLight(lightYellow);
            onLight(lightRed);
            break;
        case 8:
            offLight(lightGreen);
            offLight(lightYellow);
            blinkLightPanic(lightRed);
            break;
        default:
            blinkLightPanic(lightGreen);
            blinkLightPanic(lightYellow);
            blinkLightPanic(lightRed);
    }
}

void blinkLight(int lightToBlink)
{
    digitalWrite(lightToBlink, HIGH);
    delay(blinkRate);
    digitalWrite(lightToBlink, LOW);
    delay(blinkRate);
}

void blinkLightPanic(int lightToBlink)
{
    digitalWrite(lightToBlink, HIGH);
    delay(blinkRatePanic);
    digitalWrite(lightToBlink, LOW);
    delay(blinkRatePanic);
}

void onLight(int lightToOn)
{
    digitalWrite(lightToOn, HIGH);
}

void offLight(int lightToOff)
{
    digitalWrite(lightToOff, LOW);
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
    Spark.function("setPanicLevel", readWebPanic);
    digitalWrite(onLed, HIGH);
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