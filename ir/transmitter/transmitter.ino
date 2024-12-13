#include <IRremote.h>

#define IR_PIN 2 // Chân GPIO 2

void setup() {
  Serial.begin(115200);
  IrSender.begin(IR_PIN);
  Serial.println("Sending IR signal continuously...");
}

void loop() {
  IrSender.sendSony(0xa90, 12);
  delay(100);
}
