#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>
#include <Debouncer.h>
#include <HTTPClient.h>
#include <fastled.h>

// Button GPIO inputs
// CONNECT TO VIN and 2
#define BUTTON_PIN 2
#define NUM_LEDS 8
#define DATA_PIN 4

CRGB leds[NUM_LEDS];

// Create an instance of the Debouncer class
Debouncer buttonDebouncer(50, false); // Debounce delay of 50 ms, Use Active High
int buttonState;

// set up a code for HTTP client
const char *ssid = "ONUGuest";
const char *password = "";
const char *serverURL = "http://PI.IP.HERE.:):5000/play";
const char *serverStopURL = "http://PI.IP.HERE.:):5000/stop";
HTTPClient http;

int buttonCountOne = 0;
int buttonCountTwo = 0;
int buttonCountThree = 0;
int buttonCountFour = 0;
// Add new effect variable here

// Broadcast Address
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message
{
  float countOne;
  float countTwo;
  float countThree;
  float countFour;
  // Add new effect variable here
} struct_message;

// send and recieve data variables
float incomingDat;
float incomingDat2;
float incomingDat3;
float incomingDat4;
// Add new effect variable here
String success;
struct_message outgoingMessages;
struct_message incomingReadings;
esp_now_peer_info_t peerInfo;
bool espNowReady = false;

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
  // Add new effect variable here
}
void initializeESPNOW()
{
  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK)
  {

    espNowReady = false;
    return;
  }
  else
  {
    espNowReady = true;

    leds[0] = CRGB::Blue;
    FastLED.show();
    buttonCountOne++;
  }

  // Register callbacks
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    espNowReady = false;
  }
}

void setup()
{
  // Init Serial Monitor
  Serial.begin(9600);

  pinMode(BUTTON_PIN, INPUT_PULLDOWN);

  // Initialize LED
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS); // GRB ordering is typical

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
}

void loop()
{

  // read the state of the switch into a local variable:
  buttonState = digitalRead(BUTTON_PIN);
  bool justPressed = buttonDebouncer.debounceAndPressed(buttonState); // transition detection

  if (justPressed)
  {
    // Update the button counts
    if (buttonCountOne < 2)
    {
      buttonCountOne++;

      if (buttonCountOne == 1)
      {
        leds[0] = CRGB::White;
        FastLED.show();
        // Connect to Wi-Fi
        WiFi.begin(ssid, password);

        int retries = 0;
        while (WiFi.status() != WL_CONNECTED && retries < 20)
        {
          delay(250);
          retries++;
          Serial.println(retries);
        }

        if (WiFi.status() == WL_CONNECTED)
        {
          Serial.println("Connected");
          http.begin(serverURL);
          int responseCode = http.POST("");

          if (responseCode > 0)
          {
            String response = http.getString();
            leds[0] = CRGB::Green;
            FastLED.show();
          }
          else
          {
            Serial.println("not Connected");
            leds[0] = CRGB::Red;
            FastLED.show();
          }
        }
      }
      if (buttonCountOne == 2)
      {
        if (WiFi.status() == WL_CONNECTED)
        {
          http.begin(serverStopURL);
          int responseCode = http.POST("");

          if (responseCode > 0)
          {
            String response = http.getString();
            leds[0] = CRGB::Green;
            FastLED.show();
          }
          else
          {
            leds[0] = CRGB::Red;
            FastLED.show();
          }
        }

        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF); // Kill Wi-Fi
        delay(1000);         // Let the radio settle
        // Re-enable Station mode (needed for ESP-NOW to work)
        WiFi.mode(WIFI_STA);

        initializeESPNOW();
      }
    }

    else if (buttonCountTwo < 2)
    {
      buttonCountTwo++;
      leds[0] = CRGB::Black;
      leds[1] = CRGB::White;
      FastLED.show();
      // if the effect has been turned off, make the LED orange
      if (buttonCountTwo == 2)
      {
        leds[1] = CRGB::OrangeRed;
        FastLED.show();
      }
    }
    else if (buttonCountThree < 2)
    {
      buttonCountThree++;
      leds[1] = CRGB::Black;
      leds[2] = CRGB::White;
      FastLED.show();
      // turn on the mirror
      if (buttonCountThree == 1)
      {
        buttonCountFour = 1;
      }
      if (buttonCountThree == 2)
      {
        buttonCountFour = 1;
        leds[2] = CRGB::OrangeRed;
        FastLED.show();
      }
    }
    else if (buttonCountFour < 2)
    {
      buttonCountFour++;
      leds[2] = CRGB::Black;
      leds[3] = CRGB::White;
      FastLED.show();
      if (buttonCountFour == 2)
      {
        leds[3] = CRGB::OrangeRed;
        FastLED.show();
      }
    }
    else
    {
      buttonCountOne = 0;
      buttonCountTwo = 0;
      buttonCountThree = 0;
      buttonCountFour = 0;
      // Add new effect variable here
      leds[3] = CRGB::Black;
      FastLED.show();
    }

    // Set values to send
    outgoingMessages.countOne = buttonCountOne;
    outgoingMessages.countTwo = buttonCountTwo;
    outgoingMessages.countThree = buttonCountThree;
    outgoingMessages.countFour = buttonCountFour;
    // Add new effect variable here
    //  Send message via ESP-NOW
    if (espNowReady)
    {
      esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&outgoingMessages, sizeof(outgoingMessages));
    }

    delay(300);
  }
}
