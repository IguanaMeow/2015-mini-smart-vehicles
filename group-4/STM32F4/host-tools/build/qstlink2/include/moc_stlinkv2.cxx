/****************************************************************************
** Meta object code from reading C++ file 'stlinkv2.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../qstlink2/include/stlinkv2.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'stlinkv2.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_stlinkv2[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      28,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      17,    9,   10,    9, 0x0a,
      27,    9,    9,    9, 0x0a,
      40,    9,    9,    9, 0x0a,
      51,    9,    9,    9, 0x0a,
      66,    9,    9,    9, 0x0a,
      76,    9,    9,    9, 0x0a,
      85,    9,    9,    9, 0x0a,
      99,    9,    9,    9, 0x0a,
     112,    9,    9,    9, 0x0a,
     138,  129,   10,    9, 0x0a,
     170,  165,   10,    9, 0x2a,
     198,  189,    9,    9, 0x0a,
     235,    9,  230,    9, 0x0a,
     246,    9,    9,    9, 0x0a,
     259,    9,  230,    9, 0x0a,
     273,    9,  230,    9, 0x0a,
     285,    9,  230,    9, 0x0a,
     302,    9,  230,    9, 0x0a,
     315,  311,  230,    9, 0x0a,
     341,  311,  230,    9, 0x0a,
     360,    9,    9,    9, 0x0a,
     375,  370,    9,    9, 0x0a,
     406,    9,  398,    9, 0x0a,
     430,    9,  422,    9, 0x0a,
     442,    9,  422,    9, 0x0a,
     455,    9,  422,    9, 0x0a,
     465,    9,  422,    9, 0x0a,
     477,    9,  422,    9, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_stlinkv2[] = {
    "stlinkv2\0\0qint32\0connect()\0disconnect()\0"
    "resetMCU()\0hardResetMCU()\0haltMCU()\0"
    "runMCU()\0setModeJTAG()\0setModeSWD()\0"
    "setExitModeDFU()\0addr,len\0"
    "readMem32(quint32,quint16)\0addr\0"
    "readMem32(quint32)\0addr,buf\0"
    "writeMem32(quint32,QByteArray&)\0bool\0"
    "isLocked()\0eraseFlash()\0unlockFlash()\0"
    "lockFlash()\0unlockFlashOpt()\0isBusy()\0"
    "val\0setFlashProgramming(bool)\0"
    "setMassErase(bool)\0setSTRT()\0size\0"
    "setProgramSize(quint8)\0quint32\0"
    "readFlashSize()\0QString\0getStatus()\0"
    "getVersion()\0getMode()\0getCoreID()\0"
    "getChipID()\0"
};

void stlinkv2::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        stlinkv2 *_t = static_cast<stlinkv2 *>(_o);
        switch (_id) {
        case 0: { qint32 _r = _t->connect();
            if (_a[0]) *reinterpret_cast< qint32*>(_a[0]) = _r; }  break;
        case 1: _t->disconnect(); break;
        case 2: _t->resetMCU(); break;
        case 3: _t->hardResetMCU(); break;
        case 4: _t->haltMCU(); break;
        case 5: _t->runMCU(); break;
        case 6: _t->setModeJTAG(); break;
        case 7: _t->setModeSWD(); break;
        case 8: _t->setExitModeDFU(); break;
        case 9: { qint32 _r = _t->readMem32((*reinterpret_cast< const quint32(*)>(_a[1])),(*reinterpret_cast< const quint16(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< qint32*>(_a[0]) = _r; }  break;
        case 10: { qint32 _r = _t->readMem32((*reinterpret_cast< const quint32(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< qint32*>(_a[0]) = _r; }  break;
        case 11: _t->writeMem32((*reinterpret_cast< const quint32(*)>(_a[1])),(*reinterpret_cast< QByteArray(*)>(_a[2]))); break;
        case 12: { bool _r = _t->isLocked();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 13: _t->eraseFlash(); break;
        case 14: { bool _r = _t->unlockFlash();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 15: { bool _r = _t->lockFlash();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 16: { bool _r = _t->unlockFlashOpt();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 17: { bool _r = _t->isBusy();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 18: { bool _r = _t->setFlashProgramming((*reinterpret_cast< const bool(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 19: { bool _r = _t->setMassErase((*reinterpret_cast< const bool(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 20: _t->setSTRT(); break;
        case 21: _t->setProgramSize((*reinterpret_cast< const quint8(*)>(_a[1]))); break;
        case 22: { quint32 _r = _t->readFlashSize();
            if (_a[0]) *reinterpret_cast< quint32*>(_a[0]) = _r; }  break;
        case 23: { QString _r = _t->getStatus();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 24: { QString _r = _t->getVersion();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 25: { QString _r = _t->getMode();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 26: { QString _r = _t->getCoreID();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 27: { QString _r = _t->getChipID();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        default: ;
        }
    }
}

const QMetaObjectExtraData stlinkv2::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject stlinkv2::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_stlinkv2,
      qt_meta_data_stlinkv2, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &stlinkv2::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *stlinkv2::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *stlinkv2::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_stlinkv2))
        return static_cast<void*>(const_cast< stlinkv2*>(this));
    return QThread::qt_metacast(_clname);
}

int stlinkv2::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 28)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 28;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
