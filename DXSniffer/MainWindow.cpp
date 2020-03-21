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

uint16_t ip_checksum(uint16_t initchksum, uint8_t *ptr, int len)      //IPͷ��У���
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
		cksum = (cksum & 0xFFFF) + (cksum >> 16);    //����16λ��Ϊ0ʱ�ͽ���16λ���16λ������ӣ�ֱ����16λ��Ϊ0��
	}
	return cksum;
}                                           //IP�ײ�У����ֶεļ���


uint16_t tcp_chksum(uint16_t initcksum, uint8_t *tcphead, int tcplen, uint8_t *iparry)      //TCPchecksumУ��ͼ���
{
	uint8_t pseudoheader[74];    //����ͷ����Ϣ
	unsigned int calcksum = (unsigned int)initcksum;
	for (int i = 0; i<8; i++)
	{
		pseudoheader[i] = iparry[i];
	}                                               //α�ײ����Դ��Ŀ��IP��ַ
	pseudoheader[8] = 0;
	pseudoheader[9] = 6;                     //IP��Ӧ�İ汾��
	pseudoheader[10] = (tcplen >> 8) & 0xff;     //TCP�ĳ���
	pseudoheader[11] = (tcplen & 0xff);
	for (int i = 12; i<74; i++)
	{
		pseudoheader[i] = tcphead[i - 12];
	}

	calcksum = ip_checksum(0, pseudoheader, sizeof(pseudoheader));  //����α�ײ�������tcp���Ľ��м���
	calcksum = ~calcksum;                                         //ȡ��
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
				crc ^= gx;                     //���������
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

	//��ȡ�豸�б�
	if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf) == -1) {
		QMessageBox::warning(NULL, "��Ǹ", QObject::tr("Error in pcap_findalldevs:%1").arg(errbuf));
		exit(1);
	}

	//��ӡ�豸�б�
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
			str = QString("%1��%2 (%3)").arg(i + 1).arg(d->name).arg(d->description);
		}
		else {
			str = QString("%1��%2 (No description available)").arg(i).arg(d->name);
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
		QMessageBox::warning(NULL, "��Ǹ", QObject::tr("û���ҵ��ӿڣ���ȷ��WinPcap�Ѱ�װ"));
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
			QMessageBox::information(NULL, "��ʾ", QObject::tr("���óɹ�����ʼ����"));
			return;
		}
		i++;
	}
}

void MainWindow::clickReset() {
	QMessageBox::information(NULL, "��ʾ", QObject::tr("�ɹ�����"));
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
	char *type = "ARP";//����ARP
	QList<QAbstractButton*> list = pDeviceButtonGroup2->buttons();
	foreach(QAbstractButton *pButton, list)
	{
		if (pButton->isChecked()) {
			break;
		}
		i++;
	}
	dn = i;//�豸���

		   //���ARP����
	if (ui.ARPTypeRB_Rq->isChecked()) {
		packet[21] = 1;    //����ǰ�����ݰ������������Ӧ���
	}
	else if (ui.ARPTypeRB_Rsp->isChecked()) {
		packet[21] = 2;
	}

	//���Ŀ��MAC��ַ
	packet[0] = ui.lineEdit->text().toInt(&ok, 16);
	packet[1] = ui.lineEdit_2->text().toInt(&ok, 16);
	packet[2] = ui.lineEdit_3->text().toInt(&ok, 16);
	packet[3] = ui.lineEdit_4->text().toInt(&ok, 16);
	packet[4] = ui.lineEdit_5->text().toInt(&ok, 16);
	packet[5] = ui.lineEdit_6->text().toInt(&ok, 16);

	//���ԴMAC��ַ
	packet[6] = ui.lineEdit_7->text().toInt(&ok, 16);
	packet[7] = ui.lineEdit_8->text().toInt(&ok, 16);
	packet[8] = ui.lineEdit_9->text().toInt(&ok, 16);
	packet[9] = ui.lineEdit_10->text().toInt(&ok, 16);
	packet[10] = ui.lineEdit_11->text().toInt(&ok, 16);
	packet[11] = ui.lineEdit_12->text().toInt(&ok, 16);    //ԴMAC��ַ

	packet[12] = 8;
	packet[13] = 6;    //0x0806����ARP���ݰ�;
	packet[14] = 0;
	packet[15] = 1;    //0x0001����Ӳ������,1������̫����ַ
	packet[16] = 8;
	packet[17] = 0;   //0x0800������ӳ���Э��������IP��ַ
	packet[18] = 6;   //Ӳ����ַ����
	packet[19] = 4;   //Э���ַ����
	packet[20] = 0;

	packet[22] = packet[6];
	packet[23] = packet[7];
	packet[24] = packet[8];
	packet[25] = packet[9];
	packet[26] = packet[10];
	packet[27] = packet[11];    //�����ŵ�6���ֽڴ���ԴMAC��ַ

								/*ԴIP��ַ*/                //  ������ݵ��Ե���������ԴIP��Ŀ��IP
	packet[28] = ui.lineEdit_17->text().toInt();
	packet[29] = ui.lineEdit_18->text().toInt();
	packet[30] = ui.lineEdit_19->text().toInt();
	packet[31] = ui.lineEdit_20->text().toInt();

	//ϣ���õ���MAC��ַ
	packet[32] = ui.lineEdit_43->text().toInt();
	packet[33] = ui.lineEdit_44->text().toInt();
	packet[34] = ui.lineEdit_45->text().toInt();
	packet[35] = ui.lineEdit_46->text().toInt();
	packet[36] = ui.lineEdit_47->text().toInt();
	packet[37] = ui.lineEdit_48->text().toInt();

	/*Ŀ��IP��ַ*/
	packet[38] = ui.lineEdit_13->text().toInt();
	packet[39] = ui.lineEdit_14->text().toInt();
	packet[40] = ui.lineEdit_15->text().toInt();
	packet[41] = ui.lineEdit_16->text().toInt();

	for (i = 42; i < 96; i++)
	{
		packet[i] = 0;
	}             //�������ݵķ�װ

	alt_u32 fcs;
	alt_u8 *data = packet;
	alt_u32 dataLen = sizeof(packet) - 4;
	fcs = crc32_bit(data, dataLen, ploy);     //��λ����crcУ��
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
	char *type = "TCP";//����ARP
	QList<QAbstractButton*> list = pDeviceButtonGroup2->buttons();
	foreach(QAbstractButton *pButton, list)
	{
		if (pButton->isChecked()) {
			break;
		}
		i++;
	}
	dn = i;//�豸���

		   //���Ŀ��MAC��ַ
	packet[0] = ui.lineEdit_21->text().toInt(&ok, 16);
	packet[1] = ui.lineEdit_22->text().toInt(&ok, 16);
	packet[2] = ui.lineEdit_23->text().toInt(&ok, 16);
	packet[3] = ui.lineEdit_24->text().toInt(&ok, 16);
	packet[4] = ui.lineEdit_25->text().toInt(&ok, 16);
	packet[5] = ui.lineEdit_26->text().toInt(&ok, 16);

	//���ԴMAC��ַ
	packet[6] = ui.lineEdit_27->text().toInt(&ok, 16);
	packet[7] = ui.lineEdit_28->text().toInt(&ok, 16);
	packet[8] = ui.lineEdit_29->text().toInt(&ok, 16);
	packet[9] = ui.lineEdit_30->text().toInt(&ok, 16);
	packet[10] = ui.lineEdit_31->text().toInt(&ok, 16);
	packet[11] = ui.lineEdit_32->text().toInt(&ok, 16);

	/*���MAC֡����*/
	packet[12] = 8;
	packet[13] = 0;

	/*���IP�ײ�����*/
	packet[14] = 0x45;//�汾+�ײ����ȣ�0x45��
	packet[15] = 0x00;//���ַ���0x00��
	packet[16] = 0x00;//�ܳ���ǰһ���ֽڲ��֣�0x00��
	packet[17] = 0x52;//�ܳ��Ⱥ�һ���ֽڲ��֣�0x05��
	packet[18] = 0x00;//��ʶǰһ���ֽڲ��֣�0x00��
	packet[19] = 0x00;//��ʶ��һ���ֽڲ��֣�0x00��
	packet[20] = 0x00;//��־��Ƭƫ�Ƶ�ǰ5bit��0x00��
	packet[21] = 0x00;//Ƭƫ�Ƶĺ�8bit(0x00)
	packet[22] = 0x10;//����ʱ�䣨0x10��
	packet[23] = 0x06;//Э�飨0x06����ѡ��TCPЭ��
	packet[24] = 0x00;//�ײ�У��͵�ǰһ�ֽڣ�0x00��
	packet[25] = 0x00;//�ײ�У��͵ĺ�һ�ֽڣ�0xa0��

					  /*ԴIP��ַ*/
	packet[26] = ui.lineEdit_37->text().toInt(&ok, 10);
	packet[27] = ui.lineEdit_38->text().toInt(&ok, 10);
	packet[28] = ui.lineEdit_39->text().toInt(&ok, 10);
	packet[29] = ui.lineEdit_40->text().toInt(&ok, 10);
	/*Ŀ��IP��ַ*/
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
	packet[24] &= result >> 8;    //�����ײ�У����ֶ�


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
	packet[41] = (seq & 0x000000FF);   //����ֶ�
	packet[42] = (akSeq & 0xFF000000) >> 24;
	packet[43] = (akSeq & 0x00FF0000) >> 16;
	packet[44] = (akSeq & 0x0000FF00) >> 8;
	packet[45] = (akSeq & 0x000000FF);   //ȷ������ֶ�

	packet[46] = 0x50;
	packet[47] = 0x10;  //��־λ�Լ�����ƫ��

	u_short win = ui.lineEdit_51->text().toInt(&ok, 10);

	packet[48] = (win & 0xFF00) >> 8;
	packet[49] = win & 0x00FF;  //�����ֶ�

	packet[50] = 0x00;
	packet[51] = 0x00;  //У���

	u_short urptr = ui.lineEdit_52->text().toInt(&ok, 10);

	packet[52] = (urptr & 0xFF00) >> 8;
	packet[53] = urptr & 0x00FF;  //urgent pointer ����ָ��

								  /* ���ʣ�µ����� */
	for (i = 54; i < 96; i++)
	{
		packet[i] = 0;
	}

	/* ����TCPУ���*/
	alt_u8 iparry[8];
	for (int i = 26; i<34; i++)
	{
		iparry[i - 26] = packet[i];        //��Դ��Ŀ��IP��ַ��װһ�´���α�ײ���
	}

	alt_u8 *data_tcp = packet;
	data_tcp = data_tcp + 34;
	int tcplen = 62;                 //TCPͷ�������ݰ����ܳ���62
	uint16_t result_tcp;
	result_tcp = tcp_chksum(0, data_tcp, tcplen, iparry);
	packet[51] = result_tcp & 0xff;
	packet[50] = (result_tcp >> 8) & 0xff;    //�����ײ�У����ֶ�
											  /* ����TCPУ���*/

	alt_u32 fcs;
	/* ������̫��֡��FCS�ֶ�*/
	alt_u8 *data = packet;            //������̫��֡FCS�ֶ�
	alt_u32 dataLen = sizeof(packet) - 4;
	fcs = crc32_bit(data, dataLen, ploy);
	packet[96] = packet[97] = packet[98] = packet[99] = 0xff;
	packet[96] &= fcs;
	packet[97] &= fcs >> 8;
	packet[98] &= fcs >> 16;
	packet[99] &= fcs >> 24;
	/* ������̫��֡��FCS�ֶ�*/


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

	//��ȡitem��Э������
	QVariant variant_type = pktItem->data(Qt::UserRole);
	u_short pkt_type = variant_type.value<u_short>();

	//��ȡitem���������ݰ�
	QVariant variant_pkt = pktItem->data(Qt::UserRole + 1);
	Ethernet_pkt pkt_data = variant_pkt.value<Ethernet_pkt>();

	//��ȡitem���������ݰ�����
	int pkt_len = pktItem->data(Qt::UserRole + 2).toInt();

	for (int i = 0; i < pkt_len; i++) {
		bitset<8> tempBitset(pkt_data.byte[i]);
		bool flag = tempBitset.test(3);
		pktStr.append(str.sprintf("%02x ", pkt_data.byte[i]));
	}

	Dialog *pDialog = new Dialog(this);
	pDialog->setWindowTitle(QString("���ݰ�����"));
	pDialog->setFixedSize(600, 800);
	QGroupBox *pkt_box = new QGroupBox();

	QVBoxLayout *pVLayout_Dialog = new QVBoxLayout(pDialog);
	QVBoxLayout *pVLayout_pktgb = new QVBoxLayout(pkt_box);


	pkt_box->setTitle("ԭʼ���ݰ�");

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

		pHwType->setText(str.sprintf("Ӳ�����ͣ�Ethernet (1)"));
		pPtoType->setText(str.sprintf("Э�����ͣ�%s (0x%04x)", ntohs(arph.protyp) == 2048 ? "IPv4" : "else", ntohs(arph.protyp)));
		pHwSize->setText(str.sprintf("Ӳ����ַ���ȣ�%d", arph.hdsize));
		pPtoSize->setText(str.sprintf("Э���ַ���ȣ�%d", arph.prosize));
		pOpcode->setText(str.sprintf("�������ͣ�%s", arph.op == 256 ? "request" : "response"));
		pSmac->setText(str.sprintf("ԴMAC��ַ��%02x-%02x-%02x-%02x-%02x-%02x",
			arph.smac[0],
			arph.smac[1],
			arph.smac[2],
			arph.smac[3],
			arph.smac[4],
			arph.smac[5]
			));
		pSip->setText(str.sprintf("ԴIP��ַ��%d.%d.%d.%d",
			arph.sip[0],
			arph.sip[1],
			arph.sip[2],
			arph.sip[3]
			));
		pDmac->setText(str.sprintf("Ŀ��MAC��ַ��%02x-%02x-%02x-%02x-%02x-%02x",
			arph.dmac[0],
			arph.dmac[1],
			arph.dmac[2],
			arph.dmac[3],
			arph.dmac[4],
			arph.dmac[5]
			));
		pDip->setText(str.sprintf("Ŀ��IP��ַ��%d.%d.%d.%d",
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

			pType->setText(str.sprintf("���ͣ�%d", icmph.type));
			pCode->setText(str.sprintf("���룺%d", ntohs(icmph.code)));
			pCrc->setText(str.sprintf("У��ͣ�0x%04x", ntohs(icmph.crc)));
			pIdt->setText(str.sprintf("��ʶ����%d", ntohs(icmph.idt)));
			pSeq->setText(str.sprintf("���кţ�%d", ntohs(icmph.seq)));

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

			pType->setText(str.sprintf("���ͣ�0x%02x", igmph.type));
			pMrt->setText(str.sprintf("�����Ӧʱ�䣺0x%02x", igmph.mrt));
			pCrc->setText(str.sprintf("У��ͣ�0x%04x", ntohs(igmph.crc)));
			pGroupaddr->setText(str.sprintf("�鲥��ַ��%d.%d.%d.%d",
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

			pSport->setText(str.sprintf("Դ�˿ںţ�%d", ntohs(th.sport)));
			pDport->setText(str.sprintf("Ŀ�Ķ˿ںţ�%d", ntohs(th.dport)));
			pSequence->setText(str.sprintf("��ţ�%u", ntohl(th.sequence)));
			pAcknumber->setText(str.sprintf("ȷ����ţ�%u", ntohl(th.acknumber)));
			pHlen->setText(str.sprintf("ͷ�����ȣ�%d bytes", th.hlen * 4));
			pFlags->setText(str.sprintf("��־λ��0x%03x", th.flags));
			pWindow->setText(str.sprintf("���ڴ�С��%d", ntohs(th.window)));
			pCrc->setText(str.sprintf("У��ͣ�0x%04x", ntohs(th.crc)));
			pUrgptr->setText(str.sprintf("����ָ�룺%x", th.urgptr));
			pOp->setText(str.sprintf("ѡ�0x%x", th.op));

			int th_len = ((th.hlen) * 4);
			int ip_len = (ih.ver_ihl & 0xf) * 4;
			int data_len = pkt_len - 14 - ip_len - th_len;
			QString pktStr;
			QString temp;
			for (int i = 14 + ip_len + th_len; i < data_len; i++) {
				pktStr.append(temp.sprintf("%c", pkt_data.byte[i]));
			}
			std::cmatch cm;
			// �ж��Ƿ�ΪHTTPЭ��
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
				http_box->setTitle("HTTP����");
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

			pSport->setText(str.sprintf("Դ�˿ںţ�%d", ntohs(uh.sport)));
			pDport->setText(str.sprintf("Ŀ�Ķ˿ںţ�%d", ntohs(uh.dport)));
			plen->setText(str.sprintf("���ȣ�%d", ntohs(uh.len)));
			pCrc->setText(str.sprintf("У��ͣ�0x%04x", ntohs(uh.crc)));

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
	QMessageBox::warning(NULL, "��Ǹ", msg);
}

void MainWindow::receiveMsg(QString msg) {
	QMessageBox::information(NULL, "��ʾ", msg);
}

void MainWindow::receivePacket(const struct pcap_pkthdr* header, const u_char* pkt_data) {
	struct tm ltime;
	char timestr[16];

	arp_header *arph;	//ָ��arpͷ��
	ip_header *ih;		//ָ��ipͷ��
	tcp_header *th;		//ָ��tcpͷ��
	udp_header *uh;		//ָ��udpͷ��
	icmp_header *icmph;		//ָ��icmpͷ��
	igmp_header *igmph;		//ָ��igmpͷ��

	Ethernet_pkt *fpkt;		//ָ���������ݰ�
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


	//��ʱ���ת��Ϊ��ʶ��ĸ�ʽ
	local_tv_sec = header->ts.tv_sec;
	localtime_s(&ltime, &local_tv_sec);

	//��ӡʱ��
	str1 = str1.sprintf("%d-%d-%d %d:%d:%d\t", ltime.tm_year + 1900, ltime.tm_mon + 1,
		ltime.tm_mday, ltime.tm_hour, ltime.tm_min, ltime.tm_sec);

	fprintf(fp, "[pkt]%d-%d-%d %d:%d:%d   ", ltime.tm_year + 1900, ltime.tm_mon + 1,
		ltime.tm_mday, ltime.tm_hour, ltime.tm_min, ltime.tm_sec);

	//��ȡ�������ݰ�
	fpkt = (Ethernet_pkt*)(pkt_data);

	//��ȡ��̫��֡ͷ��Э������
	pkt_type = (u_short*)(pkt_data + 12);

	ui.tableWidget->setShowGrid(false);
	ui.tableWidget->setColumnCount(5);

	//���ñ�ͷ����
	QStringList listHeader;
	listHeader << tr("ʱ��") << tr("Դ��ַ") << tr("Ŀ�ĵ�ַ") << tr("����") << tr("Э������");
	ui.tableWidget->setHorizontalHeaderLabels(listHeader);

	//��ͷ����Ӵ�
	QFont font = ui.tableWidget->horizontalHeader()->font();
	font.setBold(true);
	ui.tableWidget->horizontalHeader()->setFont(font);

	ui.tableWidget->horizontalHeader()->resizeSection(0, 160); //���ñ�ͷ��һ�еĿ��Ϊ170
	ui.tableWidget->horizontalHeader()->resizeSection(1, 160);
	ui.tableWidget->horizontalHeader()->resizeSection(2, 160);

	ui.tableWidget->setFrameShape(QFrame::NoFrame); //�����ޱ߿�
	ui.tableWidget->setShowGrid(false); //���ò���ʾ������
	ui.tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);  //����ѡ����Ϊʱÿ��ѡ��һ��
	ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); //���ò��ɱ༭

	/*ARPЭ��Ϊ2054(0x0806��ʮ����)
	IPЭ��Ϊ2048(0x0800��ʮ����)*/
	switch (ntohs(*pkt_type)) {
	case 2054:
	{
		//���ARP���ݰ�ͷ����λ��
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

			//д����־

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

		//��ӡ���ݰ��ĳ���
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

		int row_count = ui.tableWidget->rowCount(); //��ȡ������
		ui.tableWidget->insertRow(row_count); //��������
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
			pitem->setData(Qt::UserRole, QVariant::fromValue(ntohs(*pkt_type)));//item����̫��֡ͷЭ������
			pitem->setData(Qt::UserRole + 1, QVariant::fromValue(*fpkt));//item���������ݰ�
			pitem->setData(Qt::UserRole + 2, header->len);//item�����ݰ�����
			pitem->setData(Qt::UserRole + 3, QVariant::fromValue(*arph));//item��ARPͷ
		}

		break;
	}
	case 2048:
	{
		//��ȡIP���ݰ�ͷ����λ��
		ih = (ip_header*)(pkt_data + 14);//14Ϊ��̫��֡ͷ������

		//���TCP���ݰ�ͷ����λ��
		ip_len = (ih->ver_ihl & 0xf) * 4;
		th = (tcp_header*)((u_char*)ih + ip_len);
		uh = (udp_header*)((u_char*)ih + ip_len);
		icmph = (icmp_header*)((u_char*)ih + ip_len);
		igmph = (igmp_header*)((u_char*)ih + ip_len);

		//��ӡIP��ַ��TCP�˿�
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

		//��ӡ���ݰ��ĳ���
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

		int row_count = ui.tableWidget->rowCount(); //��ȡ������
		ui.tableWidget->insertRow(row_count); //��������
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
			pitem->setData(Qt::UserRole, QVariant::fromValue(ntohs(*pkt_type)));//item����̫��֡ͷЭ������
			pitem->setData(Qt::UserRole + 1, QVariant::fromValue(*fpkt));//item���������ݰ�
			pitem->setData(Qt::UserRole + 2, header->len);//item�����ݰ�����
			pitem->setData(Qt::UserRole + 3, QVariant::fromValue(*ih));//item��IPͷ
			pitem->setData(Qt::UserRole + 4, QVariant::fromValue(*th));//item��TCPͷ
			pitem->setData(Qt::UserRole + 5, QVariant::fromValue(*uh));//item��UDPͷ
			pitem->setData(Qt::UserRole + 6, QVariant::fromValue(*icmph));//item��ICMPͷ
			pitem->setData(Qt::UserRole + 7, QVariant::fromValue(*igmph));//item��IGMPͷ
		}

		break;
	}
	default:
		break;
	}

}

