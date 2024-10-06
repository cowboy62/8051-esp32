// 定義引腳
const int buttonPin = 0;  // IO0 引腳
const int ledPin = 4;     // LED 引腳（通常是 GPIO 4）
#define RXD2 12
#define TXD2 13

// 定義字節常量
byte STX = 0x02;  // 開始標記
byte ETX = 0x03;  // 結束標記
byte ACK = 0x06;  // 確認標記

// 數據陣列
byte dataBytes[10] = {0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A};

// 計算 LRC 的函數
byte calculateLRC(byte *data, size_t length) {
    byte LRC = 0;  // LRC 的初始值
    for (size_t i = 0; i < length; i++) {
        LRC ^= data[i];  // 進行 XOR 運算
    }
    LRC ^= ETX;  // XOR 運算 ETX
    return LRC;
}

// 發送數據的函數
void sendData(byte *data, size_t length) {
    // 計算 LRC
    byte LRC = calculateLRC(data, length);

    // 構建完整數據包
    Serial2.write(STX);  // 發送 STX
    Serial2.write(data, length);  // 發送 DATA
    Serial2.write(LRC);  // 發送 LRC
    Serial2.write(ETX);  // 發送 ETX

    // 發送 ACK
    Serial2.write(ACK);
}

void setup() {
    Serial.begin(115200);  // 初始化串口，波特率為 115200

    Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
    pinMode(buttonPin, INPUT_PULLUP);  // 設定 IO0 為輸入，並啟用內部上拉電阻
    pinMode(ledPin, OUTPUT);             // 設定 LED 引腳為輸出
    digitalWrite(ledPin, LOW);           // 確保 LED 初始狀態為熄滅
}

void loop() {
    // 讀取按鈕狀態
    int buttonState = digitalRead(buttonPin);
    
    // 檢查按鈕是否被按下
    if (buttonState == LOW) { // 按鈕按下時，IO0 為 LOW
        dataBytes[0] = 0x48;  // 設置數據陣列的第一個字節
        Serial.println("Button State: Pressed"); // 傳送狀態到 SERIAL
    } else {
        dataBytes[0] = 0x4C;  // 設置數據陣列的第一個字節
        Serial.println("Button State: Not Pressed"); // 傳送狀態到 SERIAL
    }
    
    // 發送數據到 Serial1
    sendData(dataBytes, sizeof(dataBytes) / sizeof(dataBytes[0]));  
    yield(); // 讓 ESP32 檢查看門狗定時器
    delay(500); // 延遲 500 毫秒
}
