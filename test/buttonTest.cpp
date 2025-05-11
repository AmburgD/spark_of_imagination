#include <HardwareSerial.h>
#include "Debouncer.h" // Assuming the Debouncer class is in this file

#define BUTTON_PIN 2 // GPIO 21 pin connected to button

// Create an instance of the Debouncer class
Debouncer buttonDebouncer(50, false); // Debounce delay of 50 ms, assuming active-high configuration

void setup()
{
    Serial.begin(9600);         // Initialize Serial Monitor
    pinMode(BUTTON_PIN, INPUT); // Set BUTTON_PIN as input with internal pull-up resistor
}

void loop()
{
    int buttonState = digitalRead(BUTTON_PIN);                      // Read the raw state of the button
    uint8_t debouncedState = buttonDebouncer.debounce(buttonState); // Get the debounced state

    Serial.print("Raw Pin State: ");
    Serial.print(buttonState); // Print the raw state
    Serial.print(" | Debounced Pin State: ");
    Serial.println(debouncedState); // Print the debounced state

    delay(50); // Add a delay to make the output readable
}
