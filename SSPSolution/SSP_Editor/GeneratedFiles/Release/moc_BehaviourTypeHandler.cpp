/****************************************************************************
** Meta object code from reading C++ file 'BehaviourTypeHandler.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../BehaviourTypeHandler.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'BehaviourTypeHandler.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Ui__BehaviourTypeHandler_t {
    QByteArrayData data[15];
    char stringdata0[268];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Ui__BehaviourTypeHandler_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Ui__BehaviourTypeHandler_t qt_meta_stringdata_Ui__BehaviourTypeHandler = {
    {
QT_MOC_LITERAL(0, 0, 24), // "Ui::BehaviourTypeHandler"
QT_MOC_LITERAL(1, 25, 16), // "on_Speed_changed"
QT_MOC_LITERAL(2, 42, 0), // ""
QT_MOC_LITERAL(3, 43, 3), // "val"
QT_MOC_LITERAL(4, 47, 15), // "on_Time_changed"
QT_MOC_LITERAL(5, 63, 23), // "on_Path_Trigger_changed"
QT_MOC_LITERAL(6, 87, 27), // "on_Path_Trigger_Box_changed"
QT_MOC_LITERAL(7, 115, 18), // "on_Pattern_changed"
QT_MOC_LITERAL(8, 134, 24), // "on_BehaviourType_changed"
QT_MOC_LITERAL(9, 159, 26), // "on_button_distance_Changed"
QT_MOC_LITERAL(10, 186, 23), // "on_button_timer_Changed"
QT_MOC_LITERAL(11, 210, 16), // "on_CheckpointAdd"
QT_MOC_LITERAL(12, 227, 26), // "on_CheckpointIndex_changed"
QT_MOC_LITERAL(13, 254, 6), // "on_Add"
QT_MOC_LITERAL(14, 261, 6) // "on_Del"

    },
    "Ui::BehaviourTypeHandler\0on_Speed_changed\0"
    "\0val\0on_Time_changed\0on_Path_Trigger_changed\0"
    "on_Path_Trigger_Box_changed\0"
    "on_Pattern_changed\0on_BehaviourType_changed\0"
    "on_button_distance_Changed\0"
    "on_button_timer_Changed\0on_CheckpointAdd\0"
    "on_CheckpointIndex_changed\0on_Add\0"
    "on_Del"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Ui__BehaviourTypeHandler[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   74,    2, 0x0a /* Public */,
       4,    1,   77,    2, 0x0a /* Public */,
       5,    1,   80,    2, 0x0a /* Public */,
       6,    1,   83,    2, 0x0a /* Public */,
       7,    1,   86,    2, 0x0a /* Public */,
       8,    1,   89,    2, 0x0a /* Public */,
       9,    1,   92,    2, 0x0a /* Public */,
      10,    1,   95,    2, 0x0a /* Public */,
      11,    0,   98,    2, 0x0a /* Public */,
      12,    1,   99,    2, 0x0a /* Public */,
      13,    0,  102,    2, 0x0a /* Public */,
      14,    0,  103,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::Double,    3,
    QMetaType::Void, QMetaType::Double,    3,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Double,    3,
    QMetaType::Void, QMetaType::Double,    3,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Ui::BehaviourTypeHandler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        BehaviourTypeHandler *_t = static_cast<BehaviourTypeHandler *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_Speed_changed((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 1: _t->on_Time_changed((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 2: _t->on_Path_Trigger_changed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->on_Path_Trigger_Box_changed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->on_Pattern_changed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->on_BehaviourType_changed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->on_button_distance_Changed((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 7: _t->on_button_timer_Changed((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 8: _t->on_CheckpointAdd(); break;
        case 9: _t->on_CheckpointIndex_changed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->on_Add(); break;
        case 11: _t->on_Del(); break;
        default: ;
        }
    }
}

const QMetaObject Ui::BehaviourTypeHandler::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Ui__BehaviourTypeHandler.data,
      qt_meta_data_Ui__BehaviourTypeHandler,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Ui::BehaviourTypeHandler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Ui::BehaviourTypeHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Ui__BehaviourTypeHandler.stringdata0))
        return static_cast<void*>(const_cast< BehaviourTypeHandler*>(this));
    return QObject::qt_metacast(_clname);
}

int Ui::BehaviourTypeHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 12;
    }
    return _id;
}
QT_END_MOC_NAMESPACE