#define  HAVE_REMOTE

#include "Thread.h"  
#include "MainWindow.h"

#pragma execution_character_set("utf-8")

pcap_if_t *d;
int i = 0;
pcap_t *adhandle;
char errbuf[PCAP_ERRBUF_SIZE];
u_int netmask;
struct bpf_program fcode;

//�������ݰ��߳�
captureThread::captureThread()
{
	stopped = false;
}

void captureThread::run()
{
	//��ת����ѡ�豸
	for (d = this->alldevs, i = 0; i < this->deviceNumber - 1; d = d->next, i++);

	//��������
	if ((adhandle = pcap_open(d->name,
		65536,
		PCAP_OPENFLAG_PROMISCUOUS,
		1000,
		NULL,
		errbuf)) == NULL) {
		emit(sendError(QString("�޷�����������WinPcap�ݲ�֧��")));
		exit(-1);
		pcap_freealldevs(alldevs);
	}

	//���������·�㣬ֻ������̫��
	if (pcap_datalink(adhandle) != DLT_EN10MB) {
		emit(sendError(QString("��Ǹ��������ֻ֧����̫��")));
		pcap_freealldevs(alldevs);
		exit(-1);
	}

	if (d->addresses != NULL) {
		//��ýӿڵ�һ����ַ������
		netmask = ((struct sockaddr_in*)(d->addresses->netmask))->sin_addr.S_un.S_addr;
	}
	else
	{
		//����ӿ�û�е�ַ����ô����һ��C������
		netmask = 0xffffff;
	}

	//���������
	if (pcap_compile(adhandle, &fcode, this->packet_filter, 1, netmask) < 0) {
		emit(sendError(QString("��Ǹ���޷�����������������﷨")));
		pcap_freealldevs(alldevs);
		exit(-1);
	}

	//���ù�����
	if (pcap_setfilter(adhandle, &fcode) < 0) {
		emit(sendError(QString("��Ǹ�����ù�����ʧ��")));
		pcap_freealldevs(alldevs);
		exit(-1);
	}

	pcap_freealldevs(alldevs);

	pcap_loop(adhandle, 0, packet_handler, (u_char*)this);
}

void captureThread::stop()
{
	stopped = true;
	pcap_breakloop(adhandle);
	pcap_close(adhandle);
	this->quit();
}

void captureThread::setDevice(int dn, pcap_if_t *alldevs)
{
	this->deviceNumber = dn;
	this->alldevs = alldevs;
}

// ���ù�����������"arp or (ip and tcp)"
void captureThread::setFilter(const char* filter)
{
	strcpy(this->packet_filter, filter);
}


void captureThread::packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data) {
	captureThread* c = (captureThread*)param;
	emit(c->sendPacket(header, pkt_data));
}

//�������ݰ��߳�

sendThread::sendThread()
{
	stopped = false;
}

void sendThread::run()
{

	//��ת����ѡ�豸
	for (d = this->alldevs, i = 0; i < this->deviceNumber - 1; d = d->next, i++);

	//��������
	if ((adhandle = pcap_open(d->name,
		65536,
		PCAP_OPENFLAG_PROMISCUOUS,
		1000,
		NULL,
		errbuf)) == NULL) {
		emit(sendError(QString("�޷�����������WinPcap�ݲ�֧��")));
		exit(-1);
		pcap_freealldevs(alldevs);
	}

	/* �������ݰ� */
	if (pcap_sendpacket(adhandle, this->packet, 100) == 0)
	{
		QString str = QString("���ݰ����ͳɹ���");
		emit sendMsg(str);
	}
	else {
		QString str = QString("��Ǹ�����ݰ�����ʧ�ܣ�");
		emit sendMsg(str);
	}
}

void sendThread::setInfo(int dn, pcap_if_t *alldevs, const char* type, u_char* packet)
{
	this->deviceNumber = dn;
	this->alldevs = alldevs;
	this->type = type;
	for (int i = 0; i < 100; i++)
	{
		this->packet[i] = packet[i];
	}
	//this->packet = packet;
}