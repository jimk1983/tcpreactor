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
#include <vos_pub.h>
#include <rct_pub.h>

/*默认打开错误的日志*/
UINT32 g_ulRctLogSwitch = LOG_LEVEL_ERROR;   

/*设置RCT调试开关*/
VOID RCT_Set_LogSwitch(UINT32 uiSwitch)
{
    g_ulRctLogSwitch = uiSwitch;
}

/*获取RCT调试开关*/
VOID RCT_Get_LogSwitch(UINT32 *puiSwitch)
{
    if ( NULL == puiSwitch)
    {
        return;
    }
    *puiSwitch = g_ulRctLogSwitch;
}






