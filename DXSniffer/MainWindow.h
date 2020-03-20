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

#define ploy 0x04C11DB7                         //标准的CRC32多项式

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

signals: //信号
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
	u_char ver_ihl;			//版本4bit+首部长度4bit
	u_char tos;				//服务类型
	u_short tlen;			//总长
	u_short identification;	//标识
	u_short flags_fo;		//标志位3bit+段偏移量13bit
	u_char ttl;				//存活时间
	u_char proto;			//协议
	u_short crc;			//首部校验和
	ip_address saddr;		//源地址
	ip_address daddr;		//目的地址
	u_int op_pad;			//选项与填充
}ip_header;

Q_DECLARE_METATYPE(ip_header)

typedef struct tcp_header {
	u_short sport;			//源端口号
	u_short dport;			//目的端口号
	u_long sequence;		//序号
	u_long acknumber;		//确认序号
	u_char hlen;			//首部长度
	u_char flags;			//标志位
							//u_short hl_rt_flags;	//首部长度4bit+保留6bit+标志位6bit
	u_short window;			//窗口大小
	u_short crc;			//校验和
	u_short urgptr;			//紧急指针
	u_int op;				//选项
}tcp_header;

Q_DECLARE_METATYPE(tcp_header)

typedef struct arp_header
{
	u_short hdtyp;			//硬件类型
	u_short protyp;			//协议类型
	u_char hdsize;			//硬件地址长度
	u_char prosize;			//协议地址长度
	u_short op;				//操作类型，ARP请求（1），ARP应答（2），RARP请求（3），RARP应答（4）。
	u_char smac[6];			//源MAC地址
	u_char sip[4];			//源IP地址
	u_char dmac[6];			//目的MAC地址
	u_char dip[4];			//目的IP地址
}arp_header;

Q_DECLARE_METATYPE(arp_header)

//暂时存储packet的数据
typedef struct packet_box {
	const u_char *pkt_data;
}packet_box;







