/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../qstlink2/include/mainwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MainWindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      23,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      17,   11,   12,   11, 0x0a,
      27,   11,   11,   11, 0x0a,
      42,   40,   11,   11, 0x0a,
      68,   66,   11,   11, 0x0a,
     101,   90,   11,   11, 0x0a,
     125,  120,   11,   11, 0x0a,
     142,   11,   11,   11, 0x0a,
     155,   11,   11,   11, 0x0a,
     174,  166,   11,   11, 0x08,
     187,   66,   11,   11, 0x08,
     200,   11,   11,   11, 0x08,
     213,   11,   11,   11, 0x08,
     223,   11,   12,   11, 0x08,
     232,   11,   11,   11, 0x08,
     244,   11,   11,   11, 0x08,
     251,   11,   11,   11, 0x08,
     261,   11,   11,   11, 0x08,
     272,   11,   11,   11, 0x08,
     284,   11,   11,   11, 0x08,
     293,   11,   11,   11, 0x08,
     303,   11,   11,   11, 0x08,
     317,   11,   11,   11, 0x08,
     330,   11,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MainWindow[] = {
    "MainWindow\0\0bool\0Connect()\0Disconnect()\0"
    "p\0updateProgress(quint32)\0s\0"
    "updateStatus(QString)\0path,erase\0"
    "Send(QString,bool)\0path\0Receive(QString)\0"
    "eraseFlash()\0showHelp()\0enabled\0"
    "lockUI(bool)\0log(QString)\0getVersion()\0"
    "getMode()\0getMCU()\0getStatus()\0Send()\0"
    "Receive()\0ResetMCU()\0HardReset()\0"
    "RunMCU()\0HaltMCU()\0setModeJTAG()\0"
    "setModeSWD()\0on_b_sendLast_clicked()\0"
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MainWindow *_t = static_cast<MainWindow *>(_o);
        switch (_id) {
        case 0: { bool _r = _t->Connect();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 1: _t->Disconnect(); break;
        case 2: _t->updateProgress((*reinterpret_cast< const quint32(*)>(_a[1]))); break;
        case 3: _t->updateStatus((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->Send((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const bool(*)>(_a[2]))); break;
        case 5: _t->Receive((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->eraseFlash(); break;
        case 7: _t->showHelp(); break;
        case 8: _t->lockUI((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: _t->log((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 10: _t->getVersion(); break;
        case 11: _t->getMode(); break;
        case 12: { bool _r = _t->getMCU();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 13: _t->getStatus(); break;
        case 14: _t->Send(); break;
        case 15: _t->Receive(); break;
        case 16: _t->ResetMCU(); break;
        case 17: _t->HardReset(); break;
        case 18: _t->RunMCU(); break;
        case 19: _t->HaltMCU(); break;
        case 20: _t->setModeJTAG(); break;
        case 21: _t->setModeSWD(); break;
        case 22: _t->on_b_sendLast_clicked(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData MainWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow,
      qt_meta_data_MainWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MainWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 23)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 23;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
