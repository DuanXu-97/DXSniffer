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
    QByteArrayData data[22];
    char stringdata0[238];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 14), // "captureSetting"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 2), // "dn"
QT_MOC_LITERAL(4, 30, 11), // "const char*"
QT_MOC_LITERAL(5, 42, 2), // "ft"
QT_MOC_LITERAL(6, 45, 11), // "clickSubmit"
QT_MOC_LITERAL(7, 57, 17), // "clickPacketDetail"
QT_MOC_LITERAL(8, 75, 17), // "QTableWidgetItem*"
QT_MOC_LITERAL(9, 93, 7), // "pktItem"
QT_MOC_LITERAL(10, 101, 12), // "startCapture"
QT_MOC_LITERAL(11, 114, 6), // "filter"
QT_MOC_LITERAL(12, 121, 13), // "receivePacket"
QT_MOC_LITERAL(13, 135, 18), // "const pcap_pkthdr*"
QT_MOC_LITERAL(14, 154, 6), // "header"
QT_MOC_LITERAL(15, 161, 13), // "const u_char*"
QT_MOC_LITERAL(16, 175, 8), // "pkt_data"
QT_MOC_LITERAL(17, 184, 12), // "clickSendARP"
QT_MOC_LITERAL(18, 197, 12), // "clickSendTCP"
QT_MOC_LITERAL(19, 210, 12), // "errorMessage"
QT_MOC_LITERAL(20, 223, 3), // "msg"
QT_MOC_LITERAL(21, 227, 10) // "receiveMsg"

    },
    "MainWindow\0captureSetting\0\0dn\0const char*\0"
    "ft\0clickSubmit\0clickPacketDetail\0"
    "QTableWidgetItem*\0pktItem\0startCapture\0"
    "filter\0receivePacket\0const pcap_pkthdr*\0"
    "header\0const u_char*\0pkt_data\0"
    "clickSendARP\0clickSendTCP\0errorMessage\0"
    "msg\0receiveMsg"
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
       1,    2,   59,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    0,   64,    2, 0x0a /* Public */,
       7,    1,   65,    2, 0x0a /* Public */,
      10,    2,   68,    2, 0x0a /* Public */,
      12,    2,   73,    2, 0x0a /* Public */,
      17,    0,   78,    2, 0x0a /* Public */,
      18,    0,   79,    2, 0x0a /* Public */,
      19,    1,   80,    2, 0x0a /* Public */,
      21,    1,   83,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, 0x80000000 | 4,    3,    5,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 4,    3,   11,
    QMetaType::Void, 0x80000000 | 13, 0x80000000 | 15,   14,   16,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   20,
    QMetaType::Void, QMetaType::QString,   20,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MainWindow *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->captureSetting((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const char*(*)>(_a[2]))); break;
        case 1: _t->clickSubmit(); break;
        case 2: _t->clickPacketDetail((*reinterpret_cast< QTableWidgetItem*(*)>(_a[1]))); break;
        case 3: _t->startCapture((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const char*(*)>(_a[2]))); break;
        case 4: _t->receivePacket((*reinterpret_cast< const pcap_pkthdr*(*)>(_a[1])),(*reinterpret_cast< const u_char*(*)>(_a[2]))); break;
        case 5: _t->clickSendARP(); break;
        case 6: _t->clickSendTCP(); break;
        case 7: _t->errorMessage((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 8: _t->receiveMsg((*reinterpret_cast< QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (MainWindow::*_t)(int , const char * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MainWindow::captureSetting)) {
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
void MainWindow::captureSetting(int _t1, const char * _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
