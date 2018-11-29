/******************************************************************************

                  版权所有 (C), 2015-2025, 网络科技有限公司

 ******************************************************************************
  文 件 名   : vos_hash.h
  版 本 号   : 初稿
  作    者   : jimk
  生成日期   : 2016年2月15日
  最近修改   :
  功能描述   : 简单的hash实现
  函数列表   :
  修改历史   :
  1.日    期   : 2016年2月15日
    作    者   : jimk
    修改内容   : 创建文件

******************************************************************************/

#define VOS_HASHTABLE_SIZE_MIN          100
#define VOS_HASHTABLE_SIZE_MAX          18000

#define VOS_HASHNODE_STRLEN               64

/*普遍型的比较函数*/
typedef LONG (*VOS_HASH_COMP_FUNC)(const VOID *pvArgv1, const VOID *pvArgv2);

/*普遍型的Hash值计算函数*/
typedef ULONG (*VOS_HASH_VALUE_FUNC)(const VOID *pvData);

/*提供的普遍型hash节点*/
typedef struct tagVosHashNode
{
    /*既可以当头节点的冲突链表，也可以当挂链节点*/
    VOS_DLIST_NODE_S            stBlkList;
    /*冲突链表数目*/
    ULONG                       ulBlkNum;
    /*Hash唯一的键值，要保证是表内唯一的，不可重复*/
    ULONG                       ulKey;
    /*Hash节点值*/
    CHAR                        acKeyString[VOS_HASHNODE_STRLEN];
    /*Hash节点保存的数据*/
    VOID                        *pvData;
}VOS_HASH_NODE_S;

/*提供的普遍型hash表*/
typedef struct tagVOSHashTable
{
    /*当前Hash表的数组*/
    VOS_HASH_NODE_S         **ppstArrayHashTbl;
    /*Hash表的比较函数*/
    VOS_HASH_COMP_FUNC       pfHashKeyCmp;
    /*Hash表的值计算函数*/
    VOS_HASH_VALUE_FUNC      pfHashCode;
    /*选取的合适的素数数组大小*/
    LONG                                 lPrime;
}VOS_HASH_TABLE_S;


VOS_HASH_TABLE_S *VOS_Hash_TableCreate(ULONG ulSize, VOS_HASH_COMP_FUNC pfHashCmp, VOS_HASH_VALUE_FUNC pfHashVal);
VOID    VOS_Hash_TableRelease(VOS_HASH_TABLE_S *pstHashTbl);

ULONG VOS_Hash_String(const VOID *pcStr);
LONG   VOS_Hash_CmpString(const VOID *pvArgv1, const VOID *pvArgv2);

ULONG VOS_Hash_ULONGValue(const VOID * pulKey);
LONG   VOS_Hash_ULONGCmp(const VOID *pvArgv1, const VOID *pvArgv2);

LONG   VOS_Hash_ULONGKeySetData(VOS_HASH_TABLE_S *pstHashTbl, ULONG ulKey, VOID *pvData);
LONG   VOS_Hash_ULONGKeyRemove(VOS_HASH_TABLE_S *pstHashTbl, ULONG ulKey);
LONG   VOS_Hash_ULONGKeyGetData(VOS_HASH_TABLE_S *pstHashTbl, ULONG ulKey, VOID **pvData);

LONG   VOS_Hash_StringKeySetData(VOS_HASH_TABLE_S *pstHashTbl, CHAR *pcString, VOID *pvData);
LONG   VOS_Hash_StringKeyRemove(VOS_HASH_TABLE_S *pstHashTbl, CHAR *pcString);

ULONG   VOS_Hash_TablePrime(ULONG ulSize);


