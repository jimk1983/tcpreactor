/******************************************************************************

                  ��Ȩ���� (C), 2015-2025, ����Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : rct_dbg.c
  �� �� ��   : ����
  ��    ��   : jimk
  ��������   : 2016��3��6��
  ����޸�   :
  ��������   : RCT��־
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2016��3��6��
    ��    ��   : jimk
    �޸�����   : �����ļ�

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





