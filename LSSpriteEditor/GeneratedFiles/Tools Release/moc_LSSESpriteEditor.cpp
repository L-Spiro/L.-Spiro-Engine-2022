/****************************************************************************
** Meta object code from reading C++ file 'LSSESpriteEditor.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Src/LSSESpriteEditor.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'LSSESpriteEditor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_LSSpriteEditor_t {
    QByteArrayData data[15];
    char stringdata0[141];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_LSSpriteEditor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_LSSpriteEditor_t qt_meta_stringdata_LSSpriteEditor = {
    {
QT_MOC_LITERAL(0, 0, 14), // "LSSpriteEditor"
QT_MOC_LITERAL(1, 15, 10), // "NewProject"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 4), // "Open"
QT_MOC_LITERAL(4, 32, 4), // "Save"
QT_MOC_LITERAL(5, 37, 6), // "SaveAs"
QT_MOC_LITERAL(6, 44, 8), // "AddImage"
QT_MOC_LITERAL(7, 53, 11), // "DeleteImage"
QT_MOC_LITERAL(8, 65, 6), // "ZoomIn"
QT_MOC_LITERAL(9, 72, 7), // "ZoomOut"
QT_MOC_LITERAL(10, 80, 9), // "SlideZoom"
QT_MOC_LITERAL(11, 90, 8), // "SpinZoom"
QT_MOC_LITERAL(12, 99, 14), // "DrawModeNormal"
QT_MOC_LITERAL(13, 114, 13), // "DrawModeFixed"
QT_MOC_LITERAL(14, 128, 12) // "DrawModeWand"

    },
    "LSSpriteEditor\0NewProject\0\0Open\0Save\0"
    "SaveAs\0AddImage\0DeleteImage\0ZoomIn\0"
    "ZoomOut\0SlideZoom\0SpinZoom\0DrawModeNormal\0"
    "DrawModeFixed\0DrawModeWand"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_LSSpriteEditor[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   79,    2, 0x08 /* Private */,
       3,    0,   80,    2, 0x08 /* Private */,
       4,    0,   81,    2, 0x08 /* Private */,
       5,    0,   82,    2, 0x08 /* Private */,
       6,    0,   83,    2, 0x08 /* Private */,
       7,    0,   84,    2, 0x08 /* Private */,
       8,    0,   85,    2, 0x08 /* Private */,
       9,    0,   86,    2, 0x08 /* Private */,
      10,    0,   87,    2, 0x08 /* Private */,
      11,    0,   88,    2, 0x08 /* Private */,
      12,    0,   89,    2, 0x08 /* Private */,
      13,    0,   90,    2, 0x08 /* Private */,
      14,    0,   91,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void LSSpriteEditor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        LSSpriteEditor *_t = static_cast<LSSpriteEditor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->NewProject(); break;
        case 1: _t->Open(); break;
        case 2: _t->Save(); break;
        case 3: _t->SaveAs(); break;
        case 4: _t->AddImage(); break;
        case 5: _t->DeleteImage(); break;
        case 6: _t->ZoomIn(); break;
        case 7: _t->ZoomOut(); break;
        case 8: _t->SlideZoom(); break;
        case 9: _t->SpinZoom(); break;
        case 10: _t->DrawModeNormal(); break;
        case 11: _t->DrawModeFixed(); break;
        case 12: _t->DrawModeWand(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject LSSpriteEditor::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_LSSpriteEditor.data,
      qt_meta_data_LSSpriteEditor,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *LSSpriteEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LSSpriteEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_LSSpriteEditor.stringdata0))
        return static_cast<void*>(const_cast< LSSpriteEditor*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int LSSpriteEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 13;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
