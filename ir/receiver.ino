#include <IRremote.h>

// pins to receive IR signal
int RECV_PIN = 15;

void setup() {
  Serial.begin(115200);
  IrReceiver.begin(RECV_PIN, ENABLE_LED_FEEDBACK); // Start the receiver
}

void loop() {
  if (IrReceiver.decode()) {
    Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX); // Print the received code
    IrReceiver.resume();
  }
}
