/******************************************************************************

                  版权所有 (C), 2015-2025, 网络科技有限公司

 ******************************************************************************
  文 件 名   : vos_malloc.h
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
#ifndef _VOS_MALLOC_H_
#define _VOS_MALLOC_H_


/*得到指定地址的字节和字*/
/***小端系统**，例如:赋值a=0x11223344, 其实11是最高位，内存地址低到高依次是0x44,0x33,0x22,0x11
 ***大端系统**，一样的0x11223344, 0x11最高位，内存低到高的查看是，0x11, 0x22, 0x33,0x44,
 也就是说， 大端存放时，将高位放在第一个，这样方便CPU得出符号位，比小端较为快速*/

/*得到最低字节位的值*/
#define MEM_GET_BYTE(x)     (*((BYTE *)(x)))
#define MEM_GET_INT32(x)   (*((INT32 *)(x)))

/*申请内存*/
CHAR *VOS_Memory(ULONG ulSize);

/*内存清零*/
VOID VOS_Mem_Zero(VOID *pcBuf, ULONG ulSize);

/*内存安全释放*/
VOID VOS_MemFree(CHAR *ucPtr);

/*内存拷贝*/
VOID VOS_Mem_Copy(CHAR *pcDst, CHAR *pcSrc,ULONG ulSize);

/*内存安全拷贝*/
VOID VOS_Mem_Copy_S(VOID *pcDst, ULONG ulMaxLen, VOID *pcSrc,ULONG ulSize);

/*内存比较*/
LONG VOS_MemCmp(CHAR *pcMem1, CHAR *pcMem2, UINT32 uiSize);


#endif
