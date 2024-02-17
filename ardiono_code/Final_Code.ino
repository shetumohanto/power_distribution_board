#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define sample 50 //number of instantaneous measurement in one loop
#define voltage_sensor_pin_num A1 //voltage sensor connected pin number
#define current_sensor_pin_num A2 //current sensor connected pin number
#define freq 50 //operating frequency of the applied voltage
#define voltage_offset 2.49
#define tune_parameter 8.9 //voltmeter
#define current_resistance 480
#define local_relay 8
#define button_01 7
#define green_led 4
#define red_led 3
#define buzzer 5
#define max_current_limit 0.23 //Ampere ac rms
#define max_voltage_limit 240.0 //Voltage ac rms
#define min_voltage_limit 190.0 //Voltage ac rms
#define over_current_stay 100 //milli second
int T_voltage = 1000 * 1000 / freq; //period of applied sine voltage


//double inst_vol, sqre, sum;
//double rms_vol, t1, t2, val;
//float n = 1, avg = 0;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int digitalReadOutputPin(uint8_t pin)
{
  uint8_t bit = digitalPinToBitMask(pin);
  uint8_t port = digitalPinToPort(pin);
  if (port == NOT_A_PIN)
    return LOW;

  return (*portOutputRegister(port) & bit) ? HIGH : LOW;
}

void setup() {
  // put your setup code here, to run once:
  //analogReference(EXTERNAL);
  pinMode(voltage_sensor_pin_num, INPUT);
  pinMode(current_sensor_pin_num, INPUT);
  pinMode(local_relay, OUTPUT);
  digitalWrite(local_relay, HIGH);
  pinMode(green_led, OUTPUT);
  pinMode(red_led, OUTPUT);
  pinMode(buzzer, OUTPUT);
  digitalWrite(local_relay, HIGH);
  pinMode(button_01, INPUT);
  Serial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    Serial.println(F("SSD1306 allocation failed"));

}

bool button_last_state = digitalRead(button_01); //false means switch is turned off, true means on
bool button_current_state, overcurrent_voltage = false;
void loop() {
  button_current_state = digitalRead(button_01);
  if (button_current_state != button_last_state) {
    button_last_state = button_current_state;
    if (button_current_state == true) {
      digitalWrite(local_relay, !digitalReadOutputPin(local_relay));
      overcurrent_voltage = false;
      digitalWrite(red_led, LOW);
      digitalWrite(buzzer, LOW);
    }
  }
  digitalWrite(green_led, !digitalReadOutputPin(local_relay));
  
  power_mea();
  //  delay(500);
}
