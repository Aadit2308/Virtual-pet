# 🐾 Mochi - ESP32 Virtual Pet

An interactive virtual pet built using the **ESP32**, **OLED Display**, and **push buttons**. The project combines pet simulation, educational mini-games, Wi-Fi configuration, persistent storage, and a web-based setup portal to create a fun embedded systems project.

---

## 📖 Overview

Mochi is a Tamagotchi-inspired virtual pet that lives on an ESP32. Users can feed, play with, clean, and help their pet sleep using dedicated buttons. The pet's health gradually decreases over time, encouraging regular interaction.

The system also includes educational features such as vocabulary words, fun facts, quizzes, experience points (XP), and a leveling system.

Wi-Fi credentials and the pet's name can be configured through a built-in web server and are stored permanently in EEPROM.

---

## ✨ Features

* 🐶 Interactive virtual pet
* 🍔 Feed, Play, Sleep and Clean actions
* 📉 Automatic stat decay over time
* ⭐ XP and Level progression
* 🎁 Inventory system

  * Food
  * Toys
  * Soap
  * Pills
* 📚 Educational vocabulary words
* 🌍 Random fun facts
* ❓ Interactive quiz system
* 😴 Idle sleep/dream animation
* 📺 OLED graphical interface
* 🌐 Wi-Fi setup portal
* 💾 EEPROM storage for persistent settings
* 🔄 Automatic reboot after configuration

---

## 🛠 Hardware Requirements

| Component                         | Quantity    |
| --------------------------------- | ----------- |
| ESP32 Development Board           | 1           |
| SSD1306 OLED Display (128×64 I2C) | 1           |
| Push Buttons                      | 4           |
| Breadboard & Jumper Wires         | As required |
| USB Cable                         | 1           |

---

## 📌 Pin Configuration

### OLED Display

| OLED | ESP32  |
| ---- | ------ |
| SDA  | GPIO21 |
| SCL  | GPIO22 |
| VCC  | 3.3V   |
| GND  | GND    |

### Buttons

| Function | GPIO   |
| -------- | ------ |
| Feed     | GPIO33 |
| Play     | GPIO25 |
| Sleep    | GPIO26 |
| Clean    | GPIO27 |

---

## 📦 Libraries Used

* WiFi
* WebServer
* EEPROM
* Wire
* Adafruit GFX
* Adafruit SSD1306

Install the Adafruit libraries using the Arduino Library Manager before uploading the project.

---

## 🚀 Setup

### 1. Upload the Code

Flash the program to the ESP32 using Arduino IDE.

### 2. First Boot

If Wi-Fi credentials are not stored, the ESP32 creates an Access Point:

```text
Mochi_Setup
```

Connect your phone or computer to this network.

### 3. Open Setup Page

Navigate to:

```text
http://192.168.4.1
```

Enter:

* Wi-Fi SSID
* Wi-Fi Password
* Pet Name

Save the settings.

The ESP32 automatically restarts and connects to your Wi-Fi network.

---

## 🎮 Gameplay

### Feed Button

* Increases hunger points
* Long press uses stored food item
* Can trigger vocabulary lessons

---

### Play Button

* Increases happiness
* Long press uses toy
* May display interesting facts

---

### Sleep Button

* Restores sleep level
* Long press uses medicine

---

### Clean Button

* Restores cleanliness
* Long press uses soap

---

## 📈 Level System

Every interaction earns XP.

When enough XP is collected:

* Level increases
* XP requirement increases
* Inventory rewards are granted
* Educational quizzes may appear

Higher levels unlock additional challenges.

---

## 🧠 Educational Features

### Vocabulary

Examples:

* Amicable – Friendly
* Brisk – Quick and active
* Radiant – Bright and shining
* Curious – Eager to learn

---

### Fun Facts

Examples:

* Octopuses have three hearts.
* Honey never spoils.
* Bananas are berries.
* Sharks existed before trees.

---

### Quiz Mode

Random quizzes appear as the pet levels up.

Correct answers reward:

* Extra levels
* Bonus toys

---

## 💾 EEPROM Storage

The following data is stored permanently:

* Wi-Fi SSID
* Wi-Fi Password
* Pet Name
* Pet statistics
* Inventory
* Current level
* XP

This allows the virtual pet to continue where it left off after restarting.

---

## 😴 Idle Mode

If the pet is left untouched for approximately one minute, it enters a dream mode displaying random dream messages until user interaction resumes.

---

## 📂 Project Structure

```text
ESP32-Virtual-Pet/
│
├── VirtualPet.ino
├── README.md
└── Images/
    ├── setup_page.png
    ├── oled_display.png
    
```

---

## 🌟 Future Improvements

* Mobile application integration
* Sound effects using a buzzer
* RGB LED animations
* Cloud save functionality
* Online leaderboard
* Additional pet species
* Touchscreen support
* Weather integration
* Daily challenges and rewards
* Voice interaction

---

## 📸 Demo

Add screenshots or GIFs here:

* Setup webpage
* OLED interface
* Pet animations
* Quiz mode
* Dream mode

---

## 📄 License

This project is released under the MIT License.

---

## 👨‍💻 Author

**Aadit**

If you found this project useful, consider giving the repository a ⭐ on GitHub.
