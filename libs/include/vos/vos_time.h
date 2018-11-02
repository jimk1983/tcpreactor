/******************************************************************************

                  版权所有 (C), 2015-2025, 网络科技有限公司

 ******************************************************************************
  文 件 名   : vos_time.h
  版 本 号   : 初稿
  作    者   : jiangkang
  生成日期   : 2015年9月25日
  最近修改   :
  功能描述   : 系统时间的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2015年9月25日
    作    者   : jiangkang
    修改内容   : 创建文件

******************************************************************************/


#ifndef _VOS_TIME_H_
#define _VOS_TIME_H_

typedef struct tagAbsTime
{
    struct timespec stAbsTime;
}VOS_ABS_TIME_S;

#define VOS_LOCAL_SYSTIME_BUFLEN        28

typedef struct tagLocalSysTime
{
    UINT32  uiYear;
    UINT32  uiMoth;
    UINT32  uiDay;
    UINT32  uiHour;
    UINT32  uiMinute;
    UINT32  uiSecond;
    UINT32  uiMiliSec;    
}VOS_LOCAL_SYSTEM_TIME_S;



VOID VOS_GetAbsTime(VOS_ABS_TIME_S *pstAbsTime, UINT32 ulTimeMiliSec);
VOID VOS_Sleep(ULONG ulSeconds);
VOID VOS_MSleep(ULONG ulMSleep);
VOID VOS_USleep(ULONG ulUSleep);

VOID VOS_GetLocalSystemTime(VOS_LOCAL_SYSTEM_TIME_S *pstSysTime);


#endif

