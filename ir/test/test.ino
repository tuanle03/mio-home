// #include <IRremote.h>

// // Pin nhận tín hiệu hồng ngoại
// int RECV_PIN = 14;
// // Pin phát tín hiệu hồng ngoại
// #define IR_SEND_PIN 2

// void setup() {
//   Serial.begin(115200);

//   // Khởi tạo bộ nhận tín hiệu hồng ngoại
//   IrReceiver.begin(RECV_PIN, ENABLE_LED_FEEDBACK);
//   Serial.println("IR Receiver initialized...");

//   // Khởi tạo bộ phát tín hiệu hồng ngoại
//   IrSender.begin(IR_SEND_PIN);
//   Serial.println("IR Sender initialized...");
// }

// void loop() {
//   // Nhận tín hiệu hồng ngoại
//   if (IrReceiver.decode()) {
//     Serial.print("Received IR signal: ");
//     Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
//     IrReceiver.resume(); // Tiếp tục nhận tín hiệu
//   }

//   // Phát tín hiệu hồng ngoại
//   Serial.println("Sending IR signal...");
//   IrSender.sendSony(0xa90, 12); // Phát tín hiệu theo mã Sony
//   delay(500); // Thời gian chờ để tránh xung đột
// }
//========================================================================================================================================================


// #include <IRremote.h>

// // Pin nhận tín hiệu hồng ngoại
// int RECV_PIN = 14;

// // Pin phát tín hiệu hồng ngoại
// #define IR_SEND_PIN 2

// // Mã tín hiệu từ remote
// unsigned long REMOTE_ON = 0xFC03EF00;
// unsigned long REMOTE_OFF = 0xFD02EF00;

// // Biến trạng thái
// bool ledState = false;

// // Thời gian
// unsigned long lastSendTime = 0;

// void setup() {
//   Serial.begin(115200);

//   // Khởi tạo bộ nhận
//   IrReceiver.begin(RECV_PIN, ENABLE_LED_FEEDBACK);
//   Serial.println("IR Receiver initialized...");

//   // Khởi tạo bộ phát
//   IrSender.begin(IR_SEND_PIN);
//   Serial.println("IR Sender initialized...");
// }

// void loop() {
//   unsigned long currentTime = millis();

//   // Phát tín hiệu mỗi giây
//   if (currentTime - lastSendTime >= 1000) {
//     lastSendTime = currentTime;

//     if (ledState) {
//       // Gửi tín hiệu bật (ON)
//       Serial.println("Sending ON signal...");
//       IrSender.sendNEC(REMOTE_ON, 32); // Giao thức NEC, 32-bit
//     } else {
//       // Gửi tín hiệu tắt (OFF)
//       Serial.println("Sending OFF signal...");
//       IrSender.sendNEC(REMOTE_OFF, 32); // Giao thức NEC, 32-bit
//     }

//     ledState = !ledState; // Đổi trạng thái
//   }

//   // Nhận tín hiệu từ remote
//   if (IrReceiver.decode()) {
//     unsigned long receivedCode = IrReceiver.decodedIRData.decodedRawData;

//     // Hiển thị thông tin tín hiệu
//     Serial.print("Received IR signal: ");
//     Serial.println(receivedCode, HEX);

//     // Kiểm tra giao thức
//     if (IrReceiver.decodedIRData.protocol == NEC) {
//       Serial.println("Protocol: NEC");
//     } else {
//       Serial.println("Protocol: Unknown");
//       Serial.println(IrReceiver.decodedIRData.protocol);
//     }

//     IrReceiver.resume(); // Tiếp tục nhận tín hiệu
//   }
// }
//========================================================================================================================================================


// #include <IRremote.h>

// // Pin nhận tín hiệu hồng ngoại
// int RECV_PIN = 14;

// void setup() {
//   Serial.begin(115200);

//   // Khởi tạo bộ nhận
//   IrReceiver.begin(RECV_PIN, ENABLE_LED_FEEDBACK);
//   Serial.println("IR Receiver initialized...");
// }

// void loop() {
//   // Kiểm tra xem có tín hiệu hồng ngoại nào được nhận hay không
//   if (IrReceiver.decode()) {
//     Serial.println("IR signal received!");

//     // In mã tín hiệu dạng HEX
//     unsigned long receivedCode = IrReceiver.decodedIRData.decodedRawData;
//     Serial.print("Decoded HEX Code: ");
//     Serial.println(receivedCode, HEX);

//     // Kiểm tra giao thức
//     if (IrReceiver.decodedIRData.protocol == NEC) {
//       Serial.println("Protocol: NEC");
//     } else {
//       Serial.println("Protocol: Unknown");
//     }

//     // In dữ liệu RAW
//     Serial.println("Raw Data:");
//     for (uint8_t i = 1; i < IrReceiver.decodedIRData.rawDataPtr->rawlen; i++) {
//       Serial.print(IrReceiver.decodedIRData.rawDataPtr->rawbuf[i], DEC);
//       Serial.print(" ");
//     }
//     Serial.println();
//     Serial.println("---------------------------");

//     // Tiếp tục nhận tín hiệu
//     IrReceiver.resume();
//   }
// }
//========================================================================================================================================================


// #include <IRremote.h>

// // Pin nhận và phát tín hiệu hồng ngoại
// #define IR_SEND_PIN 2
// #define IR_RECV_PIN 14

// // Tín hiệu RAW đã nhận (ON/OFF)
// uint16_t rawData_ON[] = {9000, 4500, 560, 560, 560, 1690, 560, 560, 560, 560, 560, 1690, 560, 560, 560, 560, 560, 560, 560, 560, 560, 1690, 560, 560, 560, 1690, 560, 1690, 560, 1690, 560, 1690, 560, 560, 560, 1690, 560, 1690, 560, 1690, 560, 1690, 560, 1690, 560, 1690, 560, 1690, 560, 1690, 560, 560, 560, 560, 560, 560, 560, 560, 560, 560, 560, 560, 560, 560, 560, 560, 560, 560, 560, 560, 560, 560};
// uint16_t rawData_OFF[] = {9000, 4500, 560, 560, 560, 1690, 560, 560, 560, 560, 560, 1690, 560, 560, 560, 560, 560, 560, 560, 560, 560, 1690, 560, 560, 560, 1690, 560, 1690, 560, 1690, 560, 1690, 560, 560, 560, 1690, 560, 1690, 560, 1690, 560, 1690, 560, 1690, 560, 1690, 560, 1690, 560, 1690, 560, 560, 560, 560, 560, 560, 560, 560, 560, 560, 560, 560, 560, 560, 560, 560, 560, 560, 560, 560, 560, 560};

// void setup() {
//   Serial.begin(115200);

//   // Khởi tạo bộ phát
//   IrSender.begin(IR_SEND_PIN);
//   Serial.println("IR Sender initialized...");

//   // Khởi tạo bộ nhận
//   IrReceiver.begin(IR_RECV_PIN, ENABLE_LED_FEEDBACK);
//   Serial.println("IR Receiver initialized...");
// }

// void loop() {
//   // Phát tín hiệu ON
//   Serial.println("Sending ON signal...");
//   IrSender.sendRaw(rawData_ON, sizeof(rawData_ON) / sizeof(rawData_ON[0]), 38); // 38 kHz
//   delay(2000);

//   // Đọc tín hiệu phát ra
//   checkReceivedSignal();

//   // Phát tín hiệu OFF
//   Serial.println("Sending OFF signal...");
//   IrSender.sendRaw(rawData_OFF, sizeof(rawData_OFF) / sizeof(rawData_OFF[0]), 38); // 38 kHz
//   delay(2000);

//   // Đọc tín hiệu phát ra
//   checkReceivedSignal();
// }

// // Hàm kiểm tra tín hiệu đã nhận
// void checkReceivedSignal() {
//   if (IrReceiver.decode()) {
//     Serial.println("---------------------------");
//     Serial.println("IR signal received!");

//     // In mã HEX
//     if (IrReceiver.decodedIRData.decodedRawData != 0) {
//       Serial.print("Decoded HEX Code: ");
//       Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
//     }

//     // In giao thức
//     Serial.print("Protocol: ");
//     if (IrReceiver.decodedIRData.protocol == NEC) {
//       Serial.println("NEC");
//     } else {
//       Serial.println("Unknown");
//     }

//     // In dữ liệu RAW
//     if (IrReceiver.decodedIRData.rawDataPtr != nullptr) {
//       Serial.println("Raw Data:");
//       size_t rawLength = IrReceiver.decodedIRData.rawDataPtr->rawlen; // Độ dài dữ liệu
//       for (size_t i = 1; i < rawLength; i++) {
//         uint16_t rawValue = IrReceiver.decodedIRData.rawDataPtr->rawbuf[i];
//         Serial.print(rawValue); // Giá trị nguyên bản
//         Serial.print(" ");
//       }
//       Serial.println();
//     } else {
//       Serial.println("No raw data received!");
//     }

//     Serial.println("---------------------------");
//     IrReceiver.resume(); // Tiếp tục nhận tín hiệu
//   }
// }
//========================================================================================================================================================


// #include <IRremote.h>

// // Pin nhận tín hiệu hồng ngoại
// int RECV_PIN = 14;

// void setup() {
//   Serial.begin(115200);

//   // Khởi tạo bộ nhận
//   IrReceiver.begin(RECV_PIN, ENABLE_LED_FEEDBACK);
//   Serial.println("IR Receiver initialized...");
// }

// void loop() {
//   // Kiểm tra xem có tín hiệu hồng ngoại nào được nhận hay không
//   if (IrReceiver.decode()) {
//     Serial.println("IR signal received!");

//     // In mã tín hiệu dạng HEX
//     unsigned long receivedCode = IrReceiver.decodedIRData.decodedRawData;
//     Serial.print("Decoded HEX Code: ");
//     Serial.println(receivedCode, HEX);

//     // In số bit của tín hiệu
//     Serial.print("Number of bits: ");
//     Serial.println(IrReceiver.decodedIRData.numberOfBits);

//     // Đo tần số từ tín hiệu raw
//     measureCarrierFrequency();

//     // Tiếp tục nhận tín hiệu
//     IrReceiver.resume();
//   }
// }

// // Hàm đo tần số sóng mang từ tín hiệu raw
// void measureCarrierFrequency() {
//   if (IrReceiver.decodedIRData.rawDataPtr && IrReceiver.decodedIRData.rawDataPtr->rawlen > 1) {
//     unsigned int pulseWidth = IrReceiver.decodedIRData.rawDataPtr->rawbuf[1];
//     if (pulseWidth > 0) {
//       float frequency = 1000000.0 / (pulseWidth * 2); // Tần số sóng mang
//       Serial.print("Measured Carrier Frequency: ");
//       Serial.print(frequency, 2);
//       Serial.println(" Hz");
//     } else {
//       Serial.println("Unable to measure carrier frequency.");
//     }
//   } else {
//     Serial.println("Raw data not available for carrier frequency measurement.");
//   }
// }
//========================================================================================================================================================

#include <IRremote.h>

// Pin nhận tín hiệu hồng ngoại
int RECV_PIN = 14;

// Pin phát tín hiệu hồng ngoại
#define IR_SEND_PIN 2

// Mã tín hiệu từ remote
// unsigned long REMOTE_ON = 0xFC03EF00;
// unsigned long REMOTE_OFF = 0xFD02EF00;
unsigned long REMOTE_ON = 0xF7C03F; // Mã rút gọn
unsigned long REMOTE_OFF = 0xF740BF; // Mã rút gọn
unsigned long QUAT_ON_OFF = 0xE51A7F80;
unsigned long QUAT_ENCODE = 0x1FE58A7;

// Biến trạng thái
bool ledState = false;

// Thời gian
unsigned long lastSendTime = 0;

void setup() {
  Serial.begin(115200);

  // Khởi tạo bộ nhận
  IrReceiver.begin(RECV_PIN, ENABLE_LED_FEEDBACK);
  Serial.println("IR Receiver initialized...");

  // Khởi tạo bộ phát
  IrSender.begin(IR_SEND_PIN);
  Serial.println("IR Sender initialized...");
}

void loop() {
  unsigned long currentTime = millis();

  if (currentTime - lastSendTime >= 1000) {
    lastSendTime = currentTime;

    if (ledState) {
      Serial.println("Sending ON signal...");
      IrSender.sendNEC(REMOTE_ON, 32); // Gửi mã nhận được từ remote gốc
    } else {
      Serial.println("Sending OFF signal...");
      IrSender.sendNEC(REMOTE_OFF, 32);
    }

    ledState = !ledState;
  }

  // Nhận tín hiệu từ remote
  if (IrReceiver.decode()) {
    unsigned long receivedCode = IrReceiver.decodedIRData.decodedRawData;

    // Hiển thị thông tin tín hiệu
    Serial.print("Received IR signal: ");
    Serial.println(receivedCode, HEX);

    // Kiểm tra giao thức
    if (IrReceiver.decodedIRData.protocol == NEC) {
      Serial.println("Protocol: NEC");
    } else {
      Serial.println("Protocol: Unknown");
      Serial.println(IrReceiver.decodedIRData.protocol);
    }

    IrReceiver.resume(); // Tiếp tục nhận tín hiệu
  }
}