#include <RF24.h>
#include <SPI.h>
#include <nRF24L01.h>

int LED = 7;    //pin to LED

int data = -5;    //holds char from Serial
int dig_count = 0;  //track location for next int value for digits[]
//String dataS;
//int charCount = 0;   

RF24 radio(9, 8);         //RF setup for pins
const byte Campipe[5] = {'C','A','M','E','X'};  //RF transmission pipe with motor module
const byte LCDpipe[5] = {'L','C','D','T','X'};  //RF reception pipe with LCD module
byte pipenum;
int message[2];       //holds message sent by LCD module

int color;            //int value for color of object selected by player
int readyReturn = 0;  //flag to indicate when coordinates for selected object
int coords[3];        //holds coordinates and ID to send to motor module

void setup()
{
  Serial.begin(9600);
  radio.begin();
  radio.setAutoAck(true);
  radio.openWritingPipe(Campipe);   //from camera module to motor module
  radio.setPALevel(RF24_PA_LOW);
  radio.setRetries(5,15);
  radio.openReadingPipe(0, LCDpipe); //from LCD module to camera module
  pinMode(LED, OUTPUT);
}

void loop()      
{
  radio.startListening();
  while(!radio.available(&pipenum)) {   //waiting to receive message from LCD module
    digitalWrite(LED, HIGH);            //LED remains on
    break;
  }
  if (radio.available(&pipenum)) {            //message received from LCD module
    digitalWrite(LED, LOW);                   //LED turns off
    radio.read(&message, sizeof(message));    //read message
    if (message[1] == 455) {                  //check ID in message[1]
      color = message[0];                     //get int value for color
    }
  }
  //delay(5000);    //previously had delay here prior to delaying signal until camera stabilized at center position of claw machine
  int output[8];    //holds x & y values for all 4 colors sent to Serial by color detection program

  while (Serial.available() <= 0) {}  //wait until Serial receives

  while (Serial.available() > 0) {    //continue reading from Serial while it has data
    //dataS = Serial.readString();    //initially read from Serial as strings
    //Serial.print(dataS);
    //Serial.print(" ");
    data = Serial.read();             //changed to reading Serials as chars - ASCII values
    Serial.print(data);
    Serial.print("\n");

    int digits[24];   //array to hold each number in Serial
    
    if (data == 32) {       //compare char to specific ASCII value and insert integer-converted number of ASCII value to digits[] 
      digits[dig_count] = 20;
      Serial.print(dig_count);
      Serial.print(" ");
      Serial.print(digits[dig_count]);
      Serial.print("\n");
      ++dig_count;
      //++charCount;
    }
    else if (data == 45) {
      digits[dig_count] = -1;
      Serial.print(dig_count);
      Serial.print(digits[dig_count]);
      Serial.print("\n");
      ++dig_count;
      //++charCount;
    }
    else if (data == 48) {
      digits[dig_count] = 0;
      Serial.print(dig_count);
      Serial.print(" ");
      Serial.print(digits[dig_count]);
      Serial.print("\n");
      ++dig_count;
      //++charCount;
    }
    else if (data == 49) {
      digits[dig_count] = 1;
      Serial.print(dig_count);
      Serial.print(" ");
      Serial.print(digits[dig_count]);
      Serial.print("\n");
      ++dig_count;
      //++charCount;
    }
    else if (data == 50) {
      digits[dig_count] = 2;
      Serial.print(dig_count);
      Serial.print(" ");
      Serial.print(digits[dig_count]);
      Serial.print("\n");
      ++dig_count;
      //++charCount;
    }
    else if (data == 51) {
      digits[dig_count] = 3;
      Serial.print(dig_count);
      Serial.print(" ");
      Serial.print(digits[dig_count]);
      Serial.print("\n");
      ++dig_count;
      //++charCount;
    }
    else if (data == 52) {
      digits[dig_count] = 4;
      Serial.print(dig_count);
      Serial.print(" ");
      Serial.print(digits[dig_count]);
      Serial.print("\n");
      ++dig_count;
      //++charCount;
    }
    else if (data == 53) {
      digits[dig_count] = 5;
      Serial.print(dig_count);
      Serial.print(" ");
      Serial.print(digits[dig_count]);
      Serial.print("\n");
      ++dig_count;
      //++charCount;
    }
    else if (data == 54) {
      digits[dig_count] = 6;
      Serial.print(dig_count);
      Serial.print(" ");
      Serial.print(digits[dig_count]);
      Serial.print("\n");
      ++dig_count;
      //++charCount;
    }
    else if (data == 55) {
      digits[dig_count] = 7;
      Serial.print(dig_count);
      Serial.print(" ");
      Serial.print(digits[dig_count]);
      Serial.print("\n");
      ++dig_count;
      //++charCount;
    }
    else if (data == 56) {
      digits[dig_count] = 8;
      Serial.print(dig_count);
      Serial.print(" ");
      Serial.print(digits[dig_count]);
      Serial.print("\n");
      ++dig_count;
      //++charCount;
    }
    else if (data == 57) {
      digits[dig_count] = 9;
      Serial.print(dig_count);
      Serial.print(" ");
      Serial.print(digits[dig_count]);
      Serial.print("\n");
      ++dig_count;
      //++charCount;
    }
    //else if (charCount >= dataS.length()) {
    else if (data == 10) {    //newline char detected - indicates end of coordinates sent by color detection program to Serial
      //charCount = 0;
      int i = 0;
      int output_ind = 0;

      while (i < dig_count) {
        if (i != dig_count - 1) {
          int next = digits[i + 1];
          
          if (next != 20) {
            if (digits[i] == (-1)) {
              output[output_ind] = (-1);
              ++output_ind;
              i += 3;
            }
            else {
              output[output_ind] = (10 * digits[i]) + next;
              ++output_ind;
              i += 3;
            }
          }
          else {
            output[output_ind] = digits[i];
            ++output_ind;
            i += 2;
          }
        }
        else if (digits[i] == 20) {
          ++i;
        }
        else {
          output[output_ind] = digits[i];
          ++output_ind;
          i += 2;
        }
        
      }
      for (int j = 0; j < 8; ++j) {
        Serial.print(output[j]);
        Serial.print(" ");
      }
     
      Serial.print("\n");
      dig_count = 0;
      break;
    }
  }
  
  for (int k = 0; k < 8; ++k) {     //print coordinate values to Serial Monitor - mainly for testing purposes
      Serial.print(output[k]); 
      Serial.print("-");   
  }
    
  /*radio.startListening();             //Previously read from LCD here. Moved this part earlier in loop() to only get coordinates when needed
  if (radio.available(&pipenum)) {
    radio.read(&message, sizeof(message));
    if (message[1] == 455) {
      color = message[0];
    }
  }*/

  if (color == 10) { //red object selected
    if (output[0] != -1 && output[1] != -1) {   //x: output[0] & y: output[1]
      coords[0] = output[0];
      coords[2] = output[1];
    }
    else {  //no red detected
      coords[0] = -1;
      coords[2] = -1;
    }
    readyReturn = 1;      //flag to indicate coordinates are ready to be sent to motor drivers
    color = 0;
  }
  else if (color == 15) { //orange object selected
    if (output[4] != -1 && output[5] != -1) {   //x: output[4] & y: output[5]
      coords[0] = output[4];
      coords[2] = output[5];
    }
    else {  //no orange detected
      coords[0] = -1;
      coords[2] = -1;
    }
    readyReturn = 1;
    color = 0;
  }
  else if (color == 20) { //blue object selected
    
    if (output[2] != -1 && output[3] != -1) {  //x: output[2] & y: output[3]
      coords[0] = output[2];
      coords[2] = output[3];
    }
    else {  //no blue detected
      coords[0] = -1;
      coords[2] = -1;
    }
    readyReturn = 1;
    color = 0;
  }
  else if (color == 25) { //purple object selected
    
    if (output[6] != -1 && output[7] != -1) {   //x: output[6] & y: output[7]
      coords[0] = output[6];
      coords[2] = output[7];
    }
    else {  //no purple detected
      coords[0] = -1;
      coords[2] = -1;
    }
    readyReturn = 1;    
    color = 0;
  }
  else {  //no color selected
    coords[0] = -1;
    coords[2] = -1;
    readyReturn = 0;  //do not return coordinates
  }

  if (readyReturn != 0) {     //coordinates ready
    radio.stopListening();
    coords[1] = 789;  // DO NOT USE 123 OR 455 - ID for motor module to know message is from camera module
    radio.write(&coords, sizeof(coords));   //send ID and coordinates to motor module
    readyReturn = 0;
  }

}
