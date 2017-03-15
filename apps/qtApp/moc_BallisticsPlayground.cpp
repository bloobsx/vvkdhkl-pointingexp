/****************************************************************************
** Meta object code from reading C++ file 'BallisticsPlayground.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "BallisticsPlayground.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'BallisticsPlayground.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_BallisticsPlayground[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      22,   21,   21,   21, 0x05,

 // slots: signature, parameters, type, tag, flags
      55,   31,   21,   21, 0x08,
     101,   21,   21,   21, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_BallisticsPlayground[] = {
    "BallisticsPlayground\0\0finish()\0"
    "timestamp,dx,dy,buttons\0"
    "pointingEvent(TimeStamp::inttime,int,int,int)\0"
    "quit()\0"
};

void BallisticsPlayground::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        BallisticsPlayground *_t = static_cast<BallisticsPlayground *>(_o);
        switch (_id) {
        case 0: _t->finish(); break;
        case 1: _t->pointingEvent((*reinterpret_cast< TimeStamp::inttime(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        case 2: _t->quit(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData BallisticsPlayground::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject BallisticsPlayground::staticMetaObject = {
    { &BALLISCTICSPLAYGROUND_BASECLASS::staticMetaObject, qt_meta_stringdata_BallisticsPlayground,
      qt_meta_data_BallisticsPlayground, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &BallisticsPlayground::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *BallisticsPlayground::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *BallisticsPlayground::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_BallisticsPlayground))
        return static_cast<void*>(const_cast< BallisticsPlayground*>(this));
    return BALLISCTICSPLAYGROUND_BASECLASS::qt_metacast(_clname);
}

int BallisticsPlayground::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BALLISCTICSPLAYGROUND_BASECLASS::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void BallisticsPlayground::finish()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
