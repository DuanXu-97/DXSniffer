#define  HAVE_REMOTE
#include "MainWindow.h"
#include "stdio.h"

using namespace std;
#include <iostream>
#include <string>
#include <regex>
#include <bitset>

#define F_PATH "C:\\log.txt"
//#define F_PATH "C:\\NetPacketLog\\log.txt"


#pragma execution_character_set("utf-8")

FILE *fp;
pcap_if_t *alldevs;
packet_box pkt_box[500];
QRadioButton *pDevice[10];
QRadioButton *pDevice2[10];
int pkt_seq = 0;
QButtonGroup *pDeviceButtonGroup = new QButtonGroup;
QButtonGroup *pDeviceButtonGroup2 = new QButtonGroup;

uint16_t ip_checksum(uint16_t initchksum, uint8_t *ptr, int len)      //IP头部校验和
{
	unsigned int cksum;
	int idx;
	int odd;
	cksum = (unsigned int)initchksum;

	for (idx = 0; idx<len; idx += 2)
	{
		cksum += ((unsigned long)ptr[idx] << 8) + ((unsigned long)ptr[idx + 1]);
	}

	while (cksum >> 16) {
		cksum = (cksum & 0xFFFF) + (cksum >> 16);    //当高16位不为0时就将高16位与低16位反复相加，直至高16位变为0。
	}
	return cksum;
}                                           //IP首部校验和字段的计算


uint16_t tcp_chksum(uint16_t initcksum, uint8_t *tcphead, int tcplen, uint8_t *iparry)      //TCPchecksum校验和计算
{
	uint8_t pseudoheader[74];    //附加头部信息
	unsigned int calcksum = (unsigned int)initcksum;
	for (int i = 0; i<8; i++)
	{
		pseudoheader[i] = iparry[i];
	}                                               //伪首部填充源和目的IP地址
	pseudoheader[8] = 0;
	pseudoheader[9] = 6;                     //IP对应的版本号
	pseudoheader[10] = (tcplen >> 8) & 0xff;     //TCP的长度
	pseudoheader[11] = (tcplen & 0xff);
	for (int i = 12; i<74; i++)
	{
		pseudoheader[i] = tcphead[i - 12];
	}

	calcksum = ip_checksum(0, pseudoheader, sizeof(pseudoheader));  //根据伪首部和整个tcp报文进行计算
	calcksum = ~calcksum;                                         //取反
	return calcksum;
}

alt_u64 Reflect(alt_u64 ref, alt_32 ch)
{
	int i;
	alt_u64 value = 0;
	for (i = 1; i<(ch + 1); i++)
	{
		if (ref & 1)
			value |= 1 << (ch - i);
		ref >>= 1;

	}
	return value;
}

alt_u32 crc32_bit(alt_u8 *ptr, alt_u32 len, alt_u32 gx)
{
	alt_u8 i;
	alt_u32 crc = 0xffffffff;
	while (len--)
	{
		for (i = 1; i != 0; i <<= 1)
		{
			if ((crc & 0x80000000) != 0)
			{
				crc <<= 1;
				crc ^= gx;                     //做异或运算
			}
			else
				crc <<= 1;
			if ((*ptr & i) != 0)
				crc ^= gx;
		}
		ptr++;
	}
	return (Reflect(crc, 32) ^ 0xffffffff);
}


u_char ReverseBit(u_char data)
{
	data = ((data << 2) & 0x0c) | ((data >> 2) & 0x03);
	data = ((data << 1) & 0x0a) | ((data >> 1) & 0x05);
	return data;
}


MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	QObject::connect(ui.tableWidget, SIGNAL(itemDoubleClicked(QTableWidgetItem *)), this, SLOT(clickPacketDetail(QTableWidgetItem *)));
	QObject::connect(this, SIGNAL(captureSetting(int, const char *)), this, SLOT(startCapture(int, const char *)));
	QObject::connect(&cThread, SIGNAL(sendPacket(const struct pcap_pkthdr *, const u_char *)), this, SLOT(receivePacket(const struct pcap_pkthdr *, const u_char *)));
	QObject::connect(&cThread, SIGNAL(sendError(QString)), this, SLOT(errorMessage(QString)));
	QObject::connect(ui.resetBtn, SIGNAL(clicked()), this, SLOT(clickReset()));
	QObject::connect(ui.SendPacketBtn_ARP, SIGNAL(clicked()), this, SLOT(clickSendARP()));
	QObject::connect(ui.SendPacketBtn_TCP, SIGNAL(clicked()), this, SLOT(clickSendTCP()));
	QObject::connect(&sThread, SIGNAL(sendMsg(QString)), this, SLOT(receiveMsg(QString)));

	pcap_if_t *d;
	int inum;
	int i = 0;
	char errbuf[PCAP_ERRBUF_SIZE];

	fp = fopen(F_PATH, "a");

	//获取设备列表
	if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf) == -1) {
		QMessageBox::warning(NULL, "抱歉", QObject::tr("Error in pcap_findalldevs:%1").arg(errbuf));
		exit(1);
	}

	//打印设备列表
	this->printDeviceList();

	ui.ARPTypeRB_Rq->setChecked(true);
}

MainWindow::~MainWindow() {
	fclose(fp);
}


int MainWindow::printDeviceList() {
	int i = 0;
	pcap_if_t *d;

	pDeviceButtonGroup->setExclusive(true);

	for (d = alldevs; d; d = d->next, i++) {
		QString str;
		if (d->description) {
			str = QString("%1、%2 (%3)").arg(i + 1).arg(d->name).arg(d->description);
		}
		else {
			str = QString("%1、%2 (No description available)").arg(i).arg(d->name);
		}
		pDevice[i] = new QRadioButton();
		pDevice2[i] = new QRadioButton();
		pDevice[i]->setText(str);
		pDevice2[i]->setText(str);
		ui.verticalLayout_device->addWidget(pDevice[i]);
		ui.verticalLayout_device2->addWidget(pDevice2[i]);
		pDevice[i]->updateGeometry();
		pDevice2[i]->updateGeometry();
		pDeviceButtonGroup->addButton(pDevice[i]);
		pDeviceButtonGroup2->addButton(pDevice2[i]);
	}
	if (i == 0) {
		QMessageBox::warning(NULL, "抱歉", QObject::tr("没有找到接口，请确保WinPcap已安装"));
		exit(-1);

	}

	pDevice[0]->setChecked(true);
	pDevice2[0]->setChecked(true);

	connect(ui.submitBtn, SIGNAL(clicked()), this, SLOT(clickSubmit()));

	return 0;
}

void MainWindow::clickSubmit() {
	int i = 1;
	QList<QAbstractButton*> list = pDeviceButtonGroup->buttons();
	foreach(QAbstractButton *pButton, list)
	{
		if (pButton->isChecked()) {
			emit(captureSetting(i, ui.lineEdit_filter->text().toStdString().c_str()));
			ui.submitBtn->setDisabled(true);
			QMessageBox::information(NULL, "提示", QObject::tr("设置成功，开始捕获"));
			return;
		}
		i++;
	}
}

void MainWindow::clickReset() {
	QMessageBox::information(NULL, "提示", QObject::tr("成功重置"));
	cThread.stop();
	ui.submitBtn->setDisabled(false);
	ui.tableWidget->clear(); 
	ui.tableWidget->setRowCount(0);
	ui.tableWidget->setColumnCount(0);
}

void MainWindow::clickSendARP() {
	u_char packet[100];
	int i = 1;
	int dn;
	bool ok;
	char *type = "ARP";//发送ARP
	QList<QAbstractButton*> list = pDeviceButtonGroup2->buttons();
	foreach(QAbstractButton *pButton, list)
	{
		if (pButton->isChecked()) {
			break;
		}
		i++;
	}
	dn = i;//设备编号

		   //填充ARP类型
	if (ui.ARPTypeRB_Rq->isChecked()) {
		packet[21] = 1;    //代表当前的数据包是请求包还是应答包
	}
	else if (ui.ARPTypeRB_Rsp->isChecked()) {
		packet[21] = 2;
	}

	//填充目的MAC地址
	packet[0] = ui.lineEdit->text().toInt(&ok, 16);
	packet[1] = ui.lineEdit_2->text().toInt(&ok, 16);
	packet[2] = ui.lineEdit_3->text().toInt(&ok, 16);
	packet[3] = ui.lineEdit_4->text().toInt(&ok, 16);
	packet[4] = ui.lineEdit_5->text().toInt(&ok, 16);
	packet[5] = ui.lineEdit_6->text().toInt(&ok, 16);

	//填充源MAC地址
	packet[6] = ui.lineEdit_7->text().toInt(&ok, 16);
	packet[7] = ui.lineEdit_8->text().toInt(&ok, 16);
	packet[8] = ui.lineEdit_9->text().toInt(&ok, 16);
	packet[9] = ui.lineEdit_10->text().toInt(&ok, 16);
	packet[10] = ui.lineEdit_11->text().toInt(&ok, 16);
	packet[11] = ui.lineEdit_12->text().toInt(&ok, 16);    //源MAC地址

	packet[12] = 8;
	packet[13] = 6;    //0x0806代表ARP数据包;
	packet[14] = 0;
	packet[15] = 1;    //0x0001代表硬件类型,1代表以太网地址
	packet[16] = 8;
	packet[17] = 0;   //0x0800代表是映射的协议类型是IP地址
	packet[18] = 6;   //硬件地址长度
	packet[19] = 4;   //协议地址长度
	packet[20] = 0;

	packet[22] = packet[6];
	packet[23] = packet[7];
	packet[24] = packet[8];
	packet[25] = packet[9];
	packet[26] = packet[10];
	packet[27] = packet[11];    //紧接着的6个字节代表源MAC地址

								/*源IP地址*/                //  这里根据调试的主机更改源IP和目的IP
	packet[28] = ui.lineEdit_17->text().toInt();
	packet[29] = ui.lineEdit_18->text().toInt();
	packet[30] = ui.lineEdit_19->text().toInt();
	packet[31] = ui.lineEdit_20->text().toInt();

	//希望得到的MAC地址
	packet[32] = ui.lineEdit_43->text().toInt();
	packet[33] = ui.lineEdit_44->text().toInt();
	packet[34] = ui.lineEdit_45->text().toInt();
	packet[35] = ui.lineEdit_46->text().toInt();
	packet[36] = ui.lineEdit_47->text().toInt();
	packet[37] = ui.lineEdit_48->text().toInt();

	/*目的IP地址*/
	packet[38] = ui.lineEdit_13->text().toInt();
	packet[39] = ui.lineEdit_14->text().toInt();
	packet[40] = ui.lineEdit_15->text().toInt();
	packet[41] = ui.lineEdit_16->text().toInt();

	for (i = 42; i < 96; i++)
	{
		packet[i] = 0;
	}             //后面数据的封装

	alt_u32 fcs;
	alt_u8 *data = packet;
	alt_u32 dataLen = sizeof(packet) - 4;
	fcs = crc32_bit(data, dataLen, ploy);     //按位进行crc校验
	packet[96] = packet[97] = packet[98] = packet[99] = 0xff;
	packet[96] &= fcs;
	packet[97] &= fcs >> 8;
	packet[98] &= fcs >> 16;
	packet[99] &= fcs >> 24;

	sThread.setInfo(dn, alldevs, type, packet);
	sThread.start();
}

void MainWindow::clickSendTCP() {
	u_char packet[100];
	int i = 1;
	int dn;
	bool ok;
	char *type = "TCP";//发送ARP
	QList<QAbstractButton*> list = pDeviceButtonGroup2->buttons();
	foreach(QAbstractButton *pButton, list)
	{
		if (pButton->isChecked()) {
			break;
		}
		i++;
	}
	dn = i;//设备编号

		   //填充目的MAC地址
	packet[0] = ui.lineEdit_21->text().toInt(&ok, 16);
	packet[1] = ui.lineEdit_22->text().toInt(&ok, 16);
	packet[2] = ui.lineEdit_23->text().toInt(&ok, 16);
	packet[3] = ui.lineEdit_24->text().toInt(&ok, 16);
	packet[4] = ui.lineEdit_25->text().toInt(&ok, 16);
	packet[5] = ui.lineEdit_26->text().toInt(&ok, 16);

	//填充源MAC地址
	packet[6] = ui.lineEdit_27->text().toInt(&ok, 16);
	packet[7] = ui.lineEdit_28->text().toInt(&ok, 16);
	packet[8] = ui.lineEdit_29->text().toInt(&ok, 16);
	packet[9] = ui.lineEdit_30->text().toInt(&ok, 16);
	packet[10] = ui.lineEdit_31->text().toInt(&ok, 16);
	packet[11] = ui.lineEdit_32->text().toInt(&ok, 16);

	/*填充MAC帧类型*/
	packet[12] = 8;
	packet[13] = 0;

	/*填充IP首部部分*/
	packet[14] = 0x45;//版本+首部长度（0x45）
	packet[15] = 0x00;//区分服务（0x00）
	packet[16] = 0x00;//总长度前一个字节部分（0x00）
	packet[17] = 0x52;//总长度后一个字节部分（0x05）
	packet[18] = 0x00;//标识前一个字节部分（0x00）
	packet[19] = 0x00;//标识后一个字节部分（0x00）
	packet[20] = 0x00;//标志和片偏移的前5bit（0x00）
	packet[21] = 0x00;//片偏移的后8bit(0x00)
	packet[22] = 0x10;//生存时间（0x10）
	packet[23] = 0x06;//协议（0x06），选择TCP协议
	packet[24] = 0x00;//首部校验和的前一字节（0x00）
	packet[25] = 0x00;//首部校验和的后一字节（0xa0）

					  /*源IP地址*/
	packet[26] = ui.lineEdit_37->text().toInt(&ok, 10);
	packet[27] = ui.lineEdit_38->text().toInt(&ok, 10);
	packet[28] = ui.lineEdit_39->text().toInt(&ok, 10);
	packet[29] = ui.lineEdit_40->text().toInt(&ok, 10);
	/*目的IP地址*/
	packet[30] = ui.lineEdit_33->text().toInt(&ok, 10);
	packet[31] = ui.lineEdit_34->text().toInt(&ok, 10);
	packet[32] = ui.lineEdit_35->text().toInt(&ok, 10);
	packet[33] = ui.lineEdit_36->text().toInt(&ok, 10);

	alt_u8 *data_ip = packet;
	data_ip = data_ip + 14;
	int len = 20;
	uint16_t result;
	result = ~ip_checksum(0, data_ip, len);

	packet[24] = packet[25] = 0xff;
	packet[25] &= result;
	packet[24] &= result >> 8;    //计算首部校验和字段


	u_short sPort = ui.lineEdit_42->text().toInt(&ok, 10);
	u_short dPort = ui.lineEdit_41->text().toInt(&ok, 10);
	packet[34] = (sPort & 0xFF00) >> 8;
	packet[35] = sPort & 0x00FF;
	packet[36] = (dPort & 0xFF00) >> 8;
	packet[37] = dPort & 0x00FF;

	u_long seq = ui.lineEdit_49->text().toInt(&ok, 10);
	u_long akSeq = ui.lineEdit_50->text().toInt(&ok, 10);

	packet[38] = (seq & 0xFF000000) >> 24;
	packet[39] = (seq & 0x00FF0000) >> 16;
	packet[40] = (seq & 0x0000FF00) >> 8;
	packet[41] = (seq & 0x000000FF);   //序号字段
	packet[42] = (akSeq & 0xFF000000) >> 24;
	packet[43] = (akSeq & 0x00FF0000) >> 16;
	packet[44] = (akSeq & 0x0000FF00) >> 8;
	packet[45] = (akSeq & 0x000000FF);   //确认序号字段

	packet[46] = 0x50;
	packet[47] = 0x10;  //标志位以及数据偏移

	u_short win = ui.lineEdit_51->text().toInt(&ok, 10);

	packet[48] = (win & 0xFF00) >> 8;
	packet[49] = win & 0x00FF;  //窗口字段

	packet[50] = 0x00;
	packet[51] = 0x00;  //校验和

	u_short urptr = ui.lineEdit_52->text().toInt(&ok, 10);

	packet[52] = (urptr & 0xFF00) >> 8;
	packet[53] = urptr & 0x00FF;  //urgent pointer 紧急指针

								  /* 填充剩下的内容 */
	for (i = 54; i < 96; i++)
	{
		packet[i] = 0;
	}

	/* 计算TCP校验和*/
	alt_u8 iparry[8];
	for (int i = 26; i<34; i++)
	{
		iparry[i - 26] = packet[i];        //将源和目的IP地址封装一下传到伪首部。
	}

	alt_u8 *data_tcp = packet;
	data_tcp = data_tcp + 34;
	int tcplen = 62;                 //TCP头部和数据包的总长度62
	uint16_t result_tcp;
	result_tcp = tcp_chksum(0, data_tcp, tcplen, iparry);
	packet[51] = result_tcp & 0xff;
	packet[50] = (result_tcp >> 8) & 0xff;    //计算首部校验和字段
											  /* 计算TCP校验和*/

	alt_u32 fcs;
	/* 计算以太网帧的FCS字段*/
	alt_u8 *data = packet;            //计算以太网帧FCS字段
	alt_u32 dataLen = sizeof(packet) - 4;
	fcs = crc32_bit(data, dataLen, ploy);
	packet[96] = packet[97] = packet[98] = packet[99] = 0xff;
	packet[96] &= fcs;
	packet[97] &= fcs >> 8;
	packet[98] &= fcs >> 16;
	packet[99] &= fcs >> 24;
	/* 计算以太网帧的FCS字段*/


	sThread.setInfo(dn, alldevs, type, packet);
	sThread.start();
}

void MainWindow::startCapture(int dn, const char *ft) {
	cThread.setDevice(dn, alldevs);
	cThread.setFilter(ft);
	cThread.start();
}


void MainWindow::clickPacketDetail(QTableWidgetItem *pktItem) {
	QString str;
	QString pktStr;

	//获取item中协议类型
	QVariant variant_type = pktItem->data(Qt::UserRole);
	u_short pkt_type = variant_type.value<u_short>();

	//获取item中完整数据包
	QVariant variant_pkt = pktItem->data(Qt::UserRole + 1);
	Ethernet_pkt pkt_data = variant_pkt.value<Ethernet_pkt>();

	//获取item中完整数据包长度
	int pkt_len = pktItem->data(Qt::UserRole + 2).toInt();

	for (int i = 0; i < pkt_len; i++) {
		bitset<8> tempBitset(pkt_data.byte[i]);
		bool flag = tempBitset.test(3);
		pktStr.append(str.sprintf("%02x ", pkt_data.byte[i]));
	}

	Dialog *pDialog = new Dialog(this);
	pDialog->setWindowTitle(QString("数据包详情"));
	pDialog->setFixedSize(600, 800);
	QGroupBox *pkt_box = new QGroupBox();

	QVBoxLayout *pVLayout_Dialog = new QVBoxLayout(pDialog);
	QVBoxLayout *pVLayout_pktgb = new QVBoxLayout(pkt_box);


	pkt_box->setTitle("原始数据包");

	switch (pkt_type) {
	case 2054:
	{
		QVariant variant_arph = pktItem->data(Qt::UserRole + 3);
		arp_header arph = variant_arph.value<arp_header>();

		QGroupBox *arp_box = new QGroupBox();
		QVBoxLayout *pVLayout_arpgb = new QVBoxLayout(arp_box);

		QListWidget *plw_arpDetail = new QListWidget(arp_box);

		arp_box->setTitle("ARP");

		pVLayout_Dialog->addWidget(arp_box);

		pVLayout_arpgb->addWidget(plw_arpDetail);

		QListWidgetItem *pHwType = new QListWidgetItem(plw_arpDetail);
		QListWidgetItem *pPtoType = new QListWidgetItem(plw_arpDetail);
		QListWidgetItem *pHwSize = new QListWidgetItem(plw_arpDetail);
		QListWidgetItem *pPtoSize = new QListWidgetItem(plw_arpDetail);
		QListWidgetItem *pOpcode = new QListWidgetItem(plw_arpDetail);
		QListWidgetItem *pSmac = new QListWidgetItem(plw_arpDetail);
		QListWidgetItem *pSip = new QListWidgetItem(plw_arpDetail);
		QListWidgetItem *pDmac = new QListWidgetItem(plw_arpDetail);
		QListWidgetItem *pDip = new QListWidgetItem(plw_arpDetail);

		pHwType->setText(str.sprintf("硬件类型：Ethernet (1)"));
		pPtoType->setText(str.sprintf("协议类型：%s (0x%04x)", ntohs(arph.protyp) == 2048 ? "IPv4" : "else", ntohs(arph.protyp)));
		pHwSize->setText(str.sprintf("硬件地址长度：%d", arph.hdsize));
		pPtoSize->setText(str.sprintf("协议地址长度：%d", arph.prosize));
		pOpcode->setText(str.sprintf("操作类型：%s", arph.op == 256 ? "request" : "response"));
		pSmac->setText(str.sprintf("源MAC地址：%02x-%02x-%02x-%02x-%02x-%02x",
			arph.smac[0],
			arph.smac[1],
			arph.smac[2],
			arph.smac[3],
			arph.smac[4],
			arph.smac[5]
			));
		pSip->setText(str.sprintf("源IP地址：%d.%d.%d.%d",
			arph.sip[0],
			arph.sip[1],
			arph.sip[2],
			arph.sip[3]
			));
		pDmac->setText(str.sprintf("目的MAC地址：%02x-%02x-%02x-%02x-%02x-%02x",
			arph.dmac[0],
			arph.dmac[1],
			arph.dmac[2],
			arph.dmac[3],
			arph.dmac[4],
			arph.dmac[5]
			));
		pDip->setText(str.sprintf("目的IP地址：%d.%d.%d.%d",
			arph.dip[0],
			arph.dip[1],
			arph.dip[2],
			arph.dip[3]
			));

		break;
	}
	case 2048:
	{
		QVariant variant_ih = pktItem->data(Qt::UserRole + 3);
		ip_header ih = variant_ih.value<ip_header>();

		switch (ih.proto) {
		case 1:
		{
			QVariant variant_icmph = pktItem->data(Qt::UserRole + 6);
			icmp_header icmph = variant_icmph.value<icmp_header>();

			QGroupBox *icmp_box = new QGroupBox();
			QVBoxLayout *pVLayout_icmpgb = new QVBoxLayout(icmp_box);
			QListWidget *plw_icmpDetail = new QListWidget(icmp_box);
			icmp_box->setTitle("ICMP");
			pVLayout_Dialog->addWidget(icmp_box);
			pVLayout_icmpgb->addWidget(plw_icmpDetail);

			QListWidgetItem *pType = new QListWidgetItem(plw_icmpDetail);
			QListWidgetItem *pCode = new QListWidgetItem(plw_icmpDetail);
			QListWidgetItem *pCrc = new QListWidgetItem(plw_icmpDetail);
			QListWidgetItem *pIdt = new QListWidgetItem(plw_icmpDetail);
			QListWidgetItem *pSeq = new QListWidgetItem(plw_icmpDetail);

			pType->setText(str.sprintf("类型：%d", icmph.type));
			pCode->setText(str.sprintf("代码：%d", ntohs(icmph.code)));
			pCrc->setText(str.sprintf("校验和：0x%04x", ntohs(icmph.crc)));
			pIdt->setText(str.sprintf("标识符：%d", ntohs(icmph.idt)));
			pSeq->setText(str.sprintf("序列号：%d", ntohs(icmph.seq)));

			setLayout(pVLayout_icmpgb);

			break;
		}
		case 2:
		{
			QVariant variant_igmph = pktItem->data(Qt::UserRole + 7);
			igmp_header igmph = variant_igmph.value<igmp_header>();

			QGroupBox *igmp_box = new QGroupBox();
			QVBoxLayout *pVLayout_igmpgb = new QVBoxLayout(igmp_box);
			QListWidget *plw_igmpDetail = new QListWidget(igmp_box);
			igmp_box->setTitle("IGMP");
			pVLayout_Dialog->addWidget(igmp_box);
			pVLayout_igmpgb->addWidget(plw_igmpDetail);

			QListWidgetItem *pType = new QListWidgetItem(plw_igmpDetail);
			QListWidgetItem *pMrt = new QListWidgetItem(plw_igmpDetail);
			QListWidgetItem *pCrc = new QListWidgetItem(plw_igmpDetail);
			QListWidgetItem *pGroupaddr = new QListWidgetItem(plw_igmpDetail);

			pType->setText(str.sprintf("类型：0x%02x", igmph.type));
			pMrt->setText(str.sprintf("最大响应时间：0x%02x", igmph.mrt));
			pCrc->setText(str.sprintf("校验和：0x%04x", ntohs(igmph.crc)));
			pGroupaddr->setText(str.sprintf("组播地址：%d.%d.%d.%d",
				igmph.groupaddr.byte1,
				igmph.groupaddr.byte2,
				igmph.groupaddr.byte3,
				igmph.groupaddr.byte4
				));

			setLayout(pVLayout_igmpgb);

			break;
		}
		case 6:
		{

			QVariant variant_th = pktItem->data(Qt::UserRole + 4);
			tcp_header th = variant_th.value<tcp_header>();

			QGroupBox *tcp_box = new QGroupBox();
			QVBoxLayout *pVLayout_tcpgb = new QVBoxLayout(tcp_box);
			QListWidget *plw_tcpDetail = new QListWidget(tcp_box);
			tcp_box->setTitle("TCP");
			pVLayout_Dialog->addWidget(tcp_box);
			pVLayout_tcpgb->addWidget(plw_tcpDetail);

			QListWidgetItem *pSport = new QListWidgetItem(plw_tcpDetail);
			QListWidgetItem *pDport = new QListWidgetItem(plw_tcpDetail);
			QListWidgetItem *pSequence = new QListWidgetItem(plw_tcpDetail);
			QListWidgetItem *pAcknumber = new QListWidgetItem(plw_tcpDetail);
			QListWidgetItem *pHlen = new QListWidgetItem(plw_tcpDetail);
			QListWidgetItem *pFlags = new QListWidgetItem(plw_tcpDetail);
			QListWidgetItem *pWindow = new QListWidgetItem(plw_tcpDetail);
			QListWidgetItem *pCrc = new QListWidgetItem(plw_tcpDetail);
			QListWidgetItem *pUrgptr = new QListWidgetItem(plw_tcpDetail);
			QListWidgetItem *pOp = new QListWidgetItem(plw_tcpDetail);

			pSport->setText(str.sprintf("源端口号：%d", ntohs(th.sport)));
			pDport->setText(str.sprintf("目的端口号：%d", ntohs(th.dport)));
			pSequence->setText(str.sprintf("序号：%u", ntohl(th.sequence)));
			pAcknumber->setText(str.sprintf("确认序号：%u", ntohl(th.acknumber)));
			pHlen->setText(str.sprintf("头部长度：%d bytes", th.hlen * 4));
			pFlags->setText(str.sprintf("标志位：0x%03x", th.flags));
			pWindow->setText(str.sprintf("窗口大小：%d", ntohs(th.window)));
			pCrc->setText(str.sprintf("校验和：0x%04x", ntohs(th.crc)));
			pUrgptr->setText(str.sprintf("紧急指针：%x", th.urgptr));
			pOp->setText(str.sprintf("选项：0x%x", th.op));

			int th_len = ((th.hlen) * 4);
			int ip_len = (ih.ver_ihl & 0xf) * 4;
			int data_len = pkt_len - 14 - ip_len - th_len;
			QString pktStr;
			QString temp;
			for (int i = 14 + ip_len + th_len; i < data_len; i++) {
				pktStr.append(temp.sprintf("%c", pkt_data.byte[i]));
			}
			std::cmatch cm;
			// 判断是否为HTTP协议
			if (std::regex_search(pktStr.toStdString().c_str(), cm, std::regex("^(GET|POST|PUT|DELETE|HEAD|TRACE|OPTIONS|CONNECT) .* HTTP/(1\.1|1\.0)")) ||
				std::regex_search(pktStr.toStdString().c_str(), cm, std::regex("^HTTP/(1\.1|1\.0) [0-9]{3} "))) {
				QString tempStr;
				int flag = 0;
				string _r = "0d";
				string _n = "0a";
				int i;
				for (i = 14 + ip_len + th_len; i < data_len; i++) {
					if (temp.sprintf("%02x", pkt_data.byte[i]).toStdString() == _r && temp.sprintf("%02x", pkt_data.byte[i+1]).toStdString() == _n) {
						QListWidgetItem *pItem = new QListWidgetItem(plw_tcpDetail);
						pItem->setText(tempStr);
						tempStr.clear();
						if (flag == 1) {
							break;
						}
						else {
							flag = 1;
							i++;
						}
					}
					else {
						tempStr.append(temp.sprintf("%c", pkt_data.byte[i]));
						flag = 0;
					}
				}

				QString contentStr;
				for (int j = i; j < data_len; j++) {
					bitset<8> tempBitset(pkt_data.byte[j]);
					bool flag = tempBitset.test(7);
					if (!flag) {
						contentStr.append(temp.sprintf("%c", pkt_data.byte[j]));
					}
					else {
						contentStr.append(QString("."));
					}
				}

				QGroupBox *http_box = new QGroupBox();
				QVBoxLayout *pVLayout_httpgb = new QVBoxLayout(http_box);
				http_box->setTitle("HTTP内容");
				pVLayout_Dialog->addWidget(http_box);
				if (contentStr.length() > 300) {
					QLabel *pPkt_data = new QLabel(http_box);
					pPkt_data->setText(contentStr);

					QScrollArea *pkt_sa = new QScrollArea();
					pkt_sa->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
					pkt_sa->setWidget(pPkt_data);
					pPkt_data->setWordWrap(true);
					pPkt_data->adjustSize();
					pVLayout_httpgb->addWidget(pkt_sa);
				}
				else {
					QLabel *pPkt_data = new QLabel(http_box);
					pPkt_data->setText(contentStr);
					pPkt_data->setWordWrap(true);
					pPkt_data->adjustSize();
					pVLayout_httpgb->addWidget(pPkt_data);
				}
				setLayout(pVLayout_httpgb);
			}
			setLayout(pVLayout_tcpgb);

			break;
		}
		case 17:
		{
			QVariant variant_uh = pktItem->data(Qt::UserRole + 5);
			udp_header uh = variant_uh.value<udp_header>();

			QGroupBox *udp_box = new QGroupBox();
			QVBoxLayout *pVLayout_udpgb = new QVBoxLayout(udp_box);
			QListWidget *plw_udpDetail = new QListWidget(udp_box);
			udp_box->setTitle("UDP");
			pVLayout_Dialog->addWidget(udp_box);
			pVLayout_udpgb->addWidget(plw_udpDetail);

			QListWidgetItem *pSport = new QListWidgetItem(plw_udpDetail);
			QListWidgetItem *pDport = new QListWidgetItem(plw_udpDetail);
			QListWidgetItem *plen = new QListWidgetItem(plw_udpDetail);
			QListWidgetItem *pCrc = new QListWidgetItem(plw_udpDetail);

			pSport->setText(str.sprintf("源端口号：%d", ntohs(uh.sport)));
			pDport->setText(str.sprintf("目的端口号：%d", ntohs(uh.dport)));
			plen->setText(str.sprintf("长度：%d", ntohs(uh.len)));
			pCrc->setText(str.sprintf("校验和：0x%04x", ntohs(uh.crc)));

			setLayout(pVLayout_udpgb);

			break;
		}
		default:
			break;
		}

		break;
	}
	default:
		break;
	}

	pVLayout_Dialog->addWidget(pkt_box);

	if (pkt_len > 300) {
		QLabel *pPkt_data = new QLabel(pkt_box);
		pPkt_data->setText(pktStr);

		QScrollArea *pkt_sa = new QScrollArea();
		pkt_sa->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		pkt_sa->setWidget(pPkt_data);
		pPkt_data->setWordWrap(true);
		pPkt_data->adjustSize();
		pVLayout_pktgb->addWidget(pkt_sa);
	}
	else {
		QLabel *pPkt_data = new QLabel(pkt_box);
		pPkt_data->setText(pktStr);
		pPkt_data->setWordWrap(true);
		pPkt_data->adjustSize();
		pVLayout_pktgb->addWidget(pPkt_data);
	}

	setLayout(pVLayout_Dialog);
	setLayout(pVLayout_pktgb);

	pDialog->show();

}



void MainWindow::errorMessage(QString msg) {
	QMessageBox::warning(NULL, "抱歉", msg);
}

void MainWindow::receiveMsg(QString msg) {
	QMessageBox::information(NULL, "提示", msg);
}

void MainWindow::receivePacket(const struct pcap_pkthdr* header, const u_char* pkt_data) {
	struct tm ltime;
	char timestr[16];

	arp_header *arph;	//指向arp头部
	ip_header *ih;		//指向ip头部
	tcp_header *th;		//指向tcp头部
	udp_header *uh;		//指向udp头部
	icmp_header *icmph;		//指向icmp头部
	igmp_header *igmph;		//指向igmp头部

	Ethernet_pkt *fpkt;		//指向整个数据包
	u_int ip_len;
	u_short *pkt_type;
	u_short sport, dport;
	time_t local_tv_sec;
	char *tempStr;
	QString str;
	QString str1;
	QString str2;
	QString str3;
	QString str4;
	QString str5;


	//将时间戳转换为可识别的格式
	local_tv_sec = header->ts.tv_sec;
	localtime_s(&ltime, &local_tv_sec);

	//打印时间
	str1 = str1.sprintf("%d-%d-%d %d:%d:%d\t", ltime.tm_year + 1900, ltime.tm_mon + 1,
		ltime.tm_mday, ltime.tm_hour, ltime.tm_min, ltime.tm_sec);

	fprintf(fp, "[pkt]%d-%d-%d %d:%d:%d   ", ltime.tm_year + 1900, ltime.tm_mon + 1,
		ltime.tm_mday, ltime.tm_hour, ltime.tm_min, ltime.tm_sec);

	//获取整个数据包
	fpkt = (Ethernet_pkt*)(pkt_data);

	//获取以太网帧头中协议类型
	pkt_type = (u_short*)(pkt_data + 12);

	ui.tableWidget->setShowGrid(false);
	ui.tableWidget->setColumnCount(5);

	//设置表头内容
	QStringList listHeader;
	listHeader << tr("时间") << tr("源地址") << tr("目的地址") << tr("长度") << tr("协议类型");
	ui.tableWidget->setHorizontalHeaderLabels(listHeader);

	//表头字体加粗
	QFont font = ui.tableWidget->horizontalHeader()->font();
	font.setBold(true);
	ui.tableWidget->horizontalHeader()->setFont(font);

	ui.tableWidget->horizontalHeader()->resizeSection(0, 160); //设置表头第一列的宽度为170
	ui.tableWidget->horizontalHeader()->resizeSection(1, 160);
	ui.tableWidget->horizontalHeader()->resizeSection(2, 160);

	ui.tableWidget->setFrameShape(QFrame::NoFrame); //设置无边框
	ui.tableWidget->setShowGrid(false); //设置不显示格子线
	ui.tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);  //设置选择行为时每次选择一行
	ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置不可编辑

	/*ARP协议为2054(0x0806的十进制)
	IP协议为2048(0x0800的十进制)*/
	switch (ntohs(*pkt_type)) {
	case 2054:
	{
		//获得ARP数据包头部的位置
		arph = (arp_header *)(pkt_data + 14);

		u_short *dMac[3];
		dMac[0] = (u_short*)(pkt_data);
		dMac[1] = (u_short*)(pkt_data + 2);
		dMac[2] = (u_short*)(pkt_data + 4);

		if (*dMac[0] == 65535 && *dMac[1] == 65535 && *dMac[2] == 65535) {
			str2 = str2.sprintf("%02x-%02x-%02x-%02x-%02x-%02x",
				arph->smac[0],
				arph->smac[1],
				arph->smac[2],
				arph->smac[3],
				arph->smac[4],
				arph->smac[5]
				);
			str3 = str3.sprintf("Broadcast");

			//写入日志

			fprintf(fp, "[pkt-sMac]%02x-%02x-%02x-%02x-%02x-%02x   ",
				arph->smac[0],
				arph->smac[1],
				arph->smac[2],
				arph->smac[3],
				arph->smac[4],
				arph->smac[5]);

			fprintf(fp, "[pkt-dMac]Broadcast   ");
		}
		else {
			str2 = str2.sprintf("%02x-%02x-%02x-%02x-%02x-%02x",
				arph->smac[0],
				arph->smac[1],
				arph->smac[2],
				arph->smac[3],
				arph->smac[4],
				arph->smac[5]
				);

			fprintf(fp, "[pkt-sMac]%02x-%02x-%02x-%02x-%02x-%02x   ",
				arph->smac[0],
				arph->smac[1],
				arph->smac[2],
				arph->smac[3],
				arph->smac[4],
				arph->smac[5]);

			str3 = str3.sprintf("%02x-%02x-%02x-%02x-%02x-%02x",
				arph->dmac[0],
				arph->dmac[1],
				arph->dmac[2],
				arph->dmac[3],
				arph->dmac[4],
				arph->dmac[5]
				);

			fprintf(fp, "[pkt-dMac]%02x-%02x-%02x-%02x-%02x-%02x   ",
				arph->dmac[0],
				arph->dmac[1],
				arph->dmac[2],
				arph->dmac[3],
				arph->dmac[4],
				arph->dmac[5]);
		}

		//打印数据包的长度
		str4 = str4.sprintf("len:%d\t", header->len);

		fprintf(fp, "[pkt-len]len:%d   ", header->len);

		str5 = str5.sprintf("ARP");

		fprintf(fp, "[pkt-proto]ARP\n");

		QTableWidgetItem *item1 = new QTableWidgetItem;
		QTableWidgetItem *item2 = new QTableWidgetItem;
		QTableWidgetItem *item3 = new QTableWidgetItem;
		QTableWidgetItem *item4 = new QTableWidgetItem;
		QTableWidgetItem *item5 = new QTableWidgetItem;

		item1->setText(str1);
		item2->setText(str2);
		item3->setText(str3);
		item4->setText(str4);
		item5->setText(str5);

		int row_count = ui.tableWidget->rowCount(); //获取表单行数
		ui.tableWidget->insertRow(row_count); //插入新行
		ui.tableWidget->setItem(row_count, 0, item1);
		ui.tableWidget->setItem(row_count, 1, item2);
		ui.tableWidget->setItem(row_count, 2, item3);
		ui.tableWidget->setItem(row_count, 3, item4);
		ui.tableWidget->setItem(row_count, 4, item5);

		QList<QTableWidgetItem *> List;
		List.append(item1);
		List.append(item2);
		List.append(item3);
		List.append(item4);
		List.append(item5);

		foreach(QTableWidgetItem *pitem, List)
		{
			pitem->setData(Qt::UserRole, QVariant::fromValue(ntohs(*pkt_type)));//item绑定以太网帧头协议类型
			pitem->setData(Qt::UserRole + 1, QVariant::fromValue(*fpkt));//item绑定整个数据包
			pitem->setData(Qt::UserRole + 2, header->len);//item绑定数据包长度
			pitem->setData(Qt::UserRole + 3, QVariant::fromValue(*arph));//item绑定ARP头
		}

		break;
	}
	case 2048:
	{
		//获取IP数据包头部的位置
		ih = (ip_header*)(pkt_data + 14);//14为以太网帧头部长度

		//获得TCP数据包头部的位置
		ip_len = (ih->ver_ihl & 0xf) * 4;
		th = (tcp_header*)((u_char*)ih + ip_len);
		uh = (udp_header*)((u_char*)ih + ip_len);
		icmph = (icmp_header*)((u_char*)ih + ip_len);
		igmph = (igmp_header*)((u_char*)ih + ip_len);

		//打印IP地址和TCP端口
		str2 = str2.sprintf("%d.%d.%d.%d",
			ih->saddr.byte1,
			ih->saddr.byte2,
			ih->saddr.byte3,
			ih->saddr.byte4
			);

		fprintf(fp, "[pkt-sIP]%d.%d.%d.%d   ",
			ih->saddr.byte1,
			ih->saddr.byte2,
			ih->saddr.byte3,
			ih->saddr.byte4
			);

		str3 = str3.sprintf("%d.%d.%d.%d",
			ih->daddr.byte1,
			ih->daddr.byte2,
			ih->daddr.byte3,
			ih->daddr.byte4
			);

		fprintf(fp, "[pkt-dIP]%d.%d.%d.%d   ",
			ih->daddr.byte1,
			ih->daddr.byte2,
			ih->daddr.byte3,
			ih->daddr.byte4
			);

		//打印数据包的长度
		str4 = str4.sprintf("len:%d", header->len);

		fprintf(fp, "[pkt-len]len:%d   ", header->len);

		switch (ih->proto) {
		case 1:
			str5 = str5.sprintf("ICMP");
			fprintf(fp, "[pkt-proto]ICMP\n");
			break;
		case 2:
			str5 = str5.sprintf("IGMP");
			fprintf(fp, "[pkt-proto]IGMP\n");
			break;
		case 6:
		{
			int th_len = ((th->hlen) * 4);
			int data_len = (int)(header->len) - 14 - ip_len - th_len;
			QString pktStr;
			QString temp;
			for (int i = 14 + ip_len + th_len; i < data_len; i++) {
				pktStr.append(temp.sprintf("%c", fpkt->byte[i]));
			}
			std::cmatch cm;
			if (std::regex_search(pktStr.toStdString().c_str(), cm, std::regex("^(GET|POST|PUT|DELETE|HEAD|TRACE|OPTIONS|CONNECT) .* HTTP/(1\.1|1\.0)")) ||
				std::regex_search(pktStr.toStdString().c_str(), cm, std::regex("^HTTP/(1\.1|1\.0) [0-9]{3} "))) {
				str5 = str5.sprintf("HTTP");
				fprintf(fp, "[pkt-proto]HTTP\n");
			}
			else {
				str5 = str5.sprintf("TCP");
				fprintf(fp, "[pkt-proto]TCP\n");
			}
			break;
		}
		case 17:
			str5 = str5.sprintf("UDP");
			fprintf(fp, "[pkt-proto]UDP\n");
			break;
		default:
			str5 = str5.sprintf("ELSE");
			break;
		}

		QTableWidgetItem *item1 = new QTableWidgetItem;
		QTableWidgetItem *item2 = new QTableWidgetItem;
		QTableWidgetItem *item3 = new QTableWidgetItem;
		QTableWidgetItem *item4 = new QTableWidgetItem;
		QTableWidgetItem *item5 = new QTableWidgetItem;

		item1->setText(str1);
		item2->setText(str2);
		item3->setText(str3);
		item4->setText(str4);
		item5->setText(str5);

		int row_count = ui.tableWidget->rowCount(); //获取表单行数
		ui.tableWidget->insertRow(row_count); //插入新行
		ui.tableWidget->setItem(row_count, 0, item1);
		ui.tableWidget->setItem(row_count, 1, item2);
		ui.tableWidget->setItem(row_count, 2, item3);
		ui.tableWidget->setItem(row_count, 3, item4);
		ui.tableWidget->setItem(row_count, 4, item5);

		QList<QTableWidgetItem *> List;
		List.append(item1);
		List.append(item2);
		List.append(item3);
		List.append(item4);
		List.append(item5);

		foreach(QTableWidgetItem *pitem, List)
		{
			pitem->setData(Qt::UserRole, QVariant::fromValue(ntohs(*pkt_type)));//item绑定以太网帧头协议类型
			pitem->setData(Qt::UserRole + 1, QVariant::fromValue(*fpkt));//item绑定整个数据包
			pitem->setData(Qt::UserRole + 2, header->len);//item绑定数据包长度
			pitem->setData(Qt::UserRole + 3, QVariant::fromValue(*ih));//item绑定IP头
			pitem->setData(Qt::UserRole + 4, QVariant::fromValue(*th));//item绑定TCP头
			pitem->setData(Qt::UserRole + 5, QVariant::fromValue(*uh));//item绑定UDP头
			pitem->setData(Qt::UserRole + 6, QVariant::fromValue(*icmph));//item绑定ICMP头
			pitem->setData(Qt::UserRole + 7, QVariant::fromValue(*igmph));//item绑定IGMP头
		}

		break;
	}
	default:
		break;
	}

}

