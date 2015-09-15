#ifndef  _STR_H_
#define  _STR_H_

#ifdef __cplusplus
extern "C"{
#endif

int htoi(const char *s);

char* inet_n2a(unsigned int ipAddr);
int str_ConvertToUnicod(const char *pSrc, char * pUnicode, int nMaxSize);
int memcpyEx(void* dst, const void* src, int len);
void str_ConvertToHexStr(uint8* pChar, int len);
int str_i2Asiic(const uint8* pSrc, int nSrcLen, char* pDst, int mDstLen);
int str_GetFromStream(const uint8* pData, int len, char* pStrMask);

wchar* wstr_find(const wchar* pSrc, int nSrcLen, const wchar* pSub, int nDstLen);

#ifdef CONFIG_STRING
const char* strchr(const char* s, char c);
#endif

#ifdef __cplusplus
}
#endif

#endif 


