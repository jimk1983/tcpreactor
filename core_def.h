/******************************************************************************

                  版权所有 (C), 2015-2025, 网络科技有限公司

 ******************************************************************************
  文 件 名   : core_def.h
  版 本 号   : 初稿
  作    者   : jimk
  生成日期   : 
  最近修改   :
  功能描述   : 系统的核心线程的配置
  函数列表   :
  修改历史   :

******************************************************************************/

/*RCT的线程初始化的数量定义, 
可以根据各个系统不同而不同，
但是系统初始化时是需要的*/
#define SYS_CORE_PTHREAD_MGR_NUM                    1   /*定义命令管理线程数量*/

#define SYS_CORE_PTHREAD_TCP_NUM                     1    /*定义TCP监听线程数量*/

#define SYS_CORE_PTHREAD_SSL_NUM                     2    /*定义SSL 业务线程的数量*/

#define SYS_CORE_PTHREAD_UDPS_NUM                  1    /*定义UDPS业务线程的数量*/

#define SYS_CORE_PTHREAD_RAWIP_NUM                0   /*定义RAWIP收发内网报文线程数量*/

#define SYS_CORE_PTHREAD_ARP_NUM                    0   /*ARP代理线程, 可以再做点健康探测之类*/

#define SYS_CORE_PTHREAD_AAA_NUM                    1    /*AAA用户认证授权线程, DB也放在该线程*/

/*网络架构中线程通信使用的，本地127的端口号通信矩阵*/
#define SYS_LOCAL_START_PORT_TIMER                  20000         /*每个线程定时器的起始端口*/
#define SYS_LOCAL_START_PORT_MGT                     21000         /*每个线程核间消息的起始端口*/
#define SYS_LOCAL_START_PORT_EXPIR                  22000         /*每个线程老化的起始端口*/

#define SYS_LOCAL_MANAGER_PORT                          9888         /*本地的控制器的TCP对外端口*/

#define SYS_LOCAL_TIMER_CRYCLE_PORT                25000         /*本地定时器触发端口*/
