/******************************************************************************

                  版权所有 (C), 2015-2025, 网络科技有限公司

 ******************************************************************************
  文 件 名   : vos_mm.h
  版 本 号   : 初稿
  作    者   : jimk
  生成日期   : 2016年1月17日
  最近修改   :
  功能描述   : 内存管理
  函数列表   :
  修改历史   :
  1.日    期   : 2016年1月17日
    作    者   : jimk
    修改内容   : 创建文件

****************************************************/

/*目前业务就定义了2K,4K,8K,16K，可以扩展*/
#define VOS_MEM_CONNNUM          1500

#define VOS_MEM_32_NUM           VOS_MEM_CONNNUM
#define VOS_MEM_64_NUM           VOS_MEM_CONNNUM
#define VOS_MEM_128_NUM          VOS_MEM_CONNNUM
#define VOS_MEM_256_NUM          VOS_MEM_CONNNUM
#define VOS_MEM_512_NUM          VOS_MEM_CONNNUM
#define VOS_MEM_1K_NUM           VOS_MEM_CONNNUM
#define VOS_MEM_2K_NUM           10                 /*当前业务上不会用到*/
#define VOS_MEM_4K_NUM           10                 /*当前业务上不会用到*/
#define VOS_MEM_8K_NUM           10                 /*当前业务上不会用到*/
#define VOS_MEM_16K_NUM          10000              /*业务的形式以16K的为主, 每个用户最大20个缓存能力, 最多200个并发*/
#define VOS_MEM_32K_NUM          10               
#define VOS_MEM_64K_NUM          10
#define VOS_MEM_128K_NUM         10

#define VOS_CHAR_SIZE            sizeof(CHAR) 

#define VOS_FILEPATH_MAXLEN      256

#define VOS_MEM_FREED        0
#define VOS_MEM_USED         1

INT32 VOS_MemMgmt_Init();

VOID  VOS_MemMgmt_UnInit();

CHAR *VOS_MemCreate_X(ULONG ulMid, ULONG ulBufSize, CHAR *pcFileName, ULONG ulLine);

VOID  VOS_MemFree_X(CHAR *pcMemBuf);

#define VOS_Malloc(ulMid, ulSize)  VOS_MemCreate_X(ulMid, ulSize,__FILE__,__LINE__)

#define VOS_Free(pcMemBuf)    VOS_MemFree_X(pcMemBuf)

VOID  VOS_MM_MidTypeShow(UINT32 uiType,  UINT32 uiMid);

VOID  VOS_MM_TypeShow(UINT32 uiType);

VOID  VOS_MM_Show();


