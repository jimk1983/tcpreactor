/******************************************************************************

                  版权所有 (C), 2015-2025, 网络科技有限公司

 ******************************************************************************
  文 件 名   : vos_epoll.h
  版 本 号   : 初稿
  作    者   : jimk
  生成日期   : 2016年1月12日
  最近修改   :
  功能描述   : EPOLL封装
  函数列表   :
  修改历史   :
  1.日    期   : 2016年1月12日
    作    者   : jimk
    修改内容   : 创建文件

******************************************************************************/
#ifndef _VOS_EPOLL_H_
#define _VOS_EPOLL_H_


/*EPOLL控制-添加*/
#define VOS_EPOLL_CTL_ADD   0x01
/*EPOLL控制-修改*/
#define VOS_EPOLL_CTL_MOD   0x02
/*EPOLL控制-删除*/
#define VOS_EPOLL_CTL_DEL   0x04

/*EPOLL事件掩码--IN事件*/
#define VOS_EPOLL_MASK_POLLIN                 0x01
/*EPOLL事件掩码--OUT事件*/
#define VOS_EPOLL_MASK_POLLOUT              0x02
#define VOS_EPOLL_MASK_POLLNOTIFY         0x04

/*打开接收*/
#define VOS_EPOLL_CTRL_INOPEN               0x00
/*关闭接收*/
#define VOS_EPOLL_CTRL_INCLOSE              0x01
/*打开发送*/
#define VOS_EPOLL_CTRL_OUTOPEN            0x02
/*关闭发送*/
#define VOS_EPOLL_CTRL_OUTCLOSE           0x03

/*EPOLL非法值*/
#define VOS_EPOLL_INVALID                       -1

/*成功返回码*/
#define VOS_EPOLL_OK                                0
/*失败返回码*/
#define VOS_EPOLL_ERR                              -1

/*单个EPOLL最大处理*/
#define VOS_EPOLL_EVENT_MAXNUM          20000 

/*EPOLL无穷等待*/
#define VOS_EPOLL_TIME_INFININ             -1
#define VOS_EPOLL_TIME_200S                  200000


/*单次等待EPOLL响应的事件数*/
#define VOS_EPOLL_EVTWAIT_NUM           48

/*单个EPOLL单元*/
typedef struct tagVosEPollEvent
{
    INT32    lSockFd;               /*socket fd*/ 
    UINT32  ulEventMask;        /*事件掩码*/
}VOS_EPOLL_EVENT_S;


/*单次发生的EPOLL事件*/
typedef struct tagVosEPollWaitEvt
{
    VOS_EPOLL_EVENT_S astEevent[VOS_EPOLL_EVTWAIT_NUM];     /*单次发生的所有事件*/
    UINT32                    ulEvtNum;                                               /*EPOLL事件数量*/
}VOS_EPOLL_WAIT_EVT_S;

INT32 VOS_EPOLL_Create(UINT32 ulEpollNum);

VOID  VOS_EPOLL_Release(INT32 lEpollId);

INT32 VOS_EPOLL_Ctrl(INT32 lEpollFd, INT32 lSockfd, UINT32 ulEpollCtl, UINT32 ulEpollMask);

INT32 VOS_EPOLL_Wait(INT32 lEpollFd, VOS_EPOLL_WAIT_EVT_S *pstEvent, INT32 lTimeout);


#endif

