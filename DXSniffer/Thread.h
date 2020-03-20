#pragma once

#ifndef THREAD_H  
#define THREAD_H  
#include<QThread>  
#include<QString>  
#include "stdio.h"
#include "winsock2.h"
#include "pcap.h"
#include "windows.h"

#include "QMessageBox.h"

#pragma comment ( lib, "wpcap.lib" )
#pragma comment ( lib, "ws2_32" )

class captureThread : public QThread
{
	Q_OBJECT
public:
	captureThread();
	void run();
	void stop();
	void setDevice(int dn, pcap_if_t *alldevs);
	void setFilter(const char *filter);
	static void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data);

	public slots :

	signals:
		   void sendError(QString msg);
		   void sendPacket(const struct pcap_pkthdr *header, const u_char *pkt_data);

private:
	volatile bool stopped;
	int deviceNumber;
	pcap_if_t *alldevs;
	char packet_filter[100];
};

class sendThread : public QThread
{
	Q_OBJECT
public:
	sendThread();
	void run();
	void stop();
	void setInfo(int dn, pcap_if_t *alldevs, const char* type, u_char* packet);



	public slots :

	signals:
		   void sendError(QString msg);
		   void sendMsg(QString msg);

private:
	volatile bool stopped;
	int deviceNumber;
	pcap_if_t *alldevs;
	const char* type;
	u_char packet[100];
};

#endif // THREAD_H  
