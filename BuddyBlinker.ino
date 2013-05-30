//Buddy finder
//Will find devices nearby that have the same code running.
//
//Author: Sam Weiss 
//

#include <SPI.h>
#include <Wire.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>
#include <Adafruit_GPS.h>
#include <max7219.h>
#include <characters.h>
#include "WireSram.h"
#include "RF24.h"

#include "Packet.h"
#include "DeviceInfo.h"

#define MAX_ADDRESSES 10
#define ADDRESS_PING_TIMEOUT 5000
#define HAS_GPS true

#define RADIO_CE_PIN 8
#define RADIO_CS_PIN 7
#define SRAM_SLEEP_PIN 6
#define LED_MATRIX_DIN A0
#define LED_MATRIX_CS A1
#define LED_MATRIX_CLK A2
#define GPSECHO  true
#define SRAM_I2C_ADDRESS 80

// Singleton instance of the radio
RF24 radio(RADIO_CE_PIN, RADIO_CS_PIN); // use this to be electrically compatible with Mirf
MAX7219 mymatrix;
WireSram sram(SRAM_I2C_ADDRESS, SRAM_SLEEP_PIN);
SoftwareSerial myGpsSerial(3, 2);
Adafruit_GPS GPS(&myGpsSerial);

byte myAddress;
char myName[DEVICE_NAME_SIZE] = {0};
boolean sending = false;
boolean receiving = false;
// this keeps track of whether we're using the interrupt
// for GPS.  Off by default!
boolean usingInterrupt = true;

int numOfDevicesFound = 0;
int matrixToggle = 0;

DeviceInfo myDeviceInfo;

void setup() 
{    
    Serial.begin(57600);
    //Initialize GPS
    GPS.begin(9600);
    GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
    GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); 

    //Initialize LED matrix
    mymatrix.setup(LED_MATRIX_DIN, LED_MATRIX_CS, LED_MATRIX_CLK);//din,cs,clock
    mymatrix.pic(blank);//prints 8x8 array from characters.h can be usyed to storte pics

    //Initialize Sram.
    sram.Erase(0, MAX_ADDRESSES * DEVICE_INFO_SIZE);

    //Get my name and address from user or eprom
    getNameAndAddress();
    myDeviceInfo.refresh(myAddress, myName, HAS_GPS, 0.0, 0.0, 0.0, 0); 

    //Initialize NRF
    radio.begin();
    radio.setRetries(0,2);
    radio.setChannel(100);
    radio.setPALevel(RF24_PA_HIGH);
    radio.setDataRate(RF24_250KBPS);
    radio.enableDynamicPayloads();

    radio.openReadingPipe(1, myAddress);
    radio.startListening();
    mymatrix.pic(arrow);
    Serial.println("initialized."); 
    useInterrupt(true);
}

void loop()
{    
    if (GPS.newNMEAreceived()) {
        GPS.parse(GPS.lastNMEA());   // this also sets the newNMEAreceived() flag to false
    }
    doReceive();
    delay(random(0, 15));
    startSend();
    updateNumberDevicesFound();
    //Serial.println(numOfDevicesFound);
    mymatrix.drawchar(char(numOfDevicesFound + 48));
}

void doReceive(){
    radio.openReadingPipe(1, myAddress);
    if (waitDataAvailable(5))
    {
        Packet receivePacket;
        receivePacket = receive(); 
    }
}

void startSend() {
  Packet recievedAck;
  sending = true;
  radio.stopListening();
  for (short int sendAddress=0; sendAddress <= MAX_ADDRESSES; sendAddress++) {
    if(sendAddress == myAddress) 
      continue;
	Packet sendPacket('P', myAddress, sendAddress, (uint8_t*)"NAM MYOHO RENGE KYO!");
    sendPacketToAddress(sendPacket);
    radio.openReadingPipe(1, myAddress);
    radio.startListening();
    if(waitDataAvailable(5)) {
        recievedAck = receive();
    }
    radio.stopListening();
    if (&recievedAck != NULL && recievedAck.getPacketType() == 'A') {
        DeviceInfo deviceFound(recievedAck.getPayloadPointer());
        addDeviceFound(recievedAck.getFromAddress(), deviceFound);
    }
  }
  radio.startListening();
  sending = false;
}

void sendPacketToAddress(Packet sendPacket) {
  Packet getAckPacket;
  radio.openWritingPipe(sendPacket.getToAddress());
  radio.write(sendPacket.getPointer(), PACKET_SIZE); 
}

Packet receive() {
    char* typeOfPacket ;
    bool answer = false;
    Packet recievedPacket;
    answer = radio.read(recievedPacket.getPointer(), PACKET_SIZE);
    if(recievedPacket.getPacketType() != 'A') {
        if(HAS_GPS && GPS.fix) {
            myDeviceInfo.setLatitude(GPS.latitude);
            myDeviceInfo.setLongitude(GPS.longitude);
        }
        radio.stopListening();
        Packet ackPacket('A', myAddress, recievedPacket.getFromAddress(), (uint8_t*)myDeviceInfo.getPointer());
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

void addDeviceFound(int address, DeviceInfo device) {
    Serial.print("Found device: ");
    Serial.println(address);
    device.setPingTime(millis());
    device.setDistance(GPS.distance_between(device.getLatitude(), device.getLongitude(), GPS.latitude, GPS.longitude));
    device.saveToSram(sram);
}

void getNameAndAddress() {
    //try to read our address from eeprom. If not there, ask user.
    if(EEPROM.read(0) == '*')
        myAddress = EEPROM.read(1);
    else {
        mymatrix.pic(frown);
        Serial.print("Address?");
        while(!Serial.available())
            ;
        myAddress = Serial.parseInt();
        EEPROM.write(0, '*');
        EEPROM.write(1, myAddress);
    }
    Serial.print("\nMy Address : ");
    Serial.println(myAddress);
    
    if(EEPROM.read(3) == '*') {
        for(int i = 0; i < DEVICE_NAME_SIZE; i++)
            myName[i] = EEPROM.read(4 + i);
    }
    else {
        mymatrix.pic(frown);
        Serial.print("My name?");
        while(!Serial.available())
            ;
        Serial.readBytesUntil('\n', myName, DEVICE_NAME_SIZE -1);
        EEPROM.write(3, '*');
        for(int i = 0; i < DEVICE_NAME_SIZE -1 ; i++)
            EEPROM.write(4 + i, myName[i]);
        EEPROM.write(DEVICE_NAME_SIZE -1, 0);
    }
    Serial.print("\nMy name: ");
    Serial.println(myName);
    mymatrix.pic(smiley);
}

void updateNumberDevicesFound() {
    numOfDevicesFound = 0;
    long timeOut = millis() < ADDRESS_PING_TIMEOUT ? ADDRESS_PING_TIMEOUT : millis() - ADDRESS_PING_TIMEOUT;
    for (int i = 1; i <= MAX_ADDRESSES; i++) {
        DeviceInfo deviceFound;        
        deviceFound.readFromSram(i, sram);
        if(deviceFound.getPreamble() == '*') {
            if(deviceFound.getPingTime() > timeOut)
                numOfDevicesFound++;
        }
    }
}

// Interrupt is called once a millisecond, looks for any new GPS data, and stores it
SIGNAL(TIMER0_COMPA_vect) {
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
#ifdef UDR0
  if (GPSECHO)
    if (c) UDR0 = c;  
    // writing direct to UDR0 is much much faster than Serial.print 
    // but only one character can be written at a time. 
#endif
}

void useInterrupt(boolean v) {
  if (v) {
    // Timer0 is already used for millis() - we'll just interrupt somewhere
    // in the middle and call the "Compare A" function above
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
    usingInterrupt = true;
  } else {
    // do not call the interrupt function COMPA anymore
    TIMSK0 &= ~_BV(OCIE0A);
    usingInterrupt = false;
  }
}


