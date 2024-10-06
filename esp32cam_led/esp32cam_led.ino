// 定義引腳
const int buttonPin = 0;  // IO0 引腳
const int ledPin = 4;     // LED 引腳（通常是 GPIO 4）

void setup() {
    Serial.begin(115200); // 初始化串口，波特率為 115200
    pinMode(buttonPin, INPUT_PULLUP);  // 設定 IO0 為輸入，並啟用內部上拉電阻
    pinMode(ledPin, OUTPUT);             // 設定 LED 引腳為輸出
    digitalWrite(ledPin, LOW);           // 確保 LED 初始狀態為熄滅
}

void loop() {
    // 讀取按鈕狀態
    int buttonState = digitalRead(buttonPin);
    
    // 檢查按鈕是否被按下
    if (buttonState == LOW) { // 按鈕按下時，IO0 為 LOW
        digitalWrite(ledPin, HIGH); // 點亮 LED
        Serial.println("Button State: Pressed"); // 傳送狀態到 UART
    } else {
        digitalWrite(ledPin, LOW);  // 熄滅 LED
        Serial.println("Button State: Not Pressed"); // 傳送狀態到 UART
    }

    delay(100); // 防抖延遲
}
