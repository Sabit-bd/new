#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <HardwareSerial.h>

#define MQ4_PIN                 34
#define BUZZER_PIN              14
#define simSerial               Serial2
#define MCU_SIM_BAUDRATE        115200
#define MCU_SIM_TX_PIN          17
#define MCU_SIM_RX_PIN          16
#define MCU_SIM_EN_PIN          15

#define PHONE_NUMBER            "+8801864407944"

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
    lcd.init();
    lcd.backlight();
    lcd.clear();
    pinMode(MQ4_PIN, INPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW); 

    pinMode(MCU_SIM_EN_PIN, OUTPUT); 
    digitalWrite(MCU_SIM_EN_PIN, LOW);

    delay(20);
    Serial.begin(115200);
    delay(8000); 
    simSerial.begin(MCU_SIM_BAUDRATE, SERIAL_8N1, MCU_SIM_RX_PIN, MCU_SIM_TX_PIN);

    
    sim_at_cmd("AT");
    delay(1000);

    
    sim_at_cmd("AT+CMGF=1");
    delay(1000);
}

void loop() {
    int sensorValue = analogRead(MQ4_PIN);
    float voltage = sensorValue * 100 * (3.3 / 4095.0);

    lcd.clear();
    if (voltage > 300) {
        lcd.setCursor(0, 0);
        lcd.print("Emergency Gas");
        lcd.setCursor(0, 1);
        lcd.print("Detected! Alarm");
        digitalWrite(BUZZER_PIN, HIGH); 
        sent_sms("Emergency Gas Detected! Alarm Triggered!");
        call();
    } else if (voltage > 250) {
        lcd.setCursor(0, 0);
        lcd.print("Gas Detected");
        lcd.setCursor(0, 1);
        lcd.print("Sending SMS...");
        sent_sms("Gas Detected! Sending SMS...");
        digitalWrite(BUZZER_PIN, LOW); 
    } else {
        lcd.setCursor(0, 0);
        lcd.print("Gas Value:");
        lcd.setCursor(0, 1);
        lcd.print(voltage, 2);
        lcd.print(" V");
        digitalWrite(BUZZER_PIN, LOW); 
    }

    delay(1000);
}

void sim_at_wait() {
    delay(100);
    while (simSerial.available()) {
        Serial.write(simSerial.read());
    }
}

bool sim_at_cmd(String cmd) {
    simSerial.println(cmd);
    sim_at_wait();
    return true;
}

bool sim_at_send(char c) {
    simSerial.write(c);
    return true;
}

void sent_sms(const char* message) {
    sim_at_cmd("AT+CMGS=\"" + String(PHONE_NUMBER) + "\"");
    simSerial.print(message);
    sim_at_send(0x1A); 
}

void call() {
    String temp = "ATD";
    temp += PHONE_NUMBER;
    temp += ";";
    sim_at_cmd(temp);

    delay(20000);

    
    sim_at_cmd("ATH");
}
