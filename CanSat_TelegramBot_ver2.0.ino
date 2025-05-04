/*配線、備忘録として
TTGO LoRa32(Receive) ---> ESP32 WROVER-D(TelegramBot)

              GPIO22 ---> GPIO19
              GPIO21 ---> GPIO18
              GPIO04 ---> GPIO21
              GPIO15 ---> GPIO22
              GND    ---> GND

            (GPIO25) ---> (LED)
      
*/

#include <Wire.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

#define I2C_SLAVE_ADDR 8      //返信用i2Cアドレス
#define WIFI_SSID "elecom-ee9273"
#define WIFI_PASSWORD "45nx69uwed64"
#define BOT_TOKEN "7813137636:AAEKRKprt0E_RuUC0xtVC6qxbCc0RD3JP9c"
#define LED_PIN 4
#define CHAT_ID "-1002381703054"    // Chat IDの定義

const unsigned long BOT_MTBS = 1000;

WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long bot_lasttime;
bool newI2CData = false;
String i2cData = "";

void setup() {
  pinMode(2,OUTPUT);
  pinMode(4,OUTPUT);
  pinMode(0,OUTPUT);
  pinMode(16,OUTPUT);
  pinMode(17,OUTPUT);
  pinMode(26,OUTPUT);
  piMode()
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

    // 受信したTelegramメッセージをI2C経由で別のESP32に送信
    Wire1.beginTransmission(0x55);//救助隊からの返信用i2Cアドレス
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
      welcome += "こちらは法政大学第二高等学校　物理部の缶サット用Telegram Botです。\n";
      welcome += "温度,湿度,気圧,GPS座標,名前（チャットID),チャットメッセージ,ボタンの状態,RSSI(通信強度)の順で情報をお知らせします。\n";
      welcome += "返信する場合は文頭に「/」をつけてください。\n";
      welcome += "電波の状況やインターネットに接続しにくい場合はLoRa受信機のシリアルモニタから直接返信を入力することも可能です。\n";
      bot.sendMessage(chat_id, welcome, "Markdown");
    }
    else if (text == "/ヤシマ作戦、開始")
    {
      bot.sendMessage(chat_id, "ヤシマ作戦開始を承認。LoRa超遠距離通信コマンド発動。第一次電源投入開始。");

      digitalWrite(2,HIGH);
      delay(300);
      digitalWrite(0,HIGH);
      delay(300);
      digitalWrite(4,HIGH);
      delay(300);
      digitalWrite(16,HIGH);
      delay(300);
      digitalWrite(17,HIGH);
      delay(300);
      digitalWrite(26,HIGH);
      delay(300);
      digitalWrite(27,HIGH);
      delay(300);
      digitalWrite(32,HIGH);
      delay(300);
      digitalWrite(2,LOW);
      delay(300);
      digitalWrite(2,LOW);
      delay(300);
      digitalWrite(0,LOW);
      delay(300);
      digitalWrite(4,LOW);
      delay(300);
      digitalWrite(16,LOW);
      delay(300);
      digitalWrite(17,LOW);
      delay(300);
      digitalWrite(26,LOW);
      delay(300);
      digitalWrite(27,LOW);
      delay(300);
      digitalWrite(2,HIGH);
      delay(300);
      digitalWrite(0,HIGH);
      delay(300);
      digitalWrite(4,HIGH);
      delay(300);
      digitalWrite(16,HIGH);
      delay(300);
      digitalWrite(17,HIGH);
      delay(300);
      digitalWrite(26,HIGH);
      delay(300);
      digitalWrite(27,HIGH);
      delay(300);
      digitalWrite(2,LOW);
      delay(300);
      digitalWrite(2,LOW);
      delay(300);
      digitalWrite(0,LOW);
      delay(300);
      digitalWrite(4,LOW);
      delay(300);
      digitalWrite(16,LOW);
      delay(300);
      digitalWrite(2,HIGH);
      delay(300);
      digitalWrite(0,HIGH);
      delay(300);
      digitalWrite(4,HIGH);
      delay(300);
      digitalWrite(16,HIGH);
      delay(300);
      digitalWrite(2,LOW);
      delay(300);

      digitalWrite(0,LOW);
      delay(300);
      
      digitalWrite(4,LOW);
      delay(300);
      digitalWrite(2,HIGH);
      delay(300);
      digitalWrite(0,HIGH);
      delay(300);
      digitalWrite(4,HIGH);
      delay(300);
      digitalWrite(0,LOW);
      delay(300);
      
      digitalWrite(4,LOW);
      delay(300);
      digitalWrite(4,HIGH);

 // turn the LED on (HIGH is the voltage level)
    }

  }
}
