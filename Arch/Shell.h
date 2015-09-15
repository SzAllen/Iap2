#if CONFIG_CMDLINE

#ifndef  _SHELL_H_
#define  _SHELL_H_

#ifdef __cplusplus
extern "C"{
#endif

void Shell_Init(void);
void Shell_Input(const char* pStr, int len);
void Shell_GetArgCount();
void Shell_String(void* pNull, const uint8* pStr, int len);
void Shell_Run();

#ifdef __cplusplus
}
#endif

#endif 

#else
#define Shell_Init(...)
#define Shell_String 0
#endif
