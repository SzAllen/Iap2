#ifndef __RET_CODE_H_
#define __RET_CODE_H_

#ifdef __cplusplus
extern "C"{
#endif

typedef enum _RetCode
{
	 RC_SUCCESS = 0
	,RC_FAILED = 1
	,RC_PENDING = 2
}RetCode;

#ifdef __cplusplus
}
#endif

#endif


