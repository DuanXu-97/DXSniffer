/****************************************************************************
** Meta object code from reading C++ file 'Thread.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Thread.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Thread.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_captureThread_t {
    QByteArrayData data[9];
    char stringdata0[89];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_captureThread_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_captureThread_t qt_meta_stringdata_captureThread = {
    {
QT_MOC_LITERAL(0, 0, 13), // "captureThread"
QT_MOC_LITERAL(1, 14, 9), // "sendError"
QT_MOC_LITERAL(2, 24, 0), // ""
QT_MOC_LITERAL(3, 25, 3), // "msg"
QT_MOC_LITERAL(4, 29, 10), // "sendPacket"
QT_MOC_LITERAL(5, 40, 18), // "const pcap_pkthdr*"
QT_MOC_LITERAL(6, 59, 6), // "header"
QT_MOC_LITERAL(7, 66, 13), // "const u_char*"
QT_MOC_LITERAL(8, 80, 8) // "pkt_data"

    },
    "captureThread\0sendError\0\0msg\0sendPacket\0"
    "const pcap_pkthdr*\0header\0const u_char*\0"
    "pkt_data"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_captureThread[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   24,    2, 0x06 /* Public */,
       4,    2,   27,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, 0x80000000 | 5, 0x80000000 | 7,    6,    8,

       0        // eod
};

void captureThread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        captureThread *_t = static_cast<captureThread *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sendError((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->sendPacket((*reinterpret_cast< const pcap_pkthdr*(*)>(_a[1])),(*reinterpret_cast< const u_char*(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (captureThread::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&captureThread::sendError)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (captureThread::*_t)(const pcap_pkthdr * , const u_char * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&captureThread::sendPacket)) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject captureThread::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_captureThread.data,
      qt_meta_data_captureThread,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *captureThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *captureThread::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_captureThread.stringdata0))
        return static_cast<void*>(const_cast< captureThread*>(this));
    return QThread::qt_metacast(_clname);
}

int captureThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void captureThread::sendError(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void captureThread::sendPacket(const pcap_pkthdr * _t1, const u_char * _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
struct qt_meta_stringdata_sendThread_t {
    QByteArrayData data[5];
    char stringdata0[34];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_sendThread_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_sendThread_t qt_meta_stringdata_sendThread = {
    {
QT_MOC_LITERAL(0, 0, 10), // "sendThread"
QT_MOC_LITERAL(1, 11, 9), // "sendError"
QT_MOC_LITERAL(2, 21, 0), // ""
QT_MOC_LITERAL(3, 22, 3), // "msg"
QT_MOC_LITERAL(4, 26, 7) // "sendMsg"

    },
    "sendThread\0sendError\0\0msg\0sendMsg"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_sendThread[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   24,    2, 0x06 /* Public */,
       4,    1,   27,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    3,

       0        // eod
};

void sendThread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        sendThread *_t = static_cast<sendThread *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sendError((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->sendMsg((*reinterpret_cast< QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (sendThread::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&sendThread::sendError)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (sendThread::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&sendThread::sendMsg)) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject sendThread::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_sendThread.data,
      qt_meta_data_sendThread,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *sendThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *sendThread::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_sendThread.stringdata0))
        return static_cast<void*>(const_cast< sendThread*>(this));
    return QThread::qt_metacast(_clname);
}

int sendThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void sendThread::sendError(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void sendThread::sendMsg(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
