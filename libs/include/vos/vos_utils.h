/******************************************************************************

                  版权所有 (C), 2015-2025, 网络科技有限公司

 ******************************************************************************
  文 件 名   : vos_utils.h
  版 本 号   : 初稿
  作    者   : jimk
  生成日期   : 2016年1月2日
  最近修改   :
  功能描述   : 通用功能头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2016年1月2日
    作    者   : jimk
    修改内容   : 创建文件

******************************************************************************/
/*----------------------------------------------*
 * 常用的宏定义                                       *
 *----------------------------------------------*/
/*IPV4全部地址字符串最大长度*/
#define VOS_IPV4STR_MAXLEN 16
/*IPV4地址单个字符串长度*/
#define VOS_IPADDR_DOILEN  4
     
/*定义32位整形字符串长度*/
#define VOS_INT32STR_MAXLEN    12

/*一般常见文件名的长度*/
#define VOS_FILE_MAXLEN     256

/*域名长度*/
#define VOS_URL_MAXLEN      128

typedef VOID (*VOS_pfDataHandleCb)(CHAR *pvData, UINT32 uiDataSize);

/*----------------------------------------------*
 * 常用的回调函数定义                                       *
 *----------------------------------------------*/
/*常见的回调函数定义1*/
typedef VOID (*VOS_CALLBACK)(VOID *pvData);

typedef VOID (*VOS_CALLBACK_UNINIT)();

typedef struct tagVOS_Callback VOS_CALLBACK_S;
struct tagVOS_Callback
{
      VOID *pvcbFunc;
      VOID *pvData;
};
#define VOS_CALLBACK_INIT(pstCallBack_, pfFunc_, pvUserData_)\
do{\
    (pstCallBack_)->pvcbFunc = (VOID *)pfFunc_;\
    (pstCallBack_)->pvData   = (VOID *)pvUserData_;\
}while(0);



/*常见的回调函数定义2*/
typedef VOID (*VOS_ARGV_CALLBACK)(VOID *pvArgv1, VOID *pvArgv2);

typedef struct tagVOS_ArgvCallback VOS_ARGV_CALLBACK_S;

struct tagVOS_ArgvCallback
{
  VOID *pvcbFunc;
  VOID *pvArgv1;
  VOID *pvArgv2;
};

#define VOS_ARGV_CALLBACK_INIT(pstArgvCallBack_, pfFunc_, pvArgv1_, pvArgv2_)\
do{\
    (pstArgvCallBack_)->pvcbFunc = (VOID *)pfFunc_;\
    (pstArgvCallBack_)->pvArgv1  = (VOID *)pvArgv1_;\
    (pstArgvCallBack_)->pvArgv2  = pvArgv2_;\
}while(0);

BOOL VOS_CpuIsBigEndian();

ULONG VOS_GetSelfPId();


UINT32 VOS_atoi(CHAR *pcStr);
UINT32 VOS_itoa(CHAR *pcStr, UINT32 iVal);

USHORT VOS_Random16();
UINT32  VOS_Random32();


