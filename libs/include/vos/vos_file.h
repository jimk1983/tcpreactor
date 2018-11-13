/******************************************************************************

                  版权所有 (C), 2015-2025, 网络科技有限公司

 ******************************************************************************
  文 件 名   : vos_file.h
  版 本 号   : 初稿
  作    者   : jimk
  生成日期   : 2016年10月22日
  最近修改   :
  功能描述   : 文件操作适配
  函数列表   :
  修改历史   :
  1.日    期   : 2016年10月22日
    作    者   : jimk
    修改内容   : 创建文件

******************************************************************************/


/*最大的文件名长度*/
#define VOS_FILENAME_LEN        256
#define VOS_DIRSNAME_LEN        256

typedef struct tagVosDirsFilesInfo VOS_DIRS_FILESINFO_S;

/*获取目录的所有文件信息*/
struct tagVosDirsFilesInfo{
    VOS_DIRS_FILESINFO_S *next;    

    CHAR    acFileName[VOS_FILENAME_LEN + 1];           /*文件名称*/
    INT32   uiFileSize;                                 /*文件大小*/
};

UINT32 VOS_FileGetSize(CHAR *pcFullName);

LONG VOS_FileRead(CHAR *pcFilename, INT32 *pcLen, UCHAR **ppucData );

LONG VOS_DirGetCurrentPath(CHAR *pcDirPath, INT32 iMaxLen);

LONG VOS_DirIsExist(const CHAR *pcDirPath);

LONG VOS_DirsCreate(CHAR *pcDirPath);

LONG VOS_DirsCurrentRemove(CHAR *pcDir);

LONG VOS_DirsRemove(CHAR *pcDirPath);

VOS_DIRS_FILESINFO_S *VOS_DirsGetFilesInfo(CHAR *pcDir);

VOID VOS_DirsFileInfoFree(VOS_DIRS_FILESINFO_S *pstDirFileInfo);

LONG VOS_FileRemove(CHAR *pcFilePath);

LONG VOS_FileCreate(CHAR *pcFilePath);

LONG VOS_FileIsExist(const CHAR *pcFilePath);


LONG VOS_EventfdCreate(LONG lFlags);

VOID VOS_EventfdClose(LONG lEventFd);

LONG VOS_EventfdRead(LONG lEventFd, VOID *pcBuf, UINT32 uiSize);

LONG VOS_EventfdWrite(LONG lEventFd, VOID *pcBuf, UINT32 uiSize);


