/****************************************************************************
** Meta object code from reading C++ file 'transferthread.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../qstlink2/include/transferthread.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'transferthread.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_transferThread[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: signature, parameters, type, tag, flags
      18,   16,   15,   15, 0x05,
      42,   40,   15,   15, 0x05,
      62,   40,   15,   15, 0x05,
      89,   81,   15,   15, 0x05,
     104,   40,   15,   15, 0x05,

 // slots: signature, parameters, type, tag, flags
     121,   15,   15,   15, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_transferThread[] = {
    "transferThread\0\0p\0sendProgress(quint32)\0"
    "s\0sendStatus(QString)\0sendError(QString)\0"
    "enabled\0sendLock(bool)\0sendLog(QString)\0"
    "halt()\0"
};

void transferThread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        transferThread *_t = static_cast<transferThread *>(_o);
        switch (_id) {
        case 0: _t->sendProgress((*reinterpret_cast< const quint32(*)>(_a[1]))); break;
        case 1: _t->sendStatus((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->sendError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->sendLock((*reinterpret_cast< const bool(*)>(_a[1]))); break;
        case 4: _t->sendLog((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: _t->halt(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData transferThread::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject transferThread::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_transferThread,
      qt_meta_data_transferThread, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &transferThread::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *transferThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *transferThread::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_transferThread))
        return static_cast<void*>(const_cast< transferThread*>(this));
    return QThread::qt_metacast(_clname);
}

int transferThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void transferThread::sendProgress(const quint32 & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void transferThread::sendStatus(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void transferThread::sendError(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void transferThread::sendLock(const bool & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void transferThread::sendLog(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_END_MOC_NAMESPACE
