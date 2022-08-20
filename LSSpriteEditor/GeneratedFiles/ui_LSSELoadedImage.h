/********************************************************************************
** Form generated from reading UI file 'LSSELoadedImage.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LSSELOADEDIMAGE_H
#define UI_LSSELOADEDIMAGE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>
#include "Src/Widgets/LSSELoadedImagesWidget.h"

QT_BEGIN_NAMESPACE

class Ui_LoadedImagesForm
{
public:
    CLoadedImagesWidget *openGLWidget;
    QLabel *imageFile;
    QComboBox *comboBox;
    QGroupBox *groupBox;

    void setupUi(QWidget *LoadedImagesForm)
    {
        if (LoadedImagesForm->objectName().isEmpty())
            LoadedImagesForm->setObjectName(QStringLiteral("LoadedImagesForm"));
        LoadedImagesForm->resize(120, 205);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(LoadedImagesForm->sizePolicy().hasHeightForWidth());
        LoadedImagesForm->setSizePolicy(sizePolicy);
        LoadedImagesForm->setMinimumSize(QSize(120, 205));
        LoadedImagesForm->setMaximumSize(QSize(120, 205));
        openGLWidget = new CLoadedImagesWidget(LoadedImagesForm);
        openGLWidget->setObjectName(QStringLiteral("openGLWidget"));
        openGLWidget->setGeometry(QRect(0, 0, 120, 120));
        sizePolicy.setHeightForWidth(openGLWidget->sizePolicy().hasHeightForWidth());
        openGLWidget->setSizePolicy(sizePolicy);
        openGLWidget->setMinimumSize(QSize(120, 120));
        openGLWidget->setMaximumSize(QSize(120, 120));
        openGLWidget->setAutoFillBackground(true);
        imageFile = new QLabel(LoadedImagesForm);
        imageFile->setObjectName(QStringLiteral("imageFile"));
        imageFile->setGeometry(QRect(0, 120, 121, 31));
        imageFile->setTextFormat(Qt::PlainText);
        imageFile->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        imageFile->setWordWrap(true);
        comboBox = new QComboBox(LoadedImagesForm);
        comboBox->setObjectName(QStringLiteral("comboBox"));
        comboBox->setGeometry(QRect(10, 170, 101, 21));
        groupBox = new QGroupBox(LoadedImagesForm);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(0, 151, 120, 51));
        imageFile->raise();
        groupBox->raise();
        comboBox->raise();
        openGLWidget->raise();

        retranslateUi(LoadedImagesForm);

        QMetaObject::connectSlotsByName(LoadedImagesForm);
    } // setupUi

    void retranslateUi(QWidget *LoadedImagesForm)
    {
        LoadedImagesForm->setWindowTitle(QApplication::translate("LoadedImagesForm", "Form", 0));
        imageFile->setText(QApplication::translate("LoadedImagesForm", "<File Name>", 0));
        comboBox->clear();
        comboBox->insertItems(0, QStringList()
         << QApplication::translate("LoadedImagesForm", "Unchanged", 0)
         << QApplication::translate("LoadedImagesForm", "BC1", 0)
         << QApplication::translate("LoadedImagesForm", "BC2", 0)
         << QApplication::translate("LoadedImagesForm", "BC3", 0)
         << QApplication::translate("LoadedImagesForm", "BC6", 0)
         << QApplication::translate("LoadedImagesForm", "BC7", 0)
        );
        groupBox->setTitle(QApplication::translate("LoadedImagesForm", "Export As", 0));
    } // retranslateUi

};

namespace Ui {
    class LoadedImagesForm: public Ui_LoadedImagesForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LSSELOADEDIMAGE_H
