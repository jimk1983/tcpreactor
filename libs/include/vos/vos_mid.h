/******************************************************************************

                  版权所有 (C), 2015-2025, 网络科技有限公司

 ******************************************************************************
  文 件 名   : vos_mid.h
  版 本 号   : 初稿
  作    者   : jimk
  生成日期   : 2016年7月26日
  最近修改   :
  功能描述   : 模块的MID的定义
  函数列表   :
  修改历史   :
  1.日    期   : 2016年7月26日
    作    者   : jimk
    修改内容   : 创建文件

******************************************************************************/
/**********************RCT模块的TID定义*********************************/
/*说明: 该MID的定义主要是用来做内存管理，定位问题使用*/
/*16-31: 主要的任务模块定义, 低位0-15: 该模块ModuleID的下的子SubID定义*/


/***************************系统模块0x00000000 ********************************/
/*VOS 的系统内存*/
#define VOS_MID                               0x00000000


/***************************RCT网络架构0x0001xxxx*****************************/
/*网络架构的MID*/
#define RCT_MID                               0x00010000
#define RCT_MID_SID_SET(SID)         (RCT_MID  | SID )           
#define RCT_MID_SID_INI                 RCT_MID_SID_SET(0x0001)             /*0x00010001--表示INI模块使用的内存*/
#define RCT_MID_SID_TSK                RCT_MID_SID_SET(0x0002)             /*0x00010002--表示TSK模块使用的内存*/
#define RCT_MID_SID_EVT                RCT_MID_SID_SET(0x0003)             /*0x00010003--表示EVT模块使用的内存*/
#define RCT_MID_SID_UTL                RCT_MID_SID_SET(0x0004)            

/***************************share通用共享0x0002xxxx***************************/
/*通用共享MID*/
#define COM_QUE_MID                               0x00020000
#define COM_QUE_MID_SID_SET(SID)         (COM_QUE_MID | SID )           
#define COM_QUE_MID_SID_INI                 COM_QUE_MID_SID_SET(0x0001)             /*0x00010001--表示QUE模块使用的内存*/

/*TCPIP 模块*/
#define TCPIP_MID                                   0x00020100                                 /*0x0003-共享文件，01-TCP模块*/
#define TCPIP_MID_SID_SET(SID)             (TCPIP_MID| SID )
#define TCPIP_MID_SID_INI                     TCPIP_MID_SID_SET(0x0001)         
#define TCPIP_MID_SID_PKT                    TCPIP_MID_SID_SET(0x0002)          

/**************************drivers模块的MID********************************/
/*RawIp 模块*/
#define RIP_MID                                       0x00030000
#define RIP_MID_SID_SET(SID)                (RIP_MID | SID )
#define RIP_MID_SID_INI                         RIP_MID_SID_SET(0x01)              /*0x00030001--表示INI模块使用的内存*/

/*IFNET模块*/
#define IFNET_MID                                    0x00030100
#define IFNET_MID_SID_SET(SID)              (IFNET_MID | SID )
#define IFNET_MID_SID                            IFNET_MID_SID_SET(0x01)              /*0x00030001--表示默认使用的内存*/

/*NCARD模块*/
#define NCARD_MID                                    0x00030200
#define NCARD_MID_SID_SET(SID)              (NCARD_MID | SID )
#define NCARD_MID_SID                             NCARD_MID_SID_SET(0x01)              /*0x00030001--表示默认使用的内存*/



/**/
#define CLI_MID                                0x00040000
#define CLI_MID_SID_SET(SID)         (CLI_MID  | SID )           
#define CLI_MID_INI                         CLI_MID_SID_SET(0x01)             /*0x00010001--表示INI模块使用的内存*/


/*****************************************/
/*VGM 模块*/
#define VGM_MID                               0x10020000
#define VGM_MID_SID_SET(SID)         (VGM_MID| SID )
#define VGM_MID_SID_INI                 VGM_MID_SID_SET(0x0001)            /*0x10020001--表示INI模块使用的内存*/
#define VGM_MID_SID_CFG                VGM_MID_SID_SET(0x0002)            /*0x10020002--表示CFG模块使用的内存*/
#define VGM_MID_SID_SSL                VGM_MID_SID_SET(0x0003)
#define VGM_MID_SID_UDPS                VGM_MID_SID_SET(0x0004)


/*SWM 模块*/
#define SWM_MID                               0x10030000
#define SWM_MID_SID_SET(SID)        (SWM_MID| SID )
#define SWM_MID_SID_INI                 SWM_MID_SID_SET(0x0001)          /*0x10030001--表示INI模块使用的内存*/
#define SWM_MID_SID_TLS                 SWM_MID_SID_SET(0x0002)         /*0x10030002--表示TLS模块使用的内存*/
#define SWM_MID_SID_BIZ                 SWM_MID_SID_SET(0x0003)          /*0x10030003--表示BIZ模块使用的内存*/

/*NEM 模块*/
#define NEM_MID                               0x10040000
#define NEM_MID_SID_SET(SID)        (NEM_MID| SID )
#define NEM_MID_SID_INI                 NEM_MID_SID_SET(0x0001)          /*0x10040001--表示INI模块使用的内存*/
#define NEM_MID_SID_CON               NEM_MID_SID_SET(0x0002)          /*0x10040002--表示CON模块使用的内存*/
#define NEM_MID_SID_CTL               NEM_MID_SID_SET(0x0003)          /*0x10040003--表示CTL模块使用的内存*/
#define NEM_MID_SID_SES               NEM_MID_SID_SET(0x0004)          /*0x10040004--表示SES模块使用的内存*/
#define NEM_MID_SID_UDPS            NEM_MID_SID_SET(0x0005)          /*0x10040005--表示UDPS模块使用的内存*/


/*PFW 模块*/
#define PFW_MID                               0x10050000
#define PFW_MID_SID_SET(SID)        (PFW_MID| SID )
#define PFW_MID_SID_INI                 PFW_MID_SID_SET(0x0001)          /*0x10040001--表示INI模块使用的内存*/
#define PFW_MID_SID_CON               PFW_MID_SID_SET(0x0002)          /*0x10040002--表示CON模块使用的内存*/
#define PFW_MID_SID_CTL               PFW_MID_SID_SET(0x0003)          /*0x10040003--表示CTL模块使用的内存*/
#define PFW_MID_SID_SES               PFW_MID_SID_SET(0x0004)          /*0x10040004--表示SES模块使用的内存*/
#define PFW_MID_SID_INNER            PFW_MID_SID_SET(0x0005)          /*0x10040005--表示INNER模块使用的内存*/


/*APY 模块*/
#define APY_MID                               0x10060000
#define APY_MID_SID_SET(SID)        (APY_MID| SID )
#define APY_MID_SID_INI                 APY_MID_SID_SET(0x0001)          /*0x10040001--表示INI模块使用的内存*/
#define APY_MID_SID_PKT               APY_MID_SID_SET(0x0002)          /*0x10040002--表示CON模块使用的内存*/
#define APY_MID_SID_CON               APY_MID_SID_SET(0x0003)          /*0x10040002--表示CON模块使用的内存*/


/*VDM 模块*/
#define VDM_MID                               0x10070000
#define VDM_MID_SID_SET(SID)        (VDM_MID| SID )
#define VDM_MID_SID_INI                 VDM_MID_SID_SET(0x0001)        
#define VDM_MID_SID_MSG               VDM_MID_SID_SET(0x0002)          


/*AAA 模块*/
#define AAA_MID                               0x10080000
#define AAA_MID_SID_SET(SID)        (AAA_MID| SID )
#define AAA_MID_SID_INI                 AAA_MID_SID_SET(0x0001)          
#define AAA_MID_SID_MSG               AAA_MID_SID_SET(0x0002)          


/*DBM 模块*/
#define DBM_MID                           0x10090000
#define DBM_MID_SID_SET(SID)        (DBM_MID| SID )
#define DBM_MID_SID_INI                 DBM_MID_SID_SET(0x0001)          
#define DBM_MID_SID_MSG               DBM_MID_SID_SET(0x0002)          


/*PROXY 模块*/
#define PROXY_MID                               0x100A0000
#define PROXY_MID_SID_SET(SID)        (PROXY_MID| SID )
#define PROXY_MID_SID_INI                 PROXY_MID_SID_SET(0x0001)          
#define PROXY_MID_SID_MSG               PROXY_MID_SID_SET(0x0002)          

/*VSNAT 模块*/
#define VSNAT_MID                               0x100B0000
#define VSNAT_MID_SID_SET(SID)        (VSNAT_MID| SID )
#define VSNAT_MID_SID_INI                 VSNAT_MID_SID_SET(0x0001)          
#define VSNAT_MID_SID_MSG               VSNAT_MID_SID_SET(0x0002)          


