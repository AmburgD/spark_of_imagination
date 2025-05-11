#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>
#include <fastled.h>

// LED information
#define NUM_LEDS 1533
#define DATA_PIN 2

CRGB leds[NUM_LEDS];

// MAC Address of Remote
uint8_t broadcastAddress[] = {0xC8, 0xF0, 0x9E, 0xF2, 0xBA, 0x3C};

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message
{
  float countOne;
  float countTwo;
  float countThree;
  float countFour;
} struct_message;

// send and recieve data variables
float incomingDat;
float incomingDat2;
float incomingDat3;
float incomingDat4;
String success;
struct_message outgoingMessages;
struct_message incomingReadings;
esp_now_peer_info_t peerInfo;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status == 0)
  {
    success = "Delivery Success :)";
  }
  else
  {
    success = "Delivery Fail :(";
  }
}

// Callback when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  incomingDat = incomingReadings.countOne;
  incomingDat2 = incomingReadings.countTwo;
  incomingDat3 = incomingReadings.countThree;
  incomingDat4 = incomingReadings.countFour;
}

void fadeToBlack(int ledNo, byte fadeValue)
{
  leds[ledNo].fadeToBlackBy(fadeValue);
}

void setPixel(int Pixel, byte red, byte green, byte blue)
{
  leds[Pixel].r = red;
  leds[Pixel].g = green;
  leds[Pixel].b = blue;
}

void setAll(byte red, byte green, byte blue)
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    setPixel(i, red, green, blue);
  }
  FastLED.show();
}

void meteorRain(byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int stepSize)
{
  setAll(0, 0, 0); // Clear all LEDs at the start

  // Iterate through the LED strip, but only update every `stepSize` LEDs (i += stepSize)
  for (int i = NUM_LEDS - 1; i >= 0; i -= stepSize)
  { // Start from the end and move backwards with a variable step size

    // Fade brightness of all LEDs to create a trail
    for (int j = 0; j < NUM_LEDS; j++)
    {
      if ((!meteorRandomDecay) || (random(10) > 5))
      {
        fadeToBlack(j, meteorTrailDecay);
      }
    }

    // Draw the meteor effect
    for (int j = 0; j < meteorSize; j++)
    {
      if ((i - j < NUM_LEDS) && (i - j >= 0))
      {
        setPixel(i - j, red, green, blue); // Light up the meteor's LED
      }
    }

    FastLED.show(); // Update the LEDs
  }
}

void setup()
{
  // Init Serial Monitor
  Serial.begin(9600);

  // Initialize LED
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS); // GRB ordering is typica
  FastLED.setMaxRefreshRate(0);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    return;
  }
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
}

void loop()
{

  if ((int(incomingDat3) % 2) == 1)
  {
    meteorRain(0xff, 0xff, 0xff, 10, 150, true, 10);
  }
  FastLED.clear();
  FastLED.show();
}