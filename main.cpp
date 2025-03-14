#include <Wire.h>
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <WebServer.h>
// libraries; allows hardware to work!!
#include <Adafruit_NeoPixel.h>

// Define GPIOs
#define YELLOW_LED_PIN 12
#define RGB_LED_PIN 18
#define BUZZER_PIN 23
#define LEFT_BUTTON_PIN 26
#define RIGHT_BUTTON_PIN 27
#define MOISTURE_SENSOR 5

// I2C Configuration
#define I2C_MASTER_SCL_IO 22
#define I2C_MASTER_SDA_IO 21
#define I2C_MASTER_FREQ_HZ 100000
#define NUM_LEDS 4

// LCD Configuration
#define LCD_ADDRESS 0x27
#define LCD_COLUMNS 16
#define LCD_ROWS 2
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);

// Wi-Fi Configuration
const char *ssid = "CyberPonies";
const char *password = "womenRule5";

WebServer server(80);
Adafruit_NeoPixel strip(NUM_LEDS, RGB_LED_PIN, NEO_GRB + NEO_KHZ800);
// State variables
String lcdContent = "";
bool yellowLedState = false;

// Function to initialize I2C
void i2c_master_init()
{
    Wire.begin(I2C_MASTER_SDA_IO, I2C_MASTER_SCL_IO);
    Wire.setClock(I2C_MASTER_FREQ_HZ);
    Serial.println("I2C initialized successfully");
}

// Function to initialize LCD
void lcd_init()
{
    lcd.init();
    lcd.backlight();
    lcd.setCursor(-2, 0);
    lcdContent = "Moisture Sensor: ";
    lcd.print(lcdContent);
    // origianlly 2000, changed to 50000
    // delay(50000);
    lcd.clear();
}

// Function to handle button press events
void handle_button_press()
{
    bool leftButtonPressed = digitalRead(LEFT_BUTTON_PIN) == LOW;
    bool rightButtonPressed = digitalRead(RIGHT_BUTTON_PIN) == LOW;

    if (leftButtonPressed || rightButtonPressed)
    {
        yellowLedState = true;
        digitalWrite(YELLOW_LED_PIN, HIGH); // Turn on the yellow LED
        lcd.clear();
        if (leftButtonPressed)
        {
            lcdContent = "Left Button";
            lcd.setCursor(0, 0);
            lcd.print(lcdContent);
            Serial.println("Left button pressed");
        }
        else if (rightButtonPressed)
        {
            lcdContent = "Right Button";
            lcd.setCursor(0, 0);
            lcd.print(lcdContent);
            Serial.println("Right button pressed");
        }
    }
    else
    {
        yellowLedState = false;
        digitalWrite(YELLOW_LED_PIN, LOW); // Turn off the yellow LED
        Serial.println("No button pressed");
    }
}

void i2c_scanner();

void i2c_scanner()
{
    Serial.println("Scanning I2C devices...");
    for (uint8_t addr = 1; addr < 127; addr++)
    {
        Wire.beginTransmission(addr);
        if (Wire.endTransmission() == 0)
        {
            Serial.print("Found I2C device at address: 0x");
            Serial.println(addr, HEX);
        }
    }
    Serial.println("I2C scan complete.");
}

// HTML handler function
// edit for on device website!!
void handleRoot()
{
    String html = "<html><body><h1>Smart Plant Monitor Information</h1>";
    html += "<p><strong>Check out our Github page!</strong> " + lcdContent + "</p>";
    html += "<a href=https://mnmzmaya.github.io/>Github Pages Link</a>";
    html += "<p><strong>RGB LED State:</strong> " + String(yellowLedState ? "On" : "Off") + "</p>";
    html += "</body></html>";
    server.send(200, "text/html", html);
}

void setup()
{
    // intialize and show color on the LED
    strip.begin();
    strip.show();
    // Initialize Serial for debugging
    Serial.begin(115200);

    // Initialize GPIOs for LEDs, Buzzer, and Buttons
    pinMode(YELLOW_LED_PIN, OUTPUT);
    pinMode(RGB_LED_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(LEFT_BUTTON_PIN, INPUT_PULLUP);
    pinMode(RIGHT_BUTTON_PIN, INPUT_PULLUP);

    // Ensure Yellow LED is off by default
    // pinMode(YELLOW_LED_PIN, OUTPUT);
    // digitalWrite(YELLOW_LED_PIN, LOW);

    // Turn on RGB LED
    digitalWrite(RGB_LED_PIN, HIGH);

    // Beep the buzzer twice
    for (int i = 0; i < 2; i++)
    {
        digitalWrite(BUZZER_PIN, HIGH);
        delay(200);
        digitalWrite(BUZZER_PIN, LOW);
        delay(200);
    }

    // Initialize I2C
    i2c_master_init();

    // Initialize LCD
    lcd_init();

    // Run I2C Scanner
    i2c_scanner();

    // Initialize Wi-Fi
    Serial.print("Setting up Wi-Fi...");
    WiFi.softAP(ssid, password);
    IPAddress IP = WiFi.softAPIP();
    Serial.print("Wi-Fi IP Address: ");
    Serial.println(IP);

    // Set up web server
    server.on("/", handleRoot);
    server.begin();
    Serial.println("Web server started");
}

void loop()
{
    // digitalWrite(YELLOW_LED_PIN, HIGH);
    // handle_button_press();
    // server.handleClient(); // Handle incoming HTTP requests
    // delay(50);             // Check button state every 50ms
    // // sksksksksksks
    // for (int i = 0; i < NUM_LEDS; i++)
    // {
    //     strip.setPixelColor(i, strip.Color(0, 0, 255));
    //     strip.show();
    //     delay(1000);
    // }
    strip.show();
    strip.setPixelColor(0, strip.Color(255, 0, 0));   // LED 1 - Red
    strip.setPixelColor(1, strip.Color(0, 255, 0));   // LED 2 - Green
    strip.setPixelColor(2, strip.Color(0, 0, 255));   // LED 3 - Blue
    strip.setPixelColor(3, strip.Color(255, 255, 0)); // LED 4 - Yellow

    strip.show(); // Update LEDs
    delay(10);
    // doesnt work yet
    int sensorValue = analogRead(MOISTURE_SENSOR);
    // delay(1000);
    lcdContent = "Moisture Level: " + sensorValue;
}
