/******************************************************************************

                  版权所有 (C), 2015-2025, 网络科技有限公司

 ******************************************************************************
  文 件 名   : vos_config.h
  版 本 号   : 初稿
  作    者   : jimk
  生成日期   : 
  最近修改   :
  功能描述   : 操作系统的基本定义
  函数列表   :
  修改历史   :
******************************************************************************/
#ifndef _VOS_CONFIG_H_
#define _VOS_CONFIG_H_

/*定义的平台是MAC系统*/
#define     VOS_PLAT_MAC             0
/*定义的平台是Win系统*/
#define     VOS_PLAT_WIN             0
/*定义的平台是Linux系统*/
#define     VOS_PLAT_LINUX          1

/*定义的是32位系统*/
#define     VOS_PLAT_PC32           0
/*定义的是64位系统*/
#define     VOS_PLAT_PC64           1

/*小端定义*/
#ifndef VOS_LITTLE_ENDIAN
#define VOS_LITTLE_ENDIAN     1234
#endif

/*大端定义*/
#ifndef VOS_BIG_ENDIAN
#define VOS_BIG_ENDIAN          4321
#endif 

/*平台的字节序,PC一般都是小端的*/
#define VOS_BYTE_ORDER        VOS_LITTLE_ENDIAN

/*包含的是openssl库的头文件*/
/*
#include <openssl/ssl.h>
#include <openssl/x509v3.h>
#include <openssl/x509_vfy.h>
#include <openssl/crypto.h>
#include <openssl/pkcs12.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/aes.h>
*/

/*包含的是各个系统常见的头文件*/
#if VOS_PLAT_WIN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>

#elif VOS_PLAT_LINUX
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <time.h>
#include <time.h>
#include <pthread.h>
#include <math.h>

#include <netdb.h>
#include <netinet/if_ether.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <net/route.h>

#include <linux/rtnetlink.h>    

#include <dirent.h>

/*BPF规格*/
#include <linux/if_ether.h>
#include <net/if.h>
#include <linux/filter.h>

/*支持pcap扩展*/
//#include <pcap/pcap.h>   //suse有该头文件
//#include <pcap.h>

#elif VOS_PLAT_MAC
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <pthread.h> 
#include <math.h>

#endif

#endif


