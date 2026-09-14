#include <Adafruit_GFX.h> // graphics library
#include <Adafruit_ST7789.h> // driver for the ST7789 screen
#include <SPI.h> 
#include <Arduino.h>
#include <WiFi.h>
#include <time.h>

// Defining pins for the display, change according to your setup!!! Uses the white numbers on the ESP
#define TFT_SCLK 0 // labeled SCL on the screen
#define TFT_MOSI 1 // labeled SDA on the screen
#define TFT_RST 2
#define TFT_DC 3
#define TFT_CS 4
#define TFT_BL 5

#define ROW1 6
#define ROW2 7

#define COL1 8
#define COL2 9
#define COL3 10

// Fix setColRowStart() by exposing it via a subclass
class MyST7789 : public Adafruit_ST7789 {
public:
  MyST7789(int8_t cs, int8_t dc, int8_t mosi, int8_t sclk, int8_t rst)
    : Adafruit_ST7789(cs, dc, mosi, sclk, rst) {}
  void setOffsets(uint8_t col, uint8_t row) {
    _colstart = _colstart2 = col;
    _rowstart = _rowstart2 = row;
  }
};

MyST7789 tft(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

const char *ntpServer = "pool.ntp.org";

// Bangladesh
const char *TZ_INFO = "Asia/Dhaka";

void connectToWiFi()
{
    WiFi.begin(SECRET_SSID, SECRET_PASSWORD);
    int retry = 0;
    while (WiFi.status() != WL_CONNECTED && retry < 100) // ~10s
    {
        delay(100);
        retry++;
    }
}

void syncNTPTime()
{
    configTzTime(TZ_INFO, ntpServer);

    struct tm timeinfo;
    int retry = 0;
    while (!getLocalTime(&timeinfo) && retry < 10)
    {   
        Serial.println("Waiting for NTP time sync...");
        delay(1000);
        retry++;
    }
}

// Returns time string  (e.g., "15:30:25")
String getTimeString() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return "Error: Time not set";
  }
  
  char buffer[32];
  strftime(buffer, sizeof(buffer), "%H:%M:%S", &timeinfo);
  return String(buffer);
}

// Returns date string  (e.g., "2026-01-01")
String getDateString() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return "Error: Date not set";
  }
  
  char buffer[32];
  strftime(buffer, sizeof(buffer), "%Y-%m-%d", &timeinfo);
  return String(buffer);
}

void drawClock()
{
  String timeString = getTimeString();
  String dateString = getDateString();

  // Clear screen
  tft.fillScreen(ST77XX_BLACK);

  tft.setTextColor(ST77XX_WHITE);

  tft.setTextSize(4);

  // Roughly center the time
  int16_t x = 5;
  int16_t y = 35;

  tft.setCursor(x, y);

  tft.print(timeString);
  
  //date

  tft.setTextSize(2);

  tft.setCursor(20, 90);

  tft.print(dateString);
}
void setupButtons()
{
  // Rows
  pinMode(ROW1, OUTPUT);
  pinMode(ROW2, OUTPUT);

  // Columns
  pinMode(COL1, INPUT_PULLUP);
  pinMode(COL2, INPUT_PULLUP);
  pinMode(COL3, INPUT_PULLUP);

  digitalWrite(ROW1, HIGH);
  digitalWrite(ROW2, HIGH);
}


// Returns:
// 0 = nothing
// 1-6 = button number
int readButton()
{
  int rows[] = {ROW1, ROW2};
  int cols[] = {COL1, COL2, COL3};

  for (int r = 0; r < 2; r++)
  {
    // Activate one row
    digitalWrite(rows[r], LOW);

    for (int c = 0; c < 3; c++)
    {
      if (digitalRead(cols[c]) == LOW)
      {
        // Turn row back off
        digitalWrite(rows[r], HIGH);

        return r * 3 + c + 1;
      }
    }

    // Deactivate row
    digitalWrite(rows[r], HIGH);
  }

  return 0;
}


// setup() runs ONCE when the board powers on
void setup() {
  Serial.begin(115200); // lets the board talk to your computer

  pinMode(TFT_BL, OUTPUT); // Set the backlight pin mode, or just wire it to 3.3V
  digitalWrite(TFT_BL, LOW); // Turns the backlight ON, for some reason this screen is active Low, so setting it to LOW is really HIGH

  tft.init(76, 284); // Our panel size (portrait)
  tft.setOffsets(82, 18); // Offsets for the weird resolution
  tft.invertDisplay(false); // Invert the colors (This display is flipped from normal)
  tft.setRotation(1); // Landscape, if it's upside down use 3!
  Serial.println("TFT Initialized!");

  tft.fillScreen(ST77XX_BLACK); // clear the screen
  
  setupButtons();
  connectToWiFi();
  syncNTPTime();
  drawClock();
  Serial.println("Clock initialized!");

}



void loop()
{
  static int lastSecond = -1;

  struct tm timeinfo;

  if (getLocalTime(&timeinfo))
  {
    if (timeinfo.tm_sec != lastSecond)
    {
      lastSecond = timeinfo.tm_sec;

      drawClock();
    }
  }


  int button = readButton();

  if (button != 0)
  {
    Serial.print("Button pressed: ");
    Serial.println(button);

    // Temporary test behavior
    if (button == 1)
    {
      Serial.println("Button 1");
    }

    if (button == 2)
    {
      Serial.println("Button 2");
    }

    if (button == 3)
    {
      Serial.println("Button 3");
    }

    if (button == 4)
    {
      Serial.println("Button 4");
    }

    if (button == 5)
    {
      Serial.println("Button 5");
    }

    if (button == 6)
    {
      Serial.println("Button 6");
    }

    // Basic debounce
    delay(200);
  }

  delay(10);
}