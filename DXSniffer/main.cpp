#define  HAVE_REMOTE

#pragma execution_character_set("utf-8")

#include "MainWindow.h"
#include <QtWidgets/QApplication>


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;
	w.show();

	return a.exec();
}
/*
int capture(MainWindow *w) {
pcap_if_t *alldevs;
pcap_if_t *d;
int inum;
int i = 0;
pcap_t *adhandle;
char errbuf[PCAP_ERRBUF_SIZE];
u_int netmask;
char packet_filter[] = "ip and tcp";
struct bpf_program fcode;

//获取设备列表
if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf) == -1) {
QMessageBox::warning(NULL, "抱歉", QObject::tr("Error in pcap_findalldevs:%1").arg(errbuf));
exit(1);
}

//打印设备列表并判断设备数量，如果没有设备则返回
if(w->printDeviceList() == -1) return -1;


printf("Enter the interface number(1-%d):", i);
scanf_s("%d", &inum);

if (inum<1 || inum>i) {
printf("Inteface number out of range.\n");
pcap_freealldevs(alldevs);
return -1;
}

//跳转到已选设备
for (d = alldevs, i = 0; i < inum - 1; d = d->next, i++);

//打开适配器
if ((adhandle = pcap_open(d->name,
65536,
PCAP_OPENFLAG_PROMISCUOUS,
1000,
NULL,
errbuf)) == NULL) {
fprintf(stderr, "Unable to open the adapter.It's not supported by WinPcap.\n");
pcap_freealldevs(alldevs);
return -1;
}

//检查数据链路层，只考虑以太网
if (pcap_datalink(adhandle) != DLT_EN10MB) {
fprintf(stderr, "This program works only on Ethernet network.\n");
pcap_freealldevs(alldevs);
return -1;
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
if (pcap_compile(adhandle, &fcode, packet_filter, 1, netmask) < 0) {
fprintf(stderr, "Unable to compile the packet filter. Check the syntax.\n");
pcap_freealldevs(alldevs);
return -1;
}

//设置过滤器
if (pcap_setfilter(adhandle, &fcode) < 0) {
fprintf(stderr, "Error setting the filter.\n");
pcap_freealldevs(alldevs);
return -1;
}

printf("listening on %s...\n", d->description);

pcap_freealldevs(alldevs);

pcap_loop(adhandle, 0, packet_handler, NULL);


return 0;
}
*/


