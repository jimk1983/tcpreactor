/******************************************************************************

                  版权所有 (C), 2015-2025, 网络科技有限公司

 ******************************************************************************
  文 件 名   : rct_dbg.c
  版 本 号   : 初稿
  作    者   : jimk
  生成日期   : 2016年3月6日
  最近修改   :
  功能描述   : RCT日志
  函数列表   :
  修改历史   :
  1.日    期   : 2016年3月6日
    作    者   : jimk
    修改内容   : 创建文件

******************************************************************************/
extern UINT32 g_ulRctLogSwitch;

VOID RCT_Set_LogSwitch(UINT32 uiSwitch);
VOID RCT_Get_LogSwitch(UINT32 *puiSwitch);

#define DbgRctModuleError(pcDbgInfo, ...)\
{\
  if(g_ulRctLogSwitch & LOG_LEVEL_ERROR )\
  {\
    (VOID)VOS_DebugTraceOutput((DEBUG_PRINT_FORMAT pcDbgInfo),__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__);\
  }\
}

#define DbgRctModuleWarning(pcDbgInfo, ...)\
{\
  if(g_ulRctLogSwitch & LOG_LEVEL_WARNING )\
  {\
    (VOID)VOS_DebugTraceOutput((DEBUG_PRINT_FORMAT pcDbgInfo),__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__);\
  }\
}

#define DbgRctModuleEvent(pcDbgInfo, ...)\
{\
  if(g_ulRctLogSwitch & LOG_LEVEL_EVENT )\
  {\
    (VOID)VOS_DebugTraceOutput((DEBUG_PRINT_FORMAT pcDbgInfo),__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__);\
  }\
}

#define DbgRctModuleDebug(pcDbgInfo, ...)\
{\
  if(g_ulRctLogSwitch & LOG_LEVEL_DEBUG )\
  {\
    (VOID)VOS_DebugTraceOutput((DEBUG_PRINT_FORMAT pcDbgInfo),__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__);\
  }\
}





