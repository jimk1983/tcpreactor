/******************************************************************************

                  版权所有 (C), 2015-2025, 网络科技有限公司

 ******************************************************************************
  文 件 名   : vos_route.h
  版 本 号   : 初稿
  作    者   : jimk
  生成日期   : 2016年6月28日
  最近修改   :
  功能描述   : 系统路由相关
  函数列表   :
  修改历史   :
  1.日    期   : 2016年6月28日
    作    者   : jimk
    修改内容   : 创建文件

******************************************************************************/

#define VOS_RTF_UP                        0x0001      /*ioctl标记，表示可用的*/
#define VOS_RTF_GATEWAY             0x0002      
#define VOS_RTF_HOST                    0x0004

#define VOS_PATH_PROCROUTE        "/proc/net/route"
#define VOS_DEVNAME_LEN              32                  /*设备接口名称长度*/
#define VOS_ROUTETBL_SIZE           32                  /*本系统路由表最大表数量*/


/*路由表项*/
typedef struct VOSRouteEntry
{
    VOS_IPADDR_T      uiDstip;                /*目的IP地址*/
    VOS_IPADDR_T      uiMask;                /*掩码*/
    VOS_IPADDR_T      uiGateway;          /*该目的出接口网关地址*/
    UINT32      ulFlags;                          /*标记是否可用*/
    UINT32      uiMetric;                         /*优先级*/
    UINT32      uiMtu;                            /*该路由接口的MTU*/
    CHAR        acDevName[VOS_DEVNAME_LEN];     /*该接口的名称*/
}VOS_ROUTE_ENTRY_S;

/*本系统的路由表*/
typedef struct tagVOSSysRouteTable
{
    VOS_ROUTE_ENTRY_S   astArrayRouteTbl[VOS_ROUTETBL_SIZE];
}VOS_SYS_ROUTE_TBL_S;

INT32   VOS_GetCurrentRouteTable(VOS_SYS_ROUTE_TBL_S *pstRouteTbl);

INT32   VOS_GetDefaultGatewayRouteEntry(VOS_ROUTE_ENTRY_S *pstRouteEntry);

INT32   VOS_GetDefaultGateway(VOS_IPADDR_T *puiDeaultGateway);

LONG    VOS_GetDstIpBestRoute(VOS_IPADDR_T uiDstIp, VOS_ROUTE_ENTRY_S *pstRouteEntry);

LONG    VOS_RouteEntryAdd( VOS_IPADDR_T uiDstip,
                                                   VOS_IPADDR_T uiMask,
                                                   VOS_IPADDR_T uiGateway,
                                                   UINT32   uiMetric,
                                                   CHAR *pcifName);

LONG VOS_RouteEntryDel( VOS_IPADDR_T uiDstip,
                                                  VOS_IPADDR_T uiMask,
                                                  VOS_IPADDR_T uiGateway);


//-----------------------------ARP 表项: /proc/net/arp----------------------------

#define VOS_PATH_PROCARP        "/proc/net/arp"
#define VOS_ARPTBL_SIZE           128                  /*本系统ARP表支持的最大表数量*/

/*路由表项*/
typedef struct VOSARPEntry
{
    CHAR         acIpAddr[VOS_IPV4STR_MAXLEN];             /*内网IP地址*/
    UINT32      uiHwType;                                              /*HW Type*/
    UINT32      uiFlags;         
    CHAR         acMacAddr[32];          /*该IP对应的Mac地址字符串*/
    CHAR        acDevName[VOS_DEVNAME_LEN];     /*该接口的名称*/
}VOS_ARP_ENTRY_S;

/*本系统的ARP表*/
typedef struct tagVOSSysARPTable
{
    VOS_ARP_ENTRY_S   astArrayArpTbl[VOS_ARPTBL_SIZE];
}VOS_SYS_ARP_TBL_S;

