#define MY_ADDRESS 1

// nrf24_ping_client.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messageing client
// with the NRF24 class. 
// It is designed to work with the example nrf24_ping_server
// It also works with ping_server from the Mirf library
#include <stdio>
#include <SPI.h>
#include <Wire.h>
#include <max7219.h>
#include <characters.h>
#include "WireSram.h"
#include "nRF24L01.h"
#include "RF24.h"

#include "Packet.h"

#define MAX_ADDRESSES 10
                       
// Singleton instance of the radio
RF24 radio(8, 7); // use this to be electrically compatible with Mirf
MAX7219 mymatrix;

char* ackPayload = "Sam";
//uint8_t ackBuffer[PACKET_SIZE] = {0};
boolean sending = false;
boolean receiving = false;

int numOfDevicesFound = 0;
int matrixToggle = 0;

void setup() 
{    
  Serial.begin(57600);
  //fdevopen( &my_putc, 0);
  //Initialize LED matrix
  mymatrix.setup(A0,A1,A2);//din,cs,clock
  mymatrix.pic(blank);//prints 8x8 array from characters.h can be usyed to storte pics
  mymatrix.pic(smiley);

  //Initialize NRF
  radio.begin();
  radio.setRetries(0,2);
  radio.setPayloadSize(PACKET_SIZE);
  radio.setChannel(100);
  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate(RF24_250KBPS);
  radio.enableDynamicPayloads();
  //radio.enableAckPayload();

  radio.openReadingPipe(1, MY_ADDRESS);
  radio.startListening();


  Serial.print("initialized. My address: "); 
  Serial.println(MY_ADDRESS);
  delay(100);
  radio.printDetails();
  //attachInterrupt(0, receive, LOW); 
}

void loop()
{
    doReceive();
    delay(random(0, 15));
    startSend();
}

void doReceive(){
    radio.openReadingPipe(1, MY_ADDRESS);
    if (waitDataAvailable(5))
    {
        Packet receivePacket;
        //detachInterrupt(0);
        receivePacket = receive();
        Serial.print("Recieved: ");
        printPacket(&receivePacket);  
    //attachInterrupt(0, receive, LOW);
    }
}

void startSend() {
  Packet recievedAck;
  sending = true;
  radio.stopListening();
  for (short int sendAddress=0; sendAddress <= MAX_ADDRESSES; sendAddress++) {
    if(sendAddress == MY_ADDRESS) 
      continue;
	Packet sendPacket('P', MY_ADDRESS, sendAddress, (uint8_t*)"NAM MYOHO RENGE KYO!");
    sendPacketToAddress(sendPacket);
    radio.openReadingPipe(1, MY_ADDRESS);
    radio.startListening();
    if(waitDataAvailable(5)) {
        recievedAck = receive();
    }
    radio.stopListening();
    if (&recievedAck != NULL && recievedAck.getPacketType() == 'A') {
        Serial.print("ACK: ");
        printPacket(&recievedAck);
        addDeviceFound(recievedAck.getFromAddress());
    }
  }
  radio.startListening();
  sending = false;
}

void sendPacketToAddress(Packet sendPacket) {
  Packet getAckPacket;
  uint8_t buffer[PACKET_SIZE] = {0};
  sendPacket.getPacket(buffer);
  radio.openWritingPipe(sendPacket.getToAddress());
  radio.write(buffer, PACKET_SIZE); 
}

Packet receive() {
    char* typeOfPacket ;
    uint8_t buffer[PACKET_SIZE] = {0};
    bool answer = false;
       
    answer = radio.read(buffer, sizeof(buffer));
    Packet recievedPacket(buffer);
    if(recievedPacket.getPacketType() != 'A') {
        radio.stopListening();
        Packet ackPacket('A', MY_ADDRESS, recievedPacket.getFromAddress(), (uint8_t*)"Sam!");
        sendPacketToAddress(ackPacket);
        radio.startListening();
    }
    return recievedPacket;
}

bool waitDataAvailable(int timeout) {
  unsigned long started_waiting_at = millis();
  bool timeoutFlag = false;
  while ( !radio.available()  && ! timeoutFlag ) {
    if (millis() - started_waiting_at > timeout )
      timeoutFlag = true;
  }
  return !timeoutFlag;
}

void addDeviceFound(int address) {
    Serial.print("Found device: ");
    Serial.println(address); 
    if(matrixToggle == 0) {
        mymatrix.pic(wave);
        matrixToggle = 1;
    }
    else {
        mymatrix.pic(smiley);
        matrixToggle = 0;
    }   
}

void printPacket(Packet* recievePacket) {
  Serial.print("To Address: ");
  Serial.print((*recievePacket).getToAddress());
  Serial.print(". From Address: ");
  Serial.print((*recievePacket).getFromAddress());
  Serial.print(". Data: ");
  uint8_t payload[PAYLOAD_SIZE];
	(*recievePacket).getPayload(payload);	
  for(int i=0; i < PAYLOAD_SIZE; i++) {
    if(payload[i] == 0 && i !=0)
      break;
    Serial.print(char(payload[i]));  
  }
  Serial.println(".");
}

void logBuffer(char* print, uint8_t* buffer) {
    Serial.print(print);
    Serial.print("Packet: ");
    for (int i = 0; i < PACKET_SIZE; i++) {
        if (i > 4)
            Serial.print(char(*(buffer + i)));
        else 
        {
            Serial.print(*(buffer + i));
            Serial.print(", ");
        }
    }
    Serial.println();
}

int my_putc( char c, FILE *t) {
  Serial.write( c );
}

