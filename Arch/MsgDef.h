#ifndef __MSG_DEF_H_
#define __MSG_DEF_H_

#ifdef __cplusplus
extern "C"{
#endif

typedef enum _MSG_ID
{
	 MSG_TIMEOUT = 1
	 	
	,MSG_USB_PULG_IN
	,MSG_USB_PULG_OUT
	
	,MSG_WAKEUP
	,MSG_STENDBY
	,MSG_IDEL
	,MSG_ERROR
	
	,MSG_TIMEOUT_STENDBY
	,MSG_TIMEOUT_IDLE
	
	,MSG_PSW_VERIFY_OK
	
	,MSG_SWITCH_ON
	,MSG_SWITCH_OFF
	
	,MSG_BUTTON_CLICKED
	
	,MSG_BLE_READY

	 
	,MSG_USB_BUS_RESET
	,MSG_USB_SUSPEND
	,MSG_USB_RESUME
	,MSG_USB_SETADDRESS
	,MSG_USB_SETCONFIG
	,MSG_USB_VBUS_TRANSITION
	,MSG_USB_SOF
	,MSG_USB_TRANS_DONE
	,MSG_USB_SET_INTEFACE
	,MSG_USB_DISCONNECT

	,MSG_USB_HOST_ENABLE
	,MSG_USB_HOST_UNINSTALL
	,MSG_USB_HOST_ACEIVE_AOA
	 
	,MSG_USB_CHANGED
	,MSG_VBUS_CHANGED
	,MSG_BLE_DATA_CHANGED
	
	,MSG_IAP_ERROR
	,MSG_BLE_OTA_TEST
}MSG_ID;

#ifdef __cplusplus
}
#endif

#endif

