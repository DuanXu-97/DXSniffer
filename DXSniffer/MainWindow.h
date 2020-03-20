#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"
#include "stdio.h"
#include "winsock2.h"
#include "stdint.h" 
#include "pcap.h"
#include "windows.h"
#include "Thread.h"
#include "Dialog.h"


#include "QRadioButton.h"
#include "QAbstractButton.h"
#include "QListWidget.h"
#include "QMessageBox.h"
#include "QObject.h"
#include "QButtonGroup.h"
#include "QLabel.h"
#include "QDebug.h"

#define ploy 0x04C11DB7                         //��׼��CRC32����ʽ

#define alt_64 long long 
#define alt_u64 unsigned long long 
#define alt_8    char  
#define alt_u8   unsigned char  
#define alt_32   int  
#define alt_u32  unsigned int  



#pragma comment ( lib, "wpcap.lib" )
#pragma comment ( lib, "ws2_32" )

Q_DECLARE_METATYPE(u_char)
Q_DECLARE_METATYPE(u_short)

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = Q_NULLPTR);
	~MainWindow();
	int printDeviceList();

	public slots :
	void clickSubmit();
	void clickPacketDetail(QTableWidgetItem *pktItem);
	void startCapture(int dn, const char *filter);
	void receivePacket(const struct pcap_pkthdr *header, const u_char *pkt_data);
	void clickSendARP();
	void clickSendTCP();
	void errorMessage(QString msg);
	void receiveMsg(QString msg);

signals: //�ź�
	void captureSetting(int dn, const char *ft);

private:
	Ui::MainWindowClass ui;
	captureThread cThread;
	sendThread sThread;
};

typedef struct Ethernet_pkt {
	u_char byte[1518];
}Enther_pkt;

Q_DECLARE_METATYPE(Ethernet_pkt)

typedef struct ip_address {
	u_char byte1;
	u_char byte2;
	u_char byte3;
	u_char byte4;
}ip_address;

Q_DECLARE_METATYPE(ip_address)

typedef struct ip_header {
	u_char ver_ihl;			//�汾4bit+�ײ�����4bit
	u_char tos;				//��������
	u_short tlen;			//�ܳ�
	u_short identification;	//��ʶ
	u_short flags_fo;		//��־λ3bit+��ƫ����13bit
	u_char ttl;				//���ʱ��
	u_char proto;			//Э��
	u_short crc;			//�ײ�У���
	ip_address saddr;		//Դ��ַ
	ip_address daddr;		//Ŀ�ĵ�ַ
	u_int op_pad;			//ѡ�������
}ip_header;

Q_DECLARE_METATYPE(ip_header)

typedef struct tcp_header {
	u_short sport;			//Դ�˿ں�
	u_short dport;			//Ŀ�Ķ˿ں�
	u_long sequence;		//���
	u_long acknumber;		//ȷ�����
	u_char hlen;			//�ײ�����
	u_char flags;			//��־λ
							//u_short hl_rt_flags;	//�ײ�����4bit+����6bit+��־λ6bit
	u_short window;			//���ڴ�С
	u_short crc;			//У���
	u_short urgptr;			//����ָ��
	u_int op;				//ѡ��
}tcp_header;

Q_DECLARE_METATYPE(tcp_header)

typedef struct arp_header
{
	u_short hdtyp;			//Ӳ������
	u_short protyp;			//Э������
	u_char hdsize;			//Ӳ����ַ����
	u_char prosize;			//Э���ַ����
	u_short op;				//�������ͣ�ARP����1����ARPӦ��2����RARP����3����RARPӦ��4����
	u_char smac[6];			//ԴMAC��ַ
	u_char sip[4];			//ԴIP��ַ
	u_char dmac[6];			//Ŀ��MAC��ַ
	u_char dip[4];			//Ŀ��IP��ַ
}arp_header;

Q_DECLARE_METATYPE(arp_header)

//��ʱ�洢packet������
typedef struct packet_box {
	const u_char *pkt_data;
}packet_box;







