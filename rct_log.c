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
#include <vos_pub.h>
#include <rct_pub.h>

/*Ĭ�ϴ򿪴������־*/
UINT32 g_ulRctLogSwitch = LOG_LEVEL_ERROR;   

/*����RCT���Կ���*/
VOID RCT_Set_LogSwitch(UINT32 uiSwitch)
{
    g_ulRctLogSwitch = uiSwitch;
}

/*��ȡRCT���Կ���*/
VOID RCT_Get_LogSwitch(UINT32 *puiSwitch)
{
    if ( NULL == puiSwitch)
    {
        return;
    }
    *puiSwitch = g_ulRctLogSwitch;
}






