/*
 *   按下IO0傳給8051
 * 
 */

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
byte receivedData[14];  // 接收的數據
int receivedIndex = 0; // 接收數據的索引


// 定義變量

int dataIndex = 0;      // 接收數據的索引
bool receiving = false;  // 是否正在接收數據

unsigned long previousMillis = 0;  // 儲存上次更新的時間
const long interval = 1000;          // 設定間隔時間

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
    Serial2.write(ETX);  // 發送 ETX
    Serial2.write(LRC);  // 發送 LRC

    // 發送 ACK
//    Serial2.write(ACK);
}
// 處理接收的數據
void handleIncomingData() {
    while (Serial2.available()) {
        byte incomingByte = Serial2.read(); // 讀取字節

        // 檢查是否是 STX
        if (incomingByte == STX) {
            receivedIndex = 0; // 重置索引，不存儲 STX
        } else if (receivedIndex < sizeof(receivedData) - 1) {
            // 在 STX 和 ETX 之間接收數據
            receivedData[receivedIndex++] = incomingByte;
        }

        // 檢查是否接收到完整數據包
        if (receivedIndex >= 3 && receivedData[receivedIndex - 2] == ETX) {
            byte LRC = calculateLRC(receivedData, receivedIndex - 2); // 計算 LRC，不包括 ETX 和 LRC
            byte receivedLRC = receivedData[receivedIndex - 1]; // 最後一個字節是 LRC
/*
            // 打印接收到的數據和 LRC
            Serial.print("Received Data: ");
            for (int i = 0; i < receivedIndex; i++) {
                Serial.print(receivedData[i], HEX); // 以十六進制格式顯示
                Serial.print(" ");
            }
            Serial.print("LRC: ");
            Serial.print(LRC, HEX);
            Serial.print("    receivedLRC: ");
            Serial.println(receivedLRC, HEX);
*/
            // 檢查 LRC 是否匹配
            if (LRC == receivedLRC) {
                processReceivedData(receivedData, receivedIndex - 2); // 處理數據，不包括 ETX 和 LRC
            } else {
                Serial.println("LRC check failed!");
            }
            receivedIndex = 0; // 重置索引以接收下一個數據包
        }
    }
}

// 處理接收到的數據
void processReceivedData(byte *data, int length) {
    Serial.print("Received Data: ");
    for (int i = 0; i < length; i++) {
        dataBytes[i] = data[i];
        Serial.print(data[i], HEX); // 以十六進制格式顯示
        Serial.print(" ");
    }
    Serial.println();
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
        //Serial.println("Button State: Pressed"); // 傳送狀態到 SERIAL
    } else {
        dataBytes[0] = 0x4C;  // 設置數據陣列的第一個字節
        //Serial.println("Button State: Not Pressed"); // 傳送狀態到 SERIAL
    }
    
 
    handleIncomingData();  // 處理接收到的數據
    // 檢查 dataBytes[1] 是否等於 0x48
    if (dataBytes[1] == 0x48) {
        digitalWrite(ledPin, HIGH);  // 點亮 LED
    } else {
        digitalWrite(ledPin, LOW);   // 熄滅 LED
    }

    yield(); // 讓 ESP32 檢查看門狗定時器
    //delay(500); // 延遲 500 毫秒
    // 非阻塞延遲
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        // 在這裡執行需要延遲的代碼
        // 發送數據到 Serial2
        Serial.println("millis");
        sendData(dataBytes, sizeof(dataBytes) / sizeof(dataBytes[0]));
    } 
}
