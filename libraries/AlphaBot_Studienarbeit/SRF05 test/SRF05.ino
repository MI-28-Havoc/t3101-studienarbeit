// ---------------------------------------------------------------------------
// Example NewPing library sketch that does a ping about 20 times per second.
// ---------------------------------------------------------------------------

#include <NewPing.h>

#define TRIGGER_PIN_SRF05  12  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN_SRF05     11  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define TRIGGER_PIN_SR04  8  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN_SR04     9  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 500 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonarSRF05(TRIGGER_PIN_SRF05, ECHO_PIN_SRF05, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
NewPing sonarSR04(TRIGGER_PIN_SR04, ECHO_PIN_SR04, MAX_DISTANCE); // NewPing setup of pins and maximum distance.


void setup() {
  Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.
}

void loop() {
  delay(50);                     // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
  Serial.print("Ping SRF05: ");
  Serial.print(sonarSRF05.ping_cm()); // Send ping, get distance in cm and print result (0 = outside set distance range)
  Serial.print("cm");

  Serial.print(" | Ping SR04: ");
  Serial.print(sonarSR04.ping_cm()); // Send ping, get distance in cm and print result (0 = outside set distance range)
  Serial.println("cm");
}
