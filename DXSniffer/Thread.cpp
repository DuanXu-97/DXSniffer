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

//捕获数据包线程
captureThread::captureThread()
{
	stopped = false;
}

void captureThread::run()
{
	//跳转到已选设备
	for (d = this->alldevs, i = 0; i < this->deviceNumber - 1; d = d->next, i++);

	//打开适配器
	if ((adhandle = pcap_open(d->name,
		65536,
		PCAP_OPENFLAG_PROMISCUOUS,
		1000,
		NULL,
		errbuf)) == NULL) {
		emit(sendError(QString("无法打开适配器，WinPcap暂不支持")));
		exit(-1);
		pcap_freealldevs(alldevs);
	}

	//检查数据链路层，只考虑以太网
	if (pcap_datalink(adhandle) != DLT_EN10MB) {
		emit(sendError(QString("抱歉，本程序只支持以太网")));
		pcap_freealldevs(alldevs);
		exit(-1);
	}

	if (d->addresses != NULL) {
		//获得接口第一个地址的掩码
		netmask = ((struct sockaddr_in*)(d->addresses->netmask))->sin_addr.S_un.S_addr;
	}
	else
	{
		//如果接口没有地址，那么假设一个C类掩码
		netmask = 0xffffff;
	}

	//编译过滤器
	if (pcap_compile(adhandle, &fcode, this->packet_filter, 1, netmask) < 0) {
		emit(sendError(QString("抱歉，无法编译过滤器，请检查语法")));
		pcap_freealldevs(alldevs);
		exit(-1);
	}

	//设置过滤器
	if (pcap_setfilter(adhandle, &fcode) < 0) {
		emit(sendError(QString("抱歉，设置过滤器失败")));
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

// 设置过滤器，例如"arp or (ip and tcp)"
void captureThread::setFilter(const char* filter)
{
	strcpy(this->packet_filter, filter);
}


void captureThread::packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data) {
	captureThread* c = (captureThread*)param;
	emit(c->sendPacket(header, pkt_data));
}

//发送数据包线程

sendThread::sendThread()
{
	stopped = false;
}

void sendThread::run()
{

	//跳转到已选设备
	for (d = this->alldevs, i = 0; i < this->deviceNumber - 1; d = d->next, i++);

	//打开适配器
	if ((adhandle = pcap_open(d->name,
		65536,
		PCAP_OPENFLAG_PROMISCUOUS,
		1000,
		NULL,
		errbuf)) == NULL) {
		emit(sendError(QString("无法打开适配器，WinPcap暂不支持")));
		exit(-1);
		pcap_freealldevs(alldevs);
	}

	/* 发送数据包 */
	if (pcap_sendpacket(adhandle, this->packet, 100) == 0)
	{
		QString str = QString("数据包发送成功！");
		emit sendMsg(str);
	}
	else {
		QString str = QString("抱歉，数据包发送失败！");
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