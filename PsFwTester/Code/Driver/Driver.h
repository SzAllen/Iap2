
#ifndef __DRIVER__H_
#define __DRIVER__H_

#ifdef __cplusplus
extern "C"{
#endif

#include "typedef.h"
#include "Iap2Link.h"

Bool Driver_TransferStart(Irb* pIrb);
void Driver_RxDone(const void* pData, int len);
void Driver_Check();
void Driver_TxDone();


#ifdef __cplusplus
}
#endif

#endif