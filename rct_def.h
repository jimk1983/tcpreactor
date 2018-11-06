#ifndef _RCT_DEF_H_
#define _RCT_DEF_H_
/******************************************************************************

                  版权所有 (C), 2017-2020, 网络有限公司

 ******************************************************************************
  文 件 名   : rct_def.h
  版 本 号   : 初稿
  作    者   : jimk
  生成日期   : 2018年4月17日
  最近修改   :
  功能描述   : RCT公共的定义头文件, 也提供给外部了
  函数列表   :
  修改历史   :
  1.日    期   : 2018年4月17日
    作    者   : jimk
    修改内容   : 创建文件

******************************************************************************/

#if VOS_PLAT_LINUX
#define PTHREADVAL  __thread;   /*linux平台可以用线程变量*/
#else
#define PTHREADVAL
#endif

/*通用的字符串长度*/
#define RCT_STR_LEN             32

/*默认每个线程的EPOLL最大处理数目*/
#define RCT_EPOLL_WAIT_NUMS     64

/*定义的每个线程的类型*/
/*注意SSL之类可能有多个线程*/
typedef enum
{
    RCT_TYPE_MP_CLI = 0,           /*定义管理面线程: 命令行*/
    RCT_TYPE_MP_AAA,               /*定义管理面线程: AAA类型*/
    RCT_TYPE_MP_PROXY,             /*定义管理面线程: DNS代理业务*/
    RCT_TYPE_DP_TCP,               /*定义数据面线程: TCP类型*/
    RCT_TYPE_DP_SSL,               /*定义数据面线程: SSL类型*/
    RCT_TYPE_DP_UDPS,              /*定义数据面线程: UDPS类型*/
    RCT_TYPE_DP_TWORK,             /*定义数据面线程: Timer周期功能类型: 探测都可以放在这里*/
    RCT_TYPE_DP_VSNAT,             /*定义数据面线程: 虚拟网卡Tap的SNAT线程*/

    RCT_TYPE_NUMS,
}RCT_TYPE_E;


/*在单一线程里的业务种类，最多就16个大类*/
#define RCT_SUBTYPE_MAXNUMS     16

/*默认的一般都是1个业务，索引为0*/
#define RCT_SUBTYPE_SINGLE    0

/*业务类型只有单个线程的, SSL有多个线程*/
#define RCT_PTHNUM_ONLYONE    0

/*SSL线程的子类型, 都需要注册运行初始化函数
消息也用这个进行区分处理*/
typedef enum
{
    RCT_SUBTYPE_SSL_SWM=0,
    RCT_SUBTYPE_SSL_UPF,
    RCT_SUBTYPE_SSL_NEM,
    RCT_SUBTYPE_SSL_PFM,
    RCT_SUBTYPE_SSL_TCM,

    RCT_SUBTYPE_SSLNUMS
}RCT_SUBTYPE_SSL_E;

#define RCT_TYPE_PTHMAXNUMS     32      /*每个业务的最多子线程数量*/
#define RCT_TYPE_SIGLENUMS      1       /*默认就是1个线程*/
#define RCT_TYPE_SSL_MUTILNUMS  2       /*SSL启动了2个线程*/

#define RCT_MSGLOCKPLY_NONEED      0
#define RCT_MSGLOCKPLY_NEED        1   /*是否需要可靠传输，如果需要，则会将用户数据锁住，以便在返回时候确认用*/


/**************************************************************/
/*****************       消息的结构                *****************/
/**************************************************************/

typedef struct tagRctMsgHead
{
    /*用于获取通信矩阵*/
    UINT32  ulSrcRctType;               /*线程的类型*/
    /*消息子业务的区别回调函数的处理*/
    UINT32  ulSrcRctSubType;            /*单个线程包含多个模块，例如： SSL可能有SWM/NEM/TCM等*/
    UINT32  ulSrcPthIndex;              /*某类型下的第几个线程索引， 例如:SSL有两个线程*/
    UINT32  ulSrcMsgEventFd;            /*用于消息返回，直接触发即可*/
    
    
    /*用于获取通信矩阵*/
    UINT32  ulDstRctType;               /*线程的类型*/
    /*消息子业务的区别回调函数的处理*/
    UINT32  ulDstRctSubType;            /*单个线程包含多个模块，例如： SSL可能有SWM/NEM/TCM等*/
    UINT32  ulDstPthIndex;              /*某类型下的第几个线程索引， 例如:SSL有两个线程*/
    
    /*节点如何确保消息返回时候，节点已经释放, 这个机制用于返回类的消息，不返回类的消息不用设置该标记*/
    /*是否需要可靠回复: RCT_MSGLOCKPLY_NEED:
    用于当前线程和别的线程的数据查询回复消息:
    例如： NEM发送消息到AAA查询登录，AAA将结果返回，需要锁住Connon节点*/
    UINT32  ulBackFlag;            
    /*指针强制转换*/
    UINT32  ulLockRplyId;               /*消息随机ID: 0: 表示新消息，>0表示回复消息，需要检查*/
}RCT_MSG_HEAD_S;


/*通用的回调函数: 用于消息处理, 该Buf不能超过最大的Vos的队列节点长度*/
/*消息不能发送大量的数据仅用于通知用*/
typedef void (*pfComMsgHandlerCb)(char *pcBuf);

typedef void (*pfComMsgHandlerCallBack)(RCT_MSG_HEAD_S *pstHeadInfo, CHAR *pcMsg, UINT32 uiLen);


/*内部的触发器管理器*/
typedef struct  tagRctEvtReactor    RCT_EVTREACTOR_S;


/*当前最大的线程数量，每个线程可以绑定CPU，因此一般就是指CPU的个数*/
#define RCT_PTHMAXNUMS      128

/*线程的索引信息*/
typedef struct tagRctIndexInfo
{
    ULONG      ulRctType;               /*Rct的线程类型*/
    ULONG      ulPthSubIndex;           /*当前的子线程索引, SSL有多个线程，所以要这个索引*/
    ULONG      ulArryIndex;             /*记录当前线程存放全局配置数组所在的索引号*/
    ULONG      ulMsgEventFd;            /*用于消息记录返回*/
}RCT_INDEXINFO_S, *PRCT_INDEXINFO_S;




/*消息发送的时候，目的信息的封装结构*/
typedef struct tagRctMsgDstInfo
{
    ULONG ulSrcSubType;     /*源子类型: 线程主类型可以自动获取，但是子类型需要用户填写, 大部分RCT_SUBTYPE_SINGLE
                             *目前就SSL，在单个线程内有多个子业务类型，其余线程业务都很单一*/
    ULONG ulDstRctType;     /*目的类型: RCT_TYPE_E*/
    ULONG ulDstSubType;     /*目的子类型： 大部分RCT_SUBTYPE_SINGLE*/
    ULONG ulDstPthIndex;    /*目的线程索引： 并发线程，SSL有多个，大部分只有一个: 0索引:RCT_PTHNUM_ONLYONE*/
}RCTMSG_DSTINFO_S, *PRCTMSG_DSTINFO_S;

#endif