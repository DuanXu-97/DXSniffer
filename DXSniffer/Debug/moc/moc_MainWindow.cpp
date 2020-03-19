/****************************************************************************
** Meta object code from reading C++ file 'MainWindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../MainWindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MainWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[20];
    char stringdata0[226];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 12), // "deviceNumber"
QT_MOC_LITERAL(2, 24, 0), // ""
QT_MOC_LITERAL(3, 25, 2), // "dn"
QT_MOC_LITERAL(4, 28, 17), // "clickSubmitDevice"
QT_MOC_LITERAL(5, 46, 17), // "clickPacketDetail"
QT_MOC_LITERAL(6, 64, 17), // "QTableWidgetItem*"
QT_MOC_LITERAL(7, 82, 7), // "pktItem"
QT_MOC_LITERAL(8, 90, 12), // "startCapture"
QT_MOC_LITERAL(9, 103, 12), // "errorMessage"
QT_MOC_LITERAL(10, 116, 5), // "param"
QT_MOC_LITERAL(11, 122, 13), // "receivePacket"
QT_MOC_LITERAL(12, 136, 18), // "const pcap_pkthdr*"
QT_MOC_LITERAL(13, 155, 6), // "header"
QT_MOC_LITERAL(14, 162, 13), // "const u_char*"
QT_MOC_LITERAL(15, 176, 8), // "pkt_data"
QT_MOC_LITERAL(16, 185, 12), // "clickSendARP"
QT_MOC_LITERAL(17, 198, 12), // "clickSendTCP"
QT_MOC_LITERAL(18, 211, 10), // "receiveMsg"
QT_MOC_LITERAL(19, 222, 3) // "msg"

    },
    "MainWindow\0deviceNumber\0\0dn\0"
    "clickSubmitDevice\0clickPacketDetail\0"
    "QTableWidgetItem*\0pktItem\0startCapture\0"
    "errorMessage\0param\0receivePacket\0"
    "const pcap_pkthdr*\0header\0const u_char*\0"
    "pkt_data\0clickSendARP\0clickSendTCP\0"
    "receiveMsg\0msg"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   59,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    0,   62,    2, 0x0a /* Public */,
       5,    1,   63,    2, 0x0a /* Public */,
       8,    1,   66,    2, 0x0a /* Public */,
       9,    1,   69,    2, 0x0a /* Public */,
      11,    2,   72,    2, 0x0a /* Public */,
      16,    0,   77,    2, 0x0a /* Public */,
      17,    0,   78,    2, 0x0a /* Public */,
      18,    1,   79,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,   10,
    QMetaType::Void, 0x80000000 | 12, 0x80000000 | 14,   13,   15,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   19,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MainWindow *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->deviceNumber((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->clickSubmitDevice(); break;
        case 2: _t->clickPacketDetail((*reinterpret_cast< QTableWidgetItem*(*)>(_a[1]))); break;
        case 3: _t->startCapture((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->errorMessage((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->receivePacket((*reinterpret_cast< const pcap_pkthdr*(*)>(_a[1])),(*reinterpret_cast< const u_char*(*)>(_a[2]))); break;
        case 6: _t->clickSendARP(); break;
        case 7: _t->clickSendTCP(); break;
        case 8: _t->receiveMsg((*reinterpret_cast< QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (MainWindow::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MainWindow::deviceNumber)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow.data,
      qt_meta_data_MainWindow,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void MainWindow::deviceNumber(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
