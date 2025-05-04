#include <Wire.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

#define I2C_SLAVE_ADDR 8      //返信用i2Cアドレス
#define WIFI_SSID ""
#define WIFI_PASSWORD ""
#define BOT_TOKEN ""
#define LED_PIN 4
#define CHAT_ID ""    // Chat IDの定義

const unsigned long BOT_MTBS = 1000;

WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long bot_lasttime;
bool newI2CData = false;
String i2cData = "";

void setup() {
  Serial.begin(115200);
  Wire.begin(I2C_SLAVE_ADDR);
  Wire.setClock(50000);// I2Cクロック速度を50kHzに設定
  Wire.onReceive(receiveEvent);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");

  configTime(0, 0, "pool.ntp.org");
  while (time(nullptr) < 24 * 3600) {
    delay(100);
  }

  // I2Cをマスターとして初期化
  Wire1.begin(18,19);            //返信用のi2cピン定義
}

void loop() {
  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');
    bot.sendMessage(CHAT_ID, data, "");
    
  }

  if (millis() - bot_lasttime > BOT_MTBS) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while (numNewMessages) {
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    bot_lasttime = millis();
  }

  if (newI2CData) {
    bot.sendMessage(CHAT_ID, i2cData, "");
    newI2CData = false;
  }
}

void receiveEvent(int howMany) {
  char *receivedMessage = new char[howMany + 1]; // 動的にバッファを確保
  int index = 0;
  while (Wire.available() > 0) {
    receivedMessage[index++] = Wire.read();
  }
  receivedMessage[index] = '\0'; // 文字列終端を追加

  // メインループでTelegram送信を処理するために、ここではデータをシリアルに出力
  Serial.println(receivedMessage);

  i2cData = String(receivedMessage); // 受信データをグローバル変数に格納
  newI2CData = true;// 新しいデータがあることを示すフラグをセット
  digitalWrite(LED_PIN, LOW);
  delay(500);
  digitalWrite(LED_PIN, HIGH);

  delete[] receivedMessage; // メモリを解放し、スタックオーバーフローを回避
}

void handleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;
    Serial.println(text);

    
    Wire1.beginTransmission(0x55);
    for (size_t j = 0; j < text.length(); j++) {
      Wire1.write((uint8_t)text[j]);
    }
    Wire1.endTransmission();

    if (text == "/ledon") {
      digitalWrite(LED_PIN, HIGH);
      bot.sendMessage(chat_id, "Led is ON", "");
    } else if (text == "/ledoff") {
      digitalWrite(LED_PIN, LOW);
      bot.sendMessage(chat_id, "Led is OFF", "");
    } else if (text == "/status") {
      if (digitalRead(LED_PIN) == HIGH) {
        bot.sendMessage(chat_id, "Led is ON", "");
      } else {
        bot.sendMessage(chat_id, "Led is OFF", "");
      }
    } else if (text == "/start") {
      String welcome = "hello ";
      bot.sendMessage(chat_id, welcome, "Markdown");
    }
  }
}
