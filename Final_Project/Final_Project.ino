#include <Keypad.h>
#include <PubSubClient.h>
#include <WiFi.h>
  
const char *ssid_Router     = "NETGEAR93"; //Enter the router name
const char *password_Router = "dailypiano261"; //Enter the router password
int latchPin = 2;          // Pin connected to ST_CP of 74HC595（Pin12）
int clockPin = 4;          // Pin connected to SH_CP of 74HC595（Pin11）
int dataPin = 15;          // Pin connected to DS of 74HC595（Pin14
String newBoard;

// define the symbols on the buttons of the keypad
char keys[4][4] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

// Defines the pattern data for the board
int Board[] = {
  0x24, 0x24, 0xff, 0x24, 0x24, 0xff, 0x24, 0x24
};

byte rowPins[4] = {14, 27, 26, 25}; // connect to the row pinouts of the keypad
byte colPins[4] = {13, 21, 22, 23};   // connect to the column pinouts of the keypad

// initialize an instance of class NewKeypad
Keypad myKeypad = Keypad(makeKeymap(keys), rowPins, colPins, 4, 4);

// Configure MQTT
const char* mqtt_server = "test.mosquitto.org";
WiFiClient espClient;
PubSubClient client(espClient);

void setup(){
  Serial.begin(115200);
  delay(2000);
  Serial.println("Setup start");
  WiFi.begin(ssid_Router, password_Router);
  Serial.println(String("Connecting to ")+ssid_Router);

  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected, IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Setup End");
  // set MQTT server
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  // set pins to output
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
}
 
void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char) message[i]);
    newBoard += (char) message[i];
  }
  Serial.println();

}

void reconnect() {
  // Loop until reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    //Attempt to connect
    if (client.connect("MQTTBrokerClient")) {
      Serial.println("connected");
      //Subscribe
      client.subscribe("acs/board");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      //Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
  //Connect to MQTT broker
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Define a one-byte variable (8 bits) which is used to represent the selected state of 8 column.
  int cols;

  // Get the character input
  char keyPressed = myKeypad.getKey();
  char keyPressedValue[2];

  // Display the static board pattern
  for (int j = 0; j < 1; j++ ) {  // repeat 500 times
    cols = 0x01;
    for (int i = 0; i < 8; i++) {   // display 8 column data by scaning
      matrixRowsVal(Board[i]);// display the data in this column
      matrixColsVal(~cols);          // select this column
      delay(1);                     // display them for a period of time
      matrixRowsVal(0x00);          // clear the data of this column
      cols <<= 1;                   // shift"cols" 1 bit left to select the next column
    }
  }

  // If there is a character input, sent it to the serial port
  if (keyPressed) {
    Serial.println(keyPressed);
    //Publish keypressed value
    keyPressedValue[0] = keyPressed;
    keyPressedValue[1] = 0;
    client.publish("acs/move", keyPressedValue);
    Serial.print("keyPressed: ");
    Serial.println(keyPressedValue);

    for (int i = 0; i < 8; i++){
      if (newBoard.charAt(i) == 'X'){
        if (keyPressed == '1'){
          Board[0] += 0x4;
          Board[1] += 0x1;
        }
        else if (keyPressed == '2'){
          Board[3] = 0x4;
          Board[4] = 0x1;
        }
        else if (keyPressed == '3'){
          Board[0] = 0x2C;
          Board[1] = 0x34;
        }
        else if (keyPressed == '4'){
          Board[0] = 0x2C;
          Board[1] = 0x34;
        }
        else if (keyPressed == '5'){
          Board[0] = 0x2C;
          Board[1] = 0x34;
        }
        else if (keyPressed == '6'){
          Board[0] = 0x2C;
          Board[1] = 0x34;
        }
        else if (keyPressed == '7'){
          Board[0] = 0x2C;
          Board[1] = 0x34;
        }
        else if (keyPressed == '8'){
          Board[0] = 0x2C;
          Board[1] = 0x34;
        }
        else if (keyPressed == '9'){
          Board[0] = 0x2C;
          Board[1] = 0x34;
        }
        else if (keyPressed == 'A'){
          Board[0] = 0x2C;
          Board[1] = 0x34;
        }
      }
      else if (newBoard.charAt(i) == 'O'){

      }
    }
  }

}

void matrixRowsVal(int value) {
  // make latchPin output low level
  digitalWrite(latchPin, LOW);
  // Send serial data to 74HC595
  shiftOut(dataPin, clockPin, LSBFIRST, value);
  // make latchPin output high level, then 74HC595 will update the data to parallel output
  digitalWrite(latchPin, HIGH);
}

void matrixColsVal(int value) {
  // make latchPin output low level
  digitalWrite(latchPin, LOW);
  // Send serial data to 74HC595
  shiftOut(dataPin, clockPin, MSBFIRST, value);
  // make latchPin output high level, then 74HC595 will update the data to parallel output
  digitalWrite(latchPin, HIGH);
}
