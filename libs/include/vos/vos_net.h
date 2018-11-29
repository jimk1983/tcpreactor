/******************************************************************************

                  版权所有 (C), 2015-2025, 网络科技有限公司

 ******************************************************************************
  文 件 名   : vos_ifnet.h
  版 本 号   : 初稿
  作    者   : jimk
  生成日期   : 2016年4月17日
  最近修改   :
  功能描述   : 网络接口适配
  函数列表   :
  修改历史   :
  1.日    期   : 2016年4月17日
    作    者   : jimk
    修改内容   : 创建文件

******************************************************************************/
/*网络字节序地址*/
typedef UINT32    VOS_IPADDR_T;

#define VOS_IPV4ADDR_LEN            4       /*IPV4地址长度*/
#define VOS_IPV4MACADDR_LEN         6       /*MAC地址长度*/
/*硬件地址*/
typedef UCHAR   MACADDR[6];

#define VOS_IPV4ADDR_STRLEN          16

/*网卡接口名称*/
#define VOS_IFNAME_MAX_LEN     16

/*定义包对齐*/
#if VOS_PLAT_LINUX
#define VOS_ATTRIBATE_PCKALGN    __attribute__((packed, aligned(1)))
#elif VOS_PLAT_WIN
#define VOS_ATTRIBATE_PCKALGN    #program pack(1)
#endif

#define VOS_IPV4_ETH_LEN            14          /*链路层长度*/
#define VOS_IPV4_IPHEAD_LEN       20          /*IPV4不包含选项头长度*/



INT32   VOS_IPV4StrToN(CHAR *pcStr);

INT32   VOS_NToIPV4Str(UINT32 iNAddr, CHAR *pcStr);

LONG    VOS_GetMACAddrByIfName(CHAR *pcIfName, UCHAR *pcMacAddr);

LONG    VOS_GetIPAddrByIfName(CHAR *pcIfName, VOS_IPADDR_T *piIPAddr);

LONG    VOS_GetIPMaskByIfName(CHAR *pcIfName, VOS_IPADDR_T *piIPMask);

LONG    VOS_GetIndexByIfName(CHAR *pcIfName, UINT32 *puiIndex);

//LONG    VOS_GetGateway(CHAR *pcGateway);

LONG    VOS_GetDNSServerIp(VOS_IPADDR_T uiDNSSev[4]);


USHORT  VOS_IPV4Checksum(void *buf, UINT32 size);

UCHAR        *VOS_mtoa (UCHAR *pucMacAddr);
UINT32      VOS_htonl(UINT32 ulHValue);
UINT32      VOS_ntohl(UINT32 ulHValue);
USHORT     VOS_htons(USHORT usHValue);
USHORT     VOS_ntohs(USHORT usHValue);
UINT32      VOS_aton_s(CHAR *pcStr);
UINT32      VOS_aton(const CHAR *pcStr);
UINT32      VOS_ntoa(UINT32 iNAddr, CHAR *pcStr);

