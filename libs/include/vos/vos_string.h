/******************************************************************************

                  版权所有 (C), 2015-2025, 网络科技有限公司

 ******************************************************************************
  文 件 名   : vos_string.h
  版 本 号   : 初稿
  作    者   : jimk
  生成日期   : 2016年1月2日
  最近修改   :
  功能描述   : String头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2016年1月2日
    作    者   : jimk
    修改内容   : 创建文件

******************************************************************************/
#ifndef _VOS_STRING_H_
#define _VOS_STRING_H_

/*定义最长的字符串安全操作长度*/
#define VOS_MAXSTRLEN       16384

VOID VOS_Printf(CHAR *pcformat,...);

INT32 VOS_Snprintf(CHAR *pcbuf, UINT32 ulMaxLen, CHAR *pcformat,...);

INT32 VOS_StrCmp(CHAR *pcString1,CHAR *pcString2);

INT32 VOS_StrLen(CHAR *pcString);

CHAR *VOS_StrStr(const CHAR *pcStr1, const CHAR *pcStr2);

CHAR *VOS_StrCpy(CHAR *pcString1,CHAR *pcString2);

CHAR *VOS_StrCpy_S(CHAR *pcDst, ULONG ulMaxLen, CHAR *pcSrc);

CHAR *VOS_StrNCpy_S(CHAR *pcDst, ULONG ulMaxLen,CHAR *pcSrc, ULONG ulLen);

CHAR *VOS_Strrchr(const CHAR *pcString, const CHAR chEndChr);

CHAR *VOS_Strchr(const CHAR *pcString, const CHAR chStartChr);


#endif

