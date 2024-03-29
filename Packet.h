//Radio packet definition for Buddy Blinker

#ifndef _dataPacketHANDLER_H__
#define _dataPacketHANDLER_H__

#include <RF24.h>

#define PACKET_SIZE 32
#define PAYLOAD_SIZE 29

struct PacketContainer {
  char packetType; 
  byte fromAddress;
  byte toAddress;
  uint8_t payload[PAYLOAD_SIZE];
} ;

class Packet
{
  private: 
    void makePacket(char packetType, byte fromAddress, byte toAddress, uint8_t* buffer);
    PacketContainer _dataPacket;

  public:
    Packet(char packetType, byte fromAddress, byte toAddress, uint8_t* payload);
    Packet();
    uint8_t* getPointer();
    uint8_t* getPayloadPointer();

    char getPacketType();
    byte getFromAddress();
    byte getToAddress();
};
  
#endif //  _dataPacketHANDLER_H__

