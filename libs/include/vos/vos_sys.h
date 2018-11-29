/******************************************************************************

                  版权所有 (C), 2015-2025, 网络科技有限公司

 ******************************************************************************
  文 件 名   : vos_sys.h
  版 本 号   : 初稿
  作    者   : jimk
  生成日期   : 2016年5月23日
  最近修改   :
  功能描述   : 系统相关
  函数列表   :
  修改历史   :
  1.日    期   : 2016年5月23日
    作    者   : jimk
    修改内容   : 创建文件

******************************************************************************/


#if VOS_PLAT_LINUX
typedef pthread_t VOS_THREAD_T;
#elif VOS_PLAT_WIN
#ifndef HANDLE
#define HANDLE (VOID *)
#endif
typedef HANDLE  VOS_THREAD_T;
#endif

VOS_THREAD_T VOS_Thread_Create(CHAR *pcName, VOID *(* pfvThread)(VOID *arg), VOID *pvArg, INT32 iStacksize, INT32 iPrio);

LONG VOS_GetLastError();

CHAR *VOS_GetStrLastError();


