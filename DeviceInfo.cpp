

/*
#define DEVICE_INFO_SIZE 29
#define DEVICE_NAME_SIZE 10

struct DeviceInfoContainer {
  byte address;
  char name[DEVICE_NAME_SIZE];
  bool hasGps;
  
  float latitude;
  float longitude;
  float distance;
  unsigned long pingTime;
} ;

class DeviceInfo
{
  private: 
    void makeDeviceInfo(byte address, char* name, bool hasGps, float latitude, float longitude, float distance);
    DeviceInfoContainer _deviceInfo;

  public:
    DeviceInfo(byte address, char* name, bool hasGps, float latitude, float longitude, float distance);
    DeviceInfo(uint8_t* buffer); 
    DeviceInfo();
    int packetSize();

    byte* getPointer();
    char* getNamePointer();

    byte getDeviceAddress();
    void getName(char* buffer);
    bool getHasGPS();

    float getLatitude();
    float getLongitude();
    float getDistance();
    float getPingTime();

    void setLatitude(float latitude);
    void setLongitude(float longitude);
    void setDistance(float distance);
    void setPingTime(unsigned long timeOfLastPing);
} ;
*/

#include <WireSram.h>
#include "CommonUtils.h"
#include "DeviceInfo.h"

char emptyName[DEVICE_NAME_SIZE] = {0};

DeviceInfo::DeviceInfo(byte address, char* name, bool hasGps, float latitude, float longitude, float distance, unsigned long lastPingTime) {
    makeDeviceInfo(address, name, hasGps, latitude, longitude, distance, lastPingTime);
}

DeviceInfo::DeviceInfo(uint8_t* buffer) {
    makeDeviceInfo(0, emptyName, false, 0.0, 0.0, 0.0, 0);
    cpyBuffers(buffer, (uint8_t*)&_deviceInfo, DEVICE_INFO_SIZE, false);
}

DeviceInfo::DeviceInfo() {
    makeDeviceInfo(0, emptyName, false, 0.0, 0.0, 0.0, 0);
}

void DeviceInfo::refresh(byte address, char* name, bool hasGps, float latitude, float longitude, float distance, unsigned long lastPingTime) {
    makeDeviceInfo(0, emptyName, false, 0.0, 0.0, 0.0, 0);
    makeDeviceInfo(address, name, hasGps, latitude, longitude, distance, lastPingTime);
}

byte* DeviceInfo::getPointer() {
    return (byte*)&_deviceInfo;
}

char* DeviceInfo::getNamePointer() {
    return _deviceInfo.name;
}

//Saves to Sram.  Assumes address calculated form Address
void DeviceInfo::saveToSram(WireSram sramDevice) {
    sramDevice.writeBytes(_deviceInfo.address * DEVICE_INFO_SIZE, (byte*)&_deviceInfo, DEVICE_INFO_SIZE);
    //Serial.print("Save to srame: ");
    //printDevice();
}

void DeviceInfo::readFromSram(byte address, WireSram sramDevice) {
    sramDevice.readBytes(address * DEVICE_INFO_SIZE, (byte*)&_deviceInfo, DEVICE_INFO_SIZE);
    //Serial.print("Read from srame: ");
    //printDevice();
}

//Getters
char DeviceInfo::getPreamble() {
    return _deviceInfo.preamble;
}

byte DeviceInfo::getDeviceAddress() {
    return _deviceInfo.address;
}

void DeviceInfo::getName(char* buffer) {
    return cpyBuffers((uint8_t*) _deviceInfo.name, (uint8_t*)  buffer, DEVICE_NAME_SIZE, true);
}

bool DeviceInfo::getHasGPS() {
    return _deviceInfo.hasGps;
}


float DeviceInfo::getLatitude() {
    return _deviceInfo.latitude;
}

float DeviceInfo::getLongitude() {
    return _deviceInfo.longitude;
}

float DeviceInfo::getDistance() {
    return _deviceInfo.distance;
}

unsigned long DeviceInfo::getPingTime() {
    return _deviceInfo.pingTime;
}

//Setters
void DeviceInfo::setLatitude(float latitude) {
     _deviceInfo.latitude = latitude;
}

void DeviceInfo::setLongitude(float longitude) {
    _deviceInfo.longitude = longitude;
}

void DeviceInfo::setDistance(float distance) {
    _deviceInfo.distance = distance;
}

void DeviceInfo::setPingTime(unsigned long timeOfLastPing) {
    _deviceInfo.pingTime = timeOfLastPing;
}


//*** private.
void DeviceInfo::makeDeviceInfo(byte address, char* name, bool hasGps, float latitude, float longitude, float distance, unsigned long lastPingTime) {
    _deviceInfo.preamble = '*';
    _deviceInfo.address = address;
    cpyBuffers((uint8_t*)name, (uint8_t*)_deviceInfo.name, DEVICE_NAME_SIZE, true);
    _deviceInfo.hasGps = hasGps;
    _deviceInfo.latitude = latitude;
    _deviceInfo.longitude = longitude;
    _deviceInfo.distance = distance;
    _deviceInfo.pingTime = lastPingTime;
}
