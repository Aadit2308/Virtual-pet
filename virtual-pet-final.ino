#include <WiFi.h>
#include <WebServer.h>
#include <EEPROM.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define EEPROM_SIZE 512

#define FEED_BTN 33
#define PLAY_BTN 25
#define SLEEP_BTN 26
#define CLEAN_BTN 27

WebServer server(80);

String petName = "Mochi";
String wifiSSID = "";
String wifiPass = "";

bool wifiConnected = false;

int feedPoints, playPoints, sleepPoints, cleanPoints;
int foodCount = 2, toyCount = 1, soapCount = 1, pillCount = 1;
int level = 1, xpPoints = 0, xpToNextLevel = 10;
bool blinkState = false, inQuiz = false;
int correctOption = 0;
unsigned long lastFeedTime = 0, lastPlayTime = 0, lastSleepTime = 0, lastCleanTime = 0;
unsigned long lastEduTime = 0;
const unsigned long DECAY_INTERVAL = 20000;
const unsigned long EDU_INTERVAL = 10000;
int btnClickCount = 0;
unsigned long lastIdleTime = 0;

const char setupPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Virtual Pet Setup</title>
  <style>
    @import url('https://fonts.googleapis.com/css2?family=Comic+Neue:wght@700&display=swap');
    body {
      font-family: 'Comic Neue', cursive, Arial, sans-serif;
      background: linear-gradient(to bottom right, #ffe8ec, #e0f7fa);
      margin: 0; padding: 0; height: 100vh; overflow: hidden;
      display: flex; justify-content: center; align-items: center;
      position: relative;
    }
    .emoji {
      position: absolute; font-size: 32px; z-index: 1; pointer-events: none;
    }
    .emoji1 { top: 10px; left: 10px; }
    .emoji2 { top: 10px; right: 20px; }
    .emoji3 { bottom: 20px; left: 20px; }
    .emoji4 { bottom: 10px; right: 10px; }
    .emoji5 { top: 50%; left: 5%; transform: translateY(-50%); }
    .emoji6 { top: 50%; right: 5%; transform: translateY(-50%); }
    form {
      background: #ffffffcc; padding: 30px; border-radius: 20px;
      box-shadow: 0 8px 16px rgba(0,0,0,0.2);
      max-width: 400px; width: 90%; z-index: 2; position: relative;
    }
    h2 {
      text-align: center; color: #ff4081; font-size: 26px; margin-bottom: 20px;
    }
    label {
      color: #333; font-weight: bold; display: block; margin-top: 12px; font-size: 16px;
    }
    input[type="text"],
    input[type="password"] {
      width: 100%; padding: 10px; margin-top: 6px;
      border: 2px solid #ffdce0; border-radius: 12px; font-size: 16px;
      background-color: #fff9fa;
    }
    input[type="submit"] {
      margin-top: 20px; padding: 12px; width: 100%;
      background: #ff80ab; color: white; border: none; border-radius: 14px;
      font-size: 18px; cursor: pointer; transition: background 0.3s ease;
    }
    input[type="submit"]:hover {
      background: #ff4081;
    }
  </style>
</head>
<body>
  <div class="emoji emoji1">🐶</div>
  <div class="emoji emoji2">🧸</div>
  <div class="emoji emoji3">🎮</div>
  <div class="emoji emoji4">💖</div>
  <div class="emoji emoji5">✨</div>
  <div class="emoji emoji6">🦄</div>

  <form action="/save" method="POST">
    <h2>🐾 Setup Your Virtual Pet 🎉</h2>

    <label for="ssid">📶 Wi-Fi SSID:</label>
    <input type="text" id="ssid" name="ssid" placeholder="e.g. MyHomeWiFi" required value="%SSID%">

    <label for="password">🔒 Wi-Fi Password:</label>
    <input type="password" id="password" name="password" placeholder="••••••••" required value="%PASS%">

    <label for="petname">🐕 Pet Name:</label>
    <input type="text" id="petname" name="petname" placeholder="e.g. Mochi 🐾" required value="%PETNAME%">

    <input type="submit" value="🌈 Save & Connect 🚀">
  </form>
</body>
</html>
)rawliteral";

void saveSettings() {
  EEPROM.begin(EEPROM_SIZE);
  int addr = 0;

  int len = wifiSSID.length();
  EEPROM.write(addr++, len);
  for (int i = 0; i < len; i++) EEPROM.write(addr++, wifiSSID[i]);

  len = wifiPass.length();
  EEPROM.write(addr++, len);
  for (int i = 0; i < len; i++) EEPROM.write(addr++, wifiPass[i]);

  len = petName.length();
  EEPROM.write(addr++, len);
  for (int i = 0; i < len; i++) EEPROM.write(addr++, petName[i]);

  EEPROM.write(addr++, feedPoints);
  EEPROM.write(addr++, playPoints);
  EEPROM.write(addr++, sleepPoints);
  EEPROM.write(addr++, cleanPoints);

  EEPROM.write(addr++, level);
  EEPROM.write(addr++, xpPoints);
  EEPROM.write(addr++, xpToNextLevel);

  EEPROM.write(addr++, foodCount);
  EEPROM.write(addr++, toyCount);
  EEPROM.write(addr++, soapCount);
  EEPROM.write(addr++, pillCount);

  EEPROM.commit();
  EEPROM.end();
}

void loadSettings() {
  EEPROM.begin(EEPROM_SIZE);
  int addr = 0;

  int len = EEPROM.read(addr++);
  wifiSSID = "";
  for (int i = 0; i < len; i++) wifiSSID += char(EEPROM.read(addr++));

  len = EEPROM.read(addr++);
  wifiPass = "";
  for (int i = 0; i < len; i++) wifiPass += char(EEPROM.read(addr++));

  len = EEPROM.read(addr++);
  petName = "";
  for (int i = 0; i < len; i++) petName += char(EEPROM.read(addr++));

  feedPoints = EEPROM.read(addr++);
  playPoints = EEPROM.read(addr++);
  sleepPoints = EEPROM.read(addr++);
  cleanPoints = EEPROM.read(addr++);

  level = EEPROM.read(addr++);
  xpPoints = EEPROM.read(addr++);
  xpToNextLevel = EEPROM.read(addr++);

  foodCount = EEPROM.read(addr++);
  toyCount = EEPROM.read(addr++);
  soapCount = EEPROM.read(addr++);
  pillCount = EEPROM.read(addr++);

  EEPROM.end();
}

String processor(const String& var) {
  if (var == "SSID") return wifiSSID;
  if (var == "PASS") return wifiPass;
  if (var == "PETNAME") return petName;
  return "";
}

void handleRoot() {
  String page = setupPage;
  page.replace("%SSID%", wifiSSID);
  page.replace("%PASS%", wifiPass);
  page.replace("%PETNAME%", petName);
  server.send(200, "text/html", page);
}

void handleSave() {
  if (server.hasArg("ssid") && server.hasArg("password") && server.hasArg("petname")) {
    wifiSSID = server.arg("ssid");
    wifiPass = server.arg("password");
    petName = server.arg("petname");

    saveSettings();

    server.send(200, "text/html", "<h1>Saved! Rebooting...</h1>");
    delay(2000);
    ESP.restart();
  } else {
    server.send(400, "text/plain", "Missing parameters");
  }
}

void connectWiFi() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Connecting to Wi-Fi:");
  display.println(wifiSSID);
  display.display();

  WiFi.begin(wifiSSID.c_str(), wifiPass.c_str());

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Wi-Fi Connected!");
    display.println(WiFi.localIP().toString());
    display.display();
  } else {
    wifiConnected = false;
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Wi-Fi Failed");
    display.println("Connect to Mochi_Setup");
    display.display();
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.clearDisplay();
  display.display();

  pinMode(FEED_BTN, INPUT_PULLUP);
  pinMode(PLAY_BTN, INPUT_PULLUP);
  pinMode(SLEEP_BTN, INPUT_PULLUP);
  pinMode(CLEAN_BTN, INPUT_PULLUP);

  randomSeed(analogRead(0));

  EEPROM.begin(EEPROM_SIZE);
  loadSettings();

  if (wifiSSID.length() > 0) {
    connectWiFi();
  }

  if (!wifiConnected) {
    WiFi.softAP("Mochi_Setup");
    IPAddress IP = WiFi.softAPIP();

    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Wi-Fi not found.");
    display.println("Connect to:");
    display.println("Mochi_Setup");
    display.display();
  }

  server.on("/", HTTP_GET, handleRoot);
  server.on("/save", HTTP_POST, handleSave);
  server.begin();

  lastIdleTime = millis();
}

void checkPetButtons() {
  handleButton(FEED_BTN, "Food", &feedPoints);
  handleButton(PLAY_BTN, "Toy", &playPoints);
  handleButton(SLEEP_BTN, "Pill", &sleepPoints);
  handleButton(CLEAN_BTN, "Soap", &cleanPoints);
}

void handleButton(int pin, String item, int* stat) {
  if (!digitalRead(pin)) {
    lastIdleTime = millis();
    unsigned long t = millis();
    while (!digitalRead(pin)) {
      if (millis() - t > 800) {
        if (useItem(item)) {
          playAnimation(item);
        } else {
          display.clearDisplay();
          display.setCursor(0, 28);
          display.println("No more " + item + "s!");
          display.display();
          delay(1500);
        }
        return;
      }
    }
    *stat = min(*stat + 1, 15);
    gainXP();
    btnClickCount++;
    if (btnClickCount >= 15) {
      btnClickCount = 0;
      if (item == "Food" || item == "Pill") showChanceMessage("word");
      if (item == "Toy" || item == "Soap") showChanceMessage("fact");
    }
    delay(200);
  }
}

bool useItem(String item) {
  if (item == "Food" && foodCount > 0) {
    feedPoints = min(feedPoints + 3, 15);
    foodCount--;
    return true;
  } else if (item == "Toy" && toyCount > 0) {
    playPoints = min(playPoints + 3, 15);
    toyCount--;
    return true;
  } else if (item == "Soap" && soapCount > 0) {
    cleanPoints = min(cleanPoints + 3, 15);
    soapCount--;
    return true;
  } else if (item == "Pill" && pillCount > 0) {
    sleepPoints = min(sleepPoints + 3, 15);
    pillCount--;
    return true;
  }
  return false;
}

void playAnimation(String item) {
  String icon;
  if (item == "Food") icon = "\xF0\x9F\x8D\x94";  // 🍔
  if (item == "Toy") icon = "\xF0\x9F\xA7\xB8";   // 🧸
  if (item == "Soap") icon = "\xF0\x9F\xA7\xBC";  // 🧼
  if (item == "Pill") icon = "\xF0\x9F\x92\x8A";  // 💊
  for (int x = 0; x <= 80; x += 10) {
    display.clearDisplay();
    display.setCursor(x, 28);
    display.print(icon);
    display.setCursor(100, 28);
    display.print("\xF0\x9F\x90\xB6"); // 🐶
    display.display();
    delay(100);
  }
}

void gainXP() {
  xpPoints++;
  if (xpPoints >= xpToNextLevel) {
    xpPoints = 0;
    level++;
    xpToNextLevel += 5;
    toyCount++;
    if (level >= 6 || random(0, 100) < 20) startQuiz();
  }
}

void startQuiz() {
  inQuiz = true;
  correctOption = random(0, 4);
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Quiz Time!");
  display.println("Which is a fruit?");
  display.println("A. Carrot");
  display.println("B. Apple");
  display.println("C. Broccoli");
  display.println("D. Onion");
  display.display();
}

void checkQuizButtons() {
  if (!digitalRead(FEED_BTN)) handleQuizAnswer(0);
  if (!digitalRead(PLAY_BTN)) handleQuizAnswer(1);
  if (!digitalRead(SLEEP_BTN)) handleQuizAnswer(2);
  if (!digitalRead(CLEAN_BTN)) handleQuizAnswer(3);
}

void handleQuizAnswer(int pressed) {
  if (!inQuiz) return;
  inQuiz = false;
  display.clearDisplay();
  display.setCursor(0, 24);
  if (pressed == correctOption) {
    level++;
    toyCount += 2;
    display.println("Correct! Lv Up!");
  } else {
    display.println("Oops! Try again!");
  }
  display.display();
  delay(2000);
}

void showChanceMessage(String type) {
  if (millis() - lastEduTime < EDU_INTERVAL) return;
  lastEduTime = millis();
  showEducationalMessage(type);
}

void showEducationalMessage(String type) {
  display.clearDisplay();
  if (type == "word") {
    String words[][2] = {
      {"Amicable", "Friendly"},
      {"Brisk", "Quick and active"},
      {"Radiant", "Bright and shining"},
      {"Curious", "Eager to learn"}
    };
    int i = random(0, 4);
    display.setCursor(0, 10);
    display.print("Word: ");
    display.println(words[i][0]);
    display.setCursor(0, 28);
    display.print("Meaning: ");
    display.println(words[i][1]);
  } else {
    String facts[] = {
      "Octopuses have 3 hearts!",
      "Honey never spoils!",
      "Bananas are berries!",
      "Sharks came before trees!"
    };
    int i = random(0, 4);
    display.setCursor(0, 16);
    display.println("Did you know?");
    display.setCursor(0, 32);
    display.println(facts[i]);
  }
  display.display();
  delay(3000);
}

void showPetDream() {
  String dreams[] = {
    "zZz...", "🌌 Dreaming...", "🐟 Fish... Yum...", "🚀 Fly to moon?"};
  int d = random(0, 4);
  display.clearDisplay();
  display.setCursor(20, 24);
  display.println(dreams[d]);
  display.display();
  delay(2000);
  lastIdleTime = millis();
}

void decayStats() {
  unsigned long now = millis();
  if (now - lastFeedTime >= DECAY_INTERVAL) {
    feedPoints = max(feedPoints - 1, 0);
    lastFeedTime = now;
  }
  if (now - lastPlayTime >= DECAY_INTERVAL) {
    playPoints = max(playPoints - 1, 0);
    lastPlayTime = now;
  }
  if (now - lastSleepTime >= DECAY_INTERVAL) {
    sleepPoints = max(sleepPoints - 1, 0);
    lastSleepTime = now;
  }
  if (now - lastCleanTime >= DECAY_INTERVAL) {
    cleanPoints = max(cleanPoints - 1, 0);
    lastCleanTime = now;
  }
}

void drawAll(bool blink) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(petName);
  display.print(" (Lvl ");
  display.print(level);
  display.println(")");
  if (feedPoints < 5) {
    display.setCursor(0, 8);
    display.print("Mood: Hungry");
    display.setCursor(80, 0);
    display.print("Feed");
    display.setCursor(80, 8);
    display.print("with me!");
    display.setCursor(34, 24);
    display.println(" ( >.< ) ");
  } else if (playPoints < 5) {
    display.setCursor(0, 8);
    display.print("Mood: Bored");
    display.setCursor(80, 0);
    display.print("Play");
    display.setCursor(80, 8);
    display.print("with me!");
    display.setCursor(34, 24);
    display.println(" ( -.- ) ");
  } else if (sleepPoints < 5) {
    display.setCursor(0, 8);
    display.print("Mood: Sleepy");
    display.setCursor(80, 0);
    display.print("Let me");
    display.setCursor(80, 8);
    display.print("sleep!");
    display.setCursor(34, 24);
    display.println(" ( -_-) ");
  } else if (cleanPoints < 5) {
    display.setCursor(0, 8);
    display.print("Mood: Dirty");
    display.setCursor(80, 0);
    display.print("Bath");
    display.setCursor(80, 8);
    display.print("me!");
    display.setCursor(34, 24);
    display.println(" ( >.< ) ");
  } else {
    display.setCursor(0, 8);
    display.print("Mood: Happy");
    display.setCursor(40, 20);
    if (blink) display.println("( -_- )");
    else display.println("( ^_^ )");
  }
  display.setCursor(0, 44);
  display.print("F:"); display.print(feedPoints); display.print("/15");
  display.setCursor(64, 44);
  display.print("P:"); display.print(playPoints); display.print("/15");
  display.setCursor(0, 52);
  display.print("S:"); display.print(sleepPoints); display.print("/15");
  display.setCursor(64, 52);
  display.print("C:"); display.print(cleanPoints); display.print("/15");
  display.display();
}

void loop() {
  server.handleClient();

  if (inQuiz) {
    checkQuizButtons();
  } else {
    checkPetButtons();
    decayStats();
    blinkState = !blinkState;
    if (millis() - lastIdleTime > 60000) {
      showPetDream();
    } else {
      drawAll(blinkState);
    }
  }

  delay(400);
}