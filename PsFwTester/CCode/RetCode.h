#ifndef __RET_CODE_H_
#define __RET_CODE_H_

#ifdef __cplusplus
extern "C"{
#endif

typedef enum _RetCode
{
	 RC_SUCCESS 		= 0
	,RC_FAILED 			= 1
	,RC_PENDING 		= 2
	,RC_BAD_PARAMETER 	= 3
	,RC_UNKNOWN 		= 4
	,RC_DONOTHING 		= 5
	,RC_ABORTED 		= 6
	,RC_WRONG_PHASE 	= 7
	,RC_NO_RESOURCE 	= 8
	,RC_BUSY 			= 9
	,RC_DONE 			= 10
}RetCode;

#define RC_CRC_ERROR                    11
#define RC_LBA_OUT_OF_RANGE             12
#define RC_INVALID_FIELD                13
#define RC_INVALID_LUN                  14
#define RC_OUT_OF_RANGE                 15
#define RC_READ_ONLY                    16
#define RC_TASK_FOUND                   17
#define RC_BAD_COMMAND                  18
#define RC_HARDWARE                     19
#define RC_BUSY                         20
#define RC_TIMEOUT          			21
#define RC_INVALID_PORT_NUM             22
#define RC_INVALID_ADDRESS             	23
#define RC_NO_DEVICE             		24
#define RC_DEVICE_NOT_READY             25
#define RC_DEVICE_DETECT_ERROR          26
#define RC_TFD_ERROR          			27
#define RC_NULL_POINTER          		28
#define RC_NOT_READY            		29
#define RC_UNIT_ATTENTION_MEDIA_CHANGE	30
#define RC_DATA_PROTECT					31
#define RC_UNIT_ATTENTION_HARDWARE      32
#define RC_BAD_LENGTH                   33
#define RC_NO_MEDIA                     34
#define RC_LUN_NOT_READY                35
#define RC_MEDIA_REMOVAL_PREVENTED      36
#define RC_SELF_TEST_IN_PROGRESS        37
#define RC_INTERNAL_TARGET_FAILURE      38
#define RC_DIAGNOSTIC_FAILURE           39
#define RC_EP_NOT_READY                 40
#define RC_INCORRECT_SECURITY_STATE     41     // ASC=0x74, ASCQ=0x81
#define RC_AUTHENTICATION_FAILED        42     // ASC=0x74, ASCQ=0x40
#define RC_TOO_MANY_AUTHENT_FAILURES    43     // ASC=0x74, ASCQ=0x80
#define RC_SECURITY_ERROR               44     // ASC=0x74, ASCQ=0x00
#define RC_EXISTED                      45
#if READ_NO_WAIT_FOR_USB == 1
#define RC_COMPLETE                     46
#endif
#define RC_UNSUPPORT_ENCLOSURE_FUN      47
#define RC_POWERING_UP                  48

#define RC_WRITE_PROTECTED                     49
#define RC_ATA_PASS_THROUGH_SUCCESS                     50
#define RC_ROUNDED_PARAMETER            51
#define RC_LENGTH_MISMATCH              52
#define RC_DEVICE_PERMANENT             53
#define RC_UNSUPPORTED_FLASH            54     // HARDWARE ERROR (0x4),ASC=0x44 INTERNAL TARGET FAILURE (0x44) , ASCQ=0x8B (0x80 + 0x0B)
#define RC_TIME_OUT			            55     

#define INVALID_SCSI_OPCODE                     0xff

#ifdef __cplusplus
}
#endif

#endif


