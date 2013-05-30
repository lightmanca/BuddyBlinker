#ifndef _CommonUtils_H__
#define _CommonUtils_H__

#include <RF24.h>

void cpyBuffers(uint8_t* from, uint8_t* to, int len, bool nullOutRestOfBuffer);

#endif
