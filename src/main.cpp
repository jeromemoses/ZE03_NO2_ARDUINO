#include <Arduino.h>
#include <SoftwareSerial.h>
#define sensor_baud_rate 9600 // sensor only works with baud rate of 9600
#define ESP_baud_rate 115200  // separate baud rate for ESP32 can be changed
#define LED1 2                // InBuilt led pin of ESP32

SoftwareSerial NO2_serial(18, 19); // Establising software

byte NO2_qa_mode[] = {0xFF, 0x01, 0x78, 0x04, 0x00, 0x00, 0x00, 0x00, 0x83}; // Command to start O2 sensor in QnA mode
byte NO2_read[] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};    // command to read O2 concentration under QnA mode
byte NO2_received_bytes[9];                                                  // Array to store sensor response

float read_NO2(float *gas_con); // function to read o2 concentration
float NO2;                      // variable to store O2 value

void setup()
{
  Serial.begin(ESP_baud_rate);        // Initialise serial monitor from ESP32
  NO2_serial.begin(sensor_baud_rate); // Initialise Uart communication with O2 sensor
  pinMode(LED1, OUTPUT);              // declaring esp32 inbuilt led pin as output

  delay(1000);

  NO2_serial.write(NO2_qa_mode, sizeof(NO2_qa_mode));
  delay(250);
  NO2_serial.readBytes(NO2_received_bytes, sizeof(NO2_received_bytes));

  // prints the received bytes in serial monitor
  for (int i = 0; i <= 8; i++)
  {
    Serial.print(NO2_received_bytes[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  // sensor booting section
  Serial.println("Sensor is booting Please wait");
  int bootUP_time = 0;
  while (bootUP_time != 10)
  {
    Serial.print(".");
    delay(1000);
    bootUP_time += 1;
  }
  Serial.println();
}

void loop()
{

  delay(1840);

  read_NO2(&NO2); // passing O2 variable address to write the o2 gas concentration value in it

  Serial.printf("Present NO2 : %.2f \n", NO2); // prints O2 concentration in a serial monitor

  // code is just to print loading dots and simply blinks the builtin led in esp32 dev module
  for (int i = 0; i <= 40; i++)
  {
    digitalWrite(LED1, HIGH);
    Serial.print(".");
    delay(25);
    digitalWrite(LED1, LOW);
    delay(25);
  }
  Serial.println("");
  // End of loading dots print function
}

float read_NO2(float *gas_con)
{
  if (NO2_serial.write(NO2_read, sizeof(NO2_read)) == 9) // send O2 read command to o2 sensor and checks whether it id fully sent
  {
    delay(250);
    NO2_serial.readBytes(NO2_received_bytes, sizeof(NO2_received_bytes));
    *gas_con = NO2_received_bytes[2] * 256 + NO2_received_bytes[3]; // Substituting sensor responses to the gas concentration formula
    *gas_con = *gas_con * 0.1;
  }

  // debug code -> to print received bytes into serial monitor
  for (byte j = 0; j < 9; j++)
  {
    Serial.print(NO2_received_bytes[j], HEX); // Prints received byte from ZE03 sensor response
    Serial.print(" ");
  }
  Serial.print("\t");
  return *gas_con; // returns the gas concentration of O2
}