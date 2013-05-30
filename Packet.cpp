/*
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
    PacketContainer _dataPacket;

  public:
    Packet(char packetType, byte fromAddress, byte toAddress, uint8_t* payload);
    Packet();
    int packetSize();
    void refresh(char packetType, byte fromAddress, byte toAddress, uint8_t* payload);
    void refresh();
    uint8_t* getPointer();
    uint8_t* getPayloadPointer();

    char getPacketType();
    byte getFromAddress();
    byte getToAddress();
};
  
*/

#include "CommonUtils.h"
#include "Packet.h"

uint8_t emptyPayload[PAYLOAD_SIZE] = {0};

Packet::Packet() {
  makePacket(' ', 0, 0, emptyPayload);
}

Packet::Packet(char packetType, byte fromAddress, byte toAddress, uint8_t* payload) {
    makePacket(packetType, fromAddress, toAddress, payload);
}

uint8_t* Packet::getPointer() {
    return (uint8_t*)&_dataPacket;
}

uint8_t* Packet::getPayloadPointer() {
    return (uint8_t*)&_dataPacket.payload;
}

char Packet::getPacketType() {
  return _dataPacket.packetType;
}

byte Packet::getFromAddress() {
  return _dataPacket.fromAddress;
}

byte Packet::getToAddress() {
  return _dataPacket.toAddress;
}

//private
void Packet::makePacket(char packetType, byte fromAddress, byte toAddress, uint8_t* buffer) {
  _dataPacket.packetType = packetType;
  _dataPacket.fromAddress = fromAddress;
  _dataPacket.toAddress = toAddress;
  cpyBuffers(buffer, _dataPacket.payload, PAYLOAD_SIZE, true);
}



