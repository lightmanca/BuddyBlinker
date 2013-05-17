/*
#define PAYLOAD_SIZE 27

struct PacketContainer {
  char packetType; 
  uint16_t fromAddress;
  uint16_t toAddress;
  uint8_t payload[27];
};

class Packet
{
  private: 
    PacketContainer dataPacket;
    
    void makePacket(char packetType, uint16_t fromAddress, uint16_t toAddress, uint8_t* buffer);
	void cpyBuffers(uint8_t* from, uint8_t* to);
    
  public:
    Packet(char packetType, uint16_t fromAddress, uint16_t toAddress, uint8_t* buffer);
    Packet();
    PacketContainer* dataPointer();
    int packetSize();
    void refresh(char packetType, uint16_t fromAddress, uint16_t toAddress, uint8_t* buffer);
    void refresh();

    char getPacketType();
    uint16_t getFromAddress();
    uint16_t getToAddress();
    void getPayload(uint8_t* buffer);
};
  
*/

#include "Packet.h"

uint8_t emptyPayload[PAYLOAD_SIZE] = {0};

Packet::Packet() {
  makePacket(' ', 0, 0, emptyPayload);
}

Packet::Packet(uint8_t* buffer) {
  cpyBuffers(buffer, (uint8_t*)&dataPacket, PAYLOAD_SIZE, false);
}

Packet::Packet(char packetType, byte fromAddress, byte toAddress, uint8_t* payload) {
    makePacket(packetType, fromAddress, toAddress, payload);
}

int Packet::packetSize() {
  return sizeof(dataPacket);
}

void Packet::refresh(char packetType, byte fromAddress, byte toAddress, uint8_t* payload) {
    makePacket(' ', 0, 0, emptyPayload);  
    makePacket(packetType, fromAddress, toAddress, payload);
}

void Packet::refresh(uint8_t* buffer) {
    makePacket(' ', 0, 0, emptyPayload);  
    cpyBuffers(buffer, (uint8_t*)&dataPacket, PAYLOAD_SIZE, false);
}

void Packet::refresh() {
  makePacket(' ', 0, 0, emptyPayload);
}

char Packet::getPacketType() {
  return dataPacket.packetType;
}

byte Packet::getFromAddress() {
  return dataPacket.fromAddress;
}

byte Packet::getToAddress() {
  return dataPacket.toAddress;
}

void Packet::getPayload(uint8_t* buffer) {
   cpyBuffers(dataPacket.payload, buffer, PAYLOAD_SIZE, true);
}

//private
void Packet::makePacket(char packetType, byte fromAddress, byte toAddress, uint8_t* buffer) {
  dataPacket.packetType = packetType;
  dataPacket.fromAddress = fromAddress;
  dataPacket.toAddress = toAddress;
  cpyBuffers(buffer, dataPacket.payload, PAYLOAD_SIZE, true);
}

void Packet::getPacket(uint8_t* buffer) {
    cpyBuffers((uint8_t*)&dataPacket, buffer, PAYLOAD_SIZE, false);
}

//private
void Packet::cpyBuffers(uint8_t* from, uint8_t* to, int len, bool nullOutRestOfBuffer) {
	bool nullRestOfPayload = false;
	for(int i= 0; i < len; i++) {
		if(*(from +i) == 0x0 && nullOutRestOfBuffer) 
			nullRestOfPayload = true;
		*(to +i) = nullRestOfPayload ? 0x0 : *(from + i);
	}
}

