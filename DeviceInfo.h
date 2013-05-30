//Radio packet definition for Buddy Blinker

#ifndef deviceInfoContainer
#define deviceInfoContainer

#include <RF24.h>
#include <WireSram.h>

#define DEVICE_INFO_SIZE 29
#define DEVICE_NAME_SIZE 10

struct DeviceInfoContainer {
    char preamble;  //Valid packet will contain '*'. Used to determine if packet is in memory.
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
    void makeDeviceInfo(byte address, char* name, bool hasGps, float latitude, float longitude, float distance, unsigned long lastPingTime);
    DeviceInfoContainer _deviceInfo;

  public:
    DeviceInfo(byte address, char* name, bool hasGps, float latitude, float longitude, float distance, unsigned long lastPingTime);
    DeviceInfo(uint8_t* buffer);
    DeviceInfo();
    void refresh(byte address, char* name, bool hasGps, float latitude, float longitude, float distance, unsigned long lastPingTime);

    byte* getPointer();
    char* getNamePointer();

    void saveToSram(WireSram sramDevice);
    void readFromSram(byte address, WireSram sramDevice);
    //bool isPacketInSram(byte address, WireSram sramDevice);

    char getPreamble();
    byte getDeviceAddress();
    void getName(char* buffer);
    bool getHasGPS();

    float getLatitude();
    float getLongitude();
    float getDistance();
    unsigned long getPingTime();

    void setLatitude(float latitude);
    void setLongitude(float longitude);
    void setDistance(float distance);
    void setPingTime(unsigned long timeOfLastPing);

    void printDevice();
} ;

#endif