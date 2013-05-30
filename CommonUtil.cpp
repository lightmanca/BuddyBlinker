#include "CommonUtils.h"

void cpyBuffers(uint8_t* from, uint8_t* to, int len, bool nullOutRestOfBuffer) {
	bool nullRestOfPayload = false;
	for(int i= 0; i < len; i++) {
		if(*(from +i) == 0x0 && nullOutRestOfBuffer) 
			nullRestOfPayload = true;
		*(to +i) = nullRestOfPayload ? 0x0 : *(from + i);
	}
}