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

//��ȡ�豸�б�
if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf) == -1) {
QMessageBox::warning(NULL, "��Ǹ", QObject::tr("Error in pcap_findalldevs:%1").arg(errbuf));
exit(1);
}

//��ӡ�豸�б��ж��豸���������û���豸�򷵻�
if(w->printDeviceList() == -1) return -1;


printf("Enter the interface number(1-%d):", i);
scanf_s("%d", &inum);

if (inum<1 || inum>i) {
printf("Inteface number out of range.\n");
pcap_freealldevs(alldevs);
return -1;
}

//��ת����ѡ�豸
for (d = alldevs, i = 0; i < inum - 1; d = d->next, i++);

//��������
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

//���������·�㣬ֻ������̫��
if (pcap_datalink(adhandle) != DLT_EN10MB) {
fprintf(stderr, "This program works only on Ethernet network.\n");
pcap_freealldevs(alldevs);
return -1;
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
if (pcap_compile(adhandle, &fcode, packet_filter, 1, netmask) < 0) {
fprintf(stderr, "Unable to compile the packet filter. Check the syntax.\n");
pcap_freealldevs(alldevs);
return -1;
}

//���ù�����
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


