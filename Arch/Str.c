
#include "Arch.h"
#include "Str.h"

#define CONVERT_ASIIC(x) \
if(x <= 0x09) \
	x += 0x30; \
else \
	x = x + ('A' - 0x0a);

int htoi(const char *s)
{
	int n = 0;

	if( !s ) return 0;

	if( *s == '0' )
	{
		s++;
		if( *s == 'x' || *s == 'X' )s++;
	}

	while(*s != '\0')
	{
		n <<= 4;
		if( *s <= '9' )
			n |= ( *s & 0xf );
		else
			n |= ( (*s & 0xf) + 9 );
		s++;
	}
	return n;
}

char* inet_n2a(uint32 ipAddr)
{
	static char ipStr[16];
	
	memset(ipStr, 0, sizeof(ipStr));
	sprintf(ipStr, "%d.%d.%d.%d",  MEM_BYTE(ipAddr, 0), MEM_BYTE(ipAddr, 1), MEM_BYTE(ipAddr, 2), MEM_BYTE(ipAddr, 3));
	
	return ipStr;
}

int str_i2Asiic(const uint8* pSrc, int nSrcLen, char* pDst, int mDstLen)
{
	int i = 0; 
	int j = 0;

	ASRT(mDstLen >= nSrcLen * 2);

	for(i = 0; i < nSrcLen; i++)
	{
		pDst[j] = pSrc[i] >> 4;
		CONVERT_ASIIC(pDst[j]);
		j++;
		
		pDst[j] = pSrc[i] & 0x0f;
		CONVERT_ASIIC(pDst[j]);
		j++;
	}

	return j;
}

int str_add(char* src, uint32 srcBufLen, const char* dst)
{
	if(srcBufLen > strlen(src) + strlen(dst))
	{
		memcpy(&src[strlen(src)], dst, strlen(dst));
		return strlen(src) + strlen(dst);
	}
	else
	{
		return 0;
	}
}

int str_ConvertToUnicod(const char *pSrc, char * pUnicode, int nMaxSize)
{
    int idx = 0;
  
    while(*pSrc) 
    {
		pUnicode[idx++] = *pSrc++;
		pUnicode[idx++] = 0;
		
		if(idx >= nMaxSize-2)
		{
			break;
		}
    }

    pUnicode[idx++] = 0;
    pUnicode[idx++] = 0;
   
    return idx;
}

int memcpyEx(void* dst, const void* src, int len)
{
	memcpy(dst, src, len);
	return len;
}

//String convert to hex string
void str_ConvertToHexStr(uint8* pChar, int len)
{
	int i = 0;

	for(i = 0; i < len; i++, pChar++)
	{
		if(*pChar >= 'a' && *pChar <= 'f')	//convert Upper to lower
		{
			*pChar -= 0x20;
		}

		if((*pChar < '0') || ((*pChar > '9') && (*pChar < 'A')) || (*pChar > 'F') )
		{
			*pChar = '0';
		}
	}
}

int str_GetFromStream(const uint8* pData, int len, char* pStrMask)
{
	int i = 0; 
	static int nIndex = 0;

	for(i = 0; i < len; i++)
	{		
		if(pData[i] == pStrMask[nIndex] || ('?' == pStrMask[nIndex]))
		{
			if('?' == pStrMask[nIndex])
			{
				pStrMask[nIndex] = pData[i];
			}
			nIndex++;
		}
		else
		{
			nIndex = 0;
		}
		
		if(strlen(pStrMask) <= nIndex)
		{
			return True;
		}
	}

	return False;
}

#ifdef CONFIG_STRING

const char* strchr(const char* s, char c)
{	
	while(*s != '\0' && *s != c)
	{
		++s;
	}

	return *s == c ? s : 0;
}


#endif


