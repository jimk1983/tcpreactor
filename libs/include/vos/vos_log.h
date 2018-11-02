/******************************************************************************

                  版权所有 (C), 2015-2025, 网络科技有限公司

 ******************************************************************************
  文 件 名   : dbg_log.h
  版 本 号   : 初稿
  作    者   : jimk
  生成日期   : 2016年2月14日
  最近修改   :
  功能描述   : Debug
  函数列表   :
  修改历史   :
  1.日    期   : 2016年2月14日
    作    者   : jimk
    修改内容   : 创建文件

******************************************************************************/
    
#define LOG_LEVEL_ERROR             0x01
#define LOG_LEVEL_WARNING         0x02
#define LOG_LEVEL_EVENT              0x04
#define LOG_LEVEL_DEBUG             0x08
#define LOG_LEVEL_ALL                  0x0F         /*0x0F--全开，0x00:--全关闭*/


VOID VOS_PrintDebug(const CHAR *pcformat, ...);

VOID VOS_DebugTraceOutput(const CHAR *pcformat, ...);

VOID VOS_DebugBufHexOutput(CHAR *pcHeadDescript, CHAR *pcBuf, LONG ulLen);

VOID VOS_WriteSystemLog(const CHAR *pcformat,...);

VOID VOS_WriteRctLog(const CHAR *pcformat,...);

LONG VOS_Log_Init();

VOID VOS_Log_UnInit();

#define DEBUG_PRINT_FORMAT "[FILE=%s,FUNC=%s,LINE=%d]--->"

#define DbgTrace(pcDbgInfo, ...)\
{\
  if( VOS_TRUE )\
  {\
    (VOID)VOS_DebugTraceOutput((DEBUG_PRINT_FORMAT pcDbgInfo),__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__);\
  }\
}


