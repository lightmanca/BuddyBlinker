//Radio packet definition for Buddy Blinker

#ifndef _dataPacketHANDLER_H__
#define _dataPacketHANDLER_H__


#include <SPI.h>
#include "nRF24L01.h"

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
	void cpyBuffers(uint8_t* from, uint8_t* to, int len, bool nullOutRestOfBuffer);
    
  public:
    PacketContainer dataPacket;

    Packet(char packetType, byte fromAddress, byte toAddress, uint8_t* payload);
    Packet(uint8_t* buffer);
    Packet();
    int packetSize();
    void refresh(char packetType, byte fromAddress, byte toAddress, uint8_t* payload);
    void refresh(uint8_t* buffer);
    void refresh();

    char getPacketType();
    byte getFromAddress();
    byte getToAddress();

    void getPayload(uint8_t* buffer);
    void getPacket(uint8_t* buffer);
};
  
#endif //  _dataPacketHANDLER_H__

