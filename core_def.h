/******************************************************************************

                  ��Ȩ���� (C), 2015-2025, ����Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : core_def.h
  �� �� ��   : ����
  ��    ��   : jimk
  ��������   : 
  ����޸�   :
  ��������   : ϵͳ�ĺ����̵߳�����
  �����б�   :
  �޸���ʷ   :

******************************************************************************/

/*RCT���̳߳�ʼ������������, 
���Ը��ݸ���ϵͳ��ͬ����ͬ��
����ϵͳ��ʼ��ʱ����Ҫ��*/
#define SYS_CORE_PTHREAD_MGR_NUM                    1   /*������������߳�����*/

#define SYS_CORE_PTHREAD_TCP_NUM                     1    /*����TCP�����߳�����*/

#define SYS_CORE_PTHREAD_SSL_NUM                     2    /*����SSL ҵ���̵߳�����*/

#define SYS_CORE_PTHREAD_UDPS_NUM                  1    /*����UDPSҵ���̵߳�����*/

#define SYS_CORE_PTHREAD_RAWIP_NUM                0   /*����RAWIP�շ����������߳�����*/

#define SYS_CORE_PTHREAD_ARP_NUM                    0   /*ARP�����߳�, ���������㽡��̽��֮��*/

#define SYS_CORE_PTHREAD_AAA_NUM                    1    /*AAA�û���֤��Ȩ�߳�, DBҲ���ڸ��߳�*/

/*����ܹ����߳�ͨ��ʹ�õģ�����127�Ķ˿ں�ͨ�ž���*/
#define SYS_LOCAL_START_PORT_TIMER                  20000         /*ÿ���̶߳�ʱ������ʼ�˿�*/
#define SYS_LOCAL_START_PORT_MGT                     21000         /*ÿ���̺߳˼���Ϣ����ʼ�˿�*/
#define SYS_LOCAL_START_PORT_EXPIR                  22000         /*ÿ���߳��ϻ�����ʼ�˿�*/

#define SYS_LOCAL_MANAGER_PORT                          9888         /*���صĿ�������TCP����˿�*/

#define SYS_LOCAL_TIMER_CRYCLE_PORT                25000         /*���ض�ʱ�������˿�*/
