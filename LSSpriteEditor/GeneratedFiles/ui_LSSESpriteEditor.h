/********************************************************************************
** Form generated from reading UI file 'LSSESpriteEditor.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LSSESPRITEEDITOR_H
#define UI_LSSESPRITEEDITOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QSlider>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QToolBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "Src/Widgets/LSSEMainOpenGlWidget.h"
#include "Src/Widgets/LSSENaviOpenGlWidget.h"
#include "Src/Widgets/LSSEScrollWidget.h"

QT_BEGIN_NAMESPACE

class Ui_LSSpriteEditorClass
{
public:
    QAction *action_New;
    QAction *action_Open;
    QAction *action_Save;
    QAction *actionSave_As;
    QAction *actionZoom_In;
    QAction *actionZoom_Out;
    QAction *action_Undo;
    QAction *action_Redo;
    QAction *actionAdd_Image;
    QAction *action_Draw_Sprites;
    QAction *actionDraw_Normal;
    QAction *actionDraw_Fixed;
    QAction *actionDraw_Wand;
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    QWidget *spriteEditWidget;
    QGridLayout *gridLayout_2;
    CScrollWidget *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QGridLayout *gridLayout_9;
    CMainOpenGlWidget *mainOpenGLWidget;
    QWidget *rightBarWidget;
    QVBoxLayout *verticalLayout;
    QToolBox *toolBox_3;
    QWidget *imagePage_2;
    QGridLayout *gridLayout_7;
    CScrollWidget *imagesScroll;
    QWidget *imagesScroll_4;
    QToolBox *toolBox;
    QWidget *naviPage;
    QDoubleSpinBox *naviSpinBox;
    QSlider *naviSlider;
    CNaviOpenGlWidget *naviOpenGLWidget;
    QWidget *page_2;
    QMenuBar *menuBar;
    QMenu *menu_File;
    QMenu *menu_Edit;
    QStatusBar *statusBar;
    QToolBar *toolBar;
    QToolBar *mainToolBar;

    void setupUi(QMainWindow *LSSpriteEditorClass)
    {
        if (LSSpriteEditorClass->objectName().isEmpty())
            LSSpriteEditorClass->setObjectName(QStringLiteral("LSSpriteEditorClass"));
        LSSpriteEditorClass->resize(901, 784);
        LSSpriteEditorClass->setStyleSheet(QStringLiteral("background-color: qlineargradient(spread:reflect, x1:0, y1:1, x2:1, y2:0, stop:0 rgba(183, 183, 191, 255), stop:0.125 rgba(191, 191, 191, 255), stop:1 rgba(255, 255, 255, 255));"));
        action_New = new QAction(LSSpriteEditorClass);
        action_New->setObjectName(QStringLiteral("action_New"));
        QIcon icon;
        icon.addFile(QStringLiteral(":/LSSpriteEditor/Resources/addfile24.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_New->setIcon(icon);
        action_Open = new QAction(LSSpriteEditorClass);
        action_Open->setObjectName(QStringLiteral("action_Open"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/LSSpriteEditor/Resources/folder24.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Open->setIcon(icon1);
        action_Save = new QAction(LSSpriteEditorClass);
        action_Save->setObjectName(QStringLiteral("action_Save"));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/LSSpriteEditor/Resources/floppy24.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Save->setIcon(icon2);
        actionSave_As = new QAction(LSSpriteEditorClass);
        actionSave_As->setObjectName(QStringLiteral("actionSave_As"));
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/LSSpriteEditor/Resources/saveas24.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSave_As->setIcon(icon3);
        actionZoom_In = new QAction(LSSpriteEditorClass);
        actionZoom_In->setObjectName(QStringLiteral("actionZoom_In"));
        QIcon icon4;
        icon4.addFile(QStringLiteral(":/LSSpriteEditor/Resources/zoomin24.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionZoom_In->setIcon(icon4);
        actionZoom_Out = new QAction(LSSpriteEditorClass);
        actionZoom_Out->setObjectName(QStringLiteral("actionZoom_Out"));
        QIcon icon5;
        icon5.addFile(QStringLiteral(":/LSSpriteEditor/Resources/zoomout24.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionZoom_Out->setIcon(icon5);
        action_Undo = new QAction(LSSpriteEditorClass);
        action_Undo->setObjectName(QStringLiteral("action_Undo"));
        QIcon icon6;
        icon6.addFile(QStringLiteral(":/LSSpriteEditor/Resources/undo24.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Undo->setIcon(icon6);
        action_Redo = new QAction(LSSpriteEditorClass);
        action_Redo->setObjectName(QStringLiteral("action_Redo"));
        QIcon icon7;
        icon7.addFile(QStringLiteral(":/LSSpriteEditor/Resources/redo24.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Redo->setIcon(icon7);
        actionAdd_Image = new QAction(LSSpriteEditorClass);
        actionAdd_Image->setObjectName(QStringLiteral("actionAdd_Image"));
        QIcon icon8;
        icon8.addFile(QStringLiteral(":/LSSpriteEditor/Resources/pictures24.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionAdd_Image->setIcon(icon8);
        action_Draw_Sprites = new QAction(LSSpriteEditorClass);
        action_Draw_Sprites->setObjectName(QStringLiteral("action_Draw_Sprites"));
        action_Draw_Sprites->setCheckable(true);
        QIcon icon9;
        icon9.addFile(QStringLiteral(":/LSSpriteEditor/Resources/edit24.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Draw_Sprites->setIcon(icon9);
        actionDraw_Normal = new QAction(LSSpriteEditorClass);
        actionDraw_Normal->setObjectName(QStringLiteral("actionDraw_Normal"));
        actionDraw_Normal->setCheckable(true);
        QIcon icon10;
        icon10.addFile(QStringLiteral(":/LSSpriteEditor/Resources/DrawNormal.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionDraw_Normal->setIcon(icon10);
        actionDraw_Fixed = new QAction(LSSpriteEditorClass);
        actionDraw_Fixed->setObjectName(QStringLiteral("actionDraw_Fixed"));
        actionDraw_Fixed->setCheckable(true);
        QIcon icon11;
        icon11.addFile(QStringLiteral(":/LSSpriteEditor/Resources/DrawFixed.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionDraw_Fixed->setIcon(icon11);
        actionDraw_Wand = new QAction(LSSpriteEditorClass);
        actionDraw_Wand->setObjectName(QStringLiteral("actionDraw_Wand"));
        actionDraw_Wand->setCheckable(true);
        QIcon icon12;
        icon12.addFile(QStringLiteral(":/LSSpriteEditor/Resources/DrawWand.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionDraw_Wand->setIcon(icon12);
        centralWidget = new QWidget(LSSpriteEditorClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        spriteEditWidget = new QWidget(centralWidget);
        spriteEditWidget->setObjectName(QStringLiteral("spriteEditWidget"));
        spriteEditWidget->setEnabled(true);
        gridLayout_2 = new QGridLayout(spriteEditWidget);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        scrollArea = new CScrollWidget(spriteEditWidget);
        scrollArea->setObjectName(QStringLiteral("scrollArea"));
        scrollArea->setAutoFillBackground(true);
        scrollArea->setWidgetResizable(true);
        scrollArea->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 715, 704));
        gridLayout_9 = new QGridLayout(scrollAreaWidgetContents);
        gridLayout_9->setSpacing(6);
        gridLayout_9->setContentsMargins(11, 11, 11, 11);
        gridLayout_9->setObjectName(QStringLiteral("gridLayout_9"));
        gridLayout_9->setContentsMargins(0, 0, 0, 0);
        mainOpenGLWidget = new CMainOpenGlWidget(scrollAreaWidgetContents);
        mainOpenGLWidget->setObjectName(QStringLiteral("mainOpenGLWidget"));
        mainOpenGLWidget->setMouseTracking(true);

        gridLayout_9->addWidget(mainOpenGLWidget, 0, 0, 2, 1);

        scrollArea->setWidget(scrollAreaWidgetContents);

        gridLayout_2->addWidget(scrollArea, 0, 0, 1, 1);

        rightBarWidget = new QWidget(spriteEditWidget);
        rightBarWidget->setObjectName(QStringLiteral("rightBarWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(rightBarWidget->sizePolicy().hasHeightForWidth());
        rightBarWidget->setSizePolicy(sizePolicy);
        rightBarWidget->setMinimumSize(QSize(120, 0));
        verticalLayout = new QVBoxLayout(rightBarWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        toolBox_3 = new QToolBox(rightBarWidget);
        toolBox_3->setObjectName(QStringLiteral("toolBox_3"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Minimum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(toolBox_3->sizePolicy().hasHeightForWidth());
        toolBox_3->setSizePolicy(sizePolicy1);
        toolBox_3->setMinimumSize(QSize(140, 0));
        toolBox_3->setAutoFillBackground(false);
        imagePage_2 = new QWidget();
        imagePage_2->setObjectName(QStringLiteral("imagePage_2"));
        imagePage_2->setGeometry(QRect(0, 0, 140, 324));
        imagePage_2->setMinimumSize(QSize(120, 0));
        gridLayout_7 = new QGridLayout(imagePage_2);
        gridLayout_7->setSpacing(6);
        gridLayout_7->setContentsMargins(11, 11, 11, 11);
        gridLayout_7->setObjectName(QStringLiteral("gridLayout_7"));
        gridLayout_7->setContentsMargins(0, 0, 0, 0);
        imagesScroll = new CScrollWidget(imagePage_2);
        imagesScroll->setObjectName(QStringLiteral("imagesScroll"));
        imagesScroll->setAutoFillBackground(true);
        imagesScroll->setFrameShape(QFrame::NoFrame);
        imagesScroll->setFrameShadow(QFrame::Plain);
        imagesScroll->setWidgetResizable(true);
        imagesScroll->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        imagesScroll_4 = new QWidget();
        imagesScroll_4->setObjectName(QStringLiteral("imagesScroll_4"));
        imagesScroll_4->setGeometry(QRect(0, 0, 140, 324));
        imagesScroll->setWidget(imagesScroll_4);

        gridLayout_7->addWidget(imagesScroll, 0, 0, 1, 1);

        toolBox_3->addItem(imagePage_2, QStringLiteral("Images"));

        verticalLayout->addWidget(toolBox_3);

        toolBox = new QToolBox(rightBarWidget);
        toolBox->setObjectName(QStringLiteral("toolBox"));
        sizePolicy1.setHeightForWidth(toolBox->sizePolicy().hasHeightForWidth());
        toolBox->setSizePolicy(sizePolicy1);
        toolBox->setMinimumSize(QSize(140, 0));
        naviPage = new QWidget();
        naviPage->setObjectName(QStringLiteral("naviPage"));
        naviPage->setGeometry(QRect(0, 0, 140, 298));
        naviSpinBox = new QDoubleSpinBox(naviPage);
        naviSpinBox->setObjectName(QStringLiteral("naviSpinBox"));
        naviSpinBox->setGeometry(QRect(0, 139, 120, 22));
        naviSpinBox->setLayoutDirection(Qt::RightToLeft);
        naviSpinBox->setFrame(true);
        naviSpinBox->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        naviSpinBox->setButtonSymbols(QAbstractSpinBox::UpDownArrows);
        naviSpinBox->setAccelerated(false);
        naviSpinBox->setMinimum(1);
        naviSpinBox->setMaximum(1.67772e+07);
        naviSlider = new QSlider(naviPage);
        naviSlider->setObjectName(QStringLiteral("naviSlider"));
        naviSlider->setGeometry(QRect(0, 120, 120, 19));
        naviSlider->setMinimum(1);
        naviSlider->setMaximum(16777215);
        naviSlider->setOrientation(Qt::Horizontal);
        naviSlider->setInvertedAppearance(false);
        naviSlider->setInvertedControls(false);
        naviSlider->setTickPosition(QSlider::NoTicks);
        naviOpenGLWidget = new CNaviOpenGlWidget(naviPage);
        naviOpenGLWidget->setObjectName(QStringLiteral("naviOpenGLWidget"));
        naviOpenGLWidget->setGeometry(QRect(0, 0, 120, 120));
        naviOpenGLWidget->setMinimumSize(QSize(120, 120));
        naviOpenGLWidget->setMaximumSize(QSize(120, 120));
        naviOpenGLWidget->setAutoFillBackground(false);
        toolBox->addItem(naviPage, QStringLiteral("Navigation"));
        page_2 = new QWidget();
        page_2->setObjectName(QStringLiteral("page_2"));
        page_2->setGeometry(QRect(0, 0, 140, 298));
        toolBox->addItem(page_2, QStringLiteral("Page 2"));

        verticalLayout->addWidget(toolBox);


        gridLayout_2->addWidget(rightBarWidget, 0, 1, 1, 1);


        gridLayout->addWidget(spriteEditWidget, 0, 0, 1, 1);

        LSSpriteEditorClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(LSSpriteEditorClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 901, 21));
        menuBar->setAutoFillBackground(false);
        menu_File = new QMenu(menuBar);
        menu_File->setObjectName(QStringLiteral("menu_File"));
        menu_Edit = new QMenu(menuBar);
        menu_Edit->setObjectName(QStringLiteral("menu_Edit"));
        LSSpriteEditorClass->setMenuBar(menuBar);
        statusBar = new QStatusBar(LSSpriteEditorClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        LSSpriteEditorClass->setStatusBar(statusBar);
        toolBar = new QToolBar(LSSpriteEditorClass);
        toolBar->setObjectName(QStringLiteral("toolBar"));
        toolBar->setStyleSheet(QStringLiteral("background-color: rgb(223, 223, 223);"));
        toolBar->setAllowedAreas(Qt::LeftToolBarArea|Qt::RightToolBarArea);
        LSSpriteEditorClass->addToolBar(Qt::LeftToolBarArea, toolBar);
        mainToolBar = new QToolBar(LSSpriteEditorClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        mainToolBar->setStyleSheet(QStringLiteral("background-color: rgb(223, 223, 223);"));
        LSSpriteEditorClass->addToolBar(Qt::TopToolBarArea, mainToolBar);

        menuBar->addAction(menu_File->menuAction());
        menuBar->addAction(menu_Edit->menuAction());
        menu_File->addAction(action_New);
        menu_File->addAction(action_Open);
        menu_File->addAction(action_Save);
        menu_File->addAction(actionSave_As);
        menu_File->addSeparator();
        menu_File->addAction(actionAdd_Image);
        menu_Edit->addAction(action_Undo);
        menu_Edit->addAction(action_Redo);
        toolBar->addAction(actionZoom_In);
        toolBar->addAction(actionZoom_Out);
        toolBar->addSeparator();
        toolBar->addAction(actionDraw_Normal);
        toolBar->addAction(actionDraw_Fixed);
        toolBar->addAction(actionDraw_Wand);
        mainToolBar->addAction(action_New);
        mainToolBar->addAction(action_Open);
        mainToolBar->addAction(action_Save);
        mainToolBar->addAction(actionSave_As);
        mainToolBar->addSeparator();
        mainToolBar->addAction(action_Undo);
        mainToolBar->addAction(action_Redo);

        retranslateUi(LSSpriteEditorClass);
        QObject::connect(actionAdd_Image, SIGNAL(triggered()), LSSpriteEditorClass, SLOT(AddImage()));
        QObject::connect(actionSave_As, SIGNAL(triggered()), LSSpriteEditorClass, SLOT(SaveAs()));
        QObject::connect(action_New, SIGNAL(triggered()), LSSpriteEditorClass, SLOT(NewProject()));
        QObject::connect(action_Open, SIGNAL(triggered()), LSSpriteEditorClass, SLOT(Open()));
        QObject::connect(action_Save, SIGNAL(triggered()), LSSpriteEditorClass, SLOT(Save()));
        QObject::connect(actionZoom_In, SIGNAL(triggered()), LSSpriteEditorClass, SLOT(ZoomIn()));
        QObject::connect(actionZoom_Out, SIGNAL(triggered()), LSSpriteEditorClass, SLOT(ZoomOut()));
        QObject::connect(naviSlider, SIGNAL(valueChanged(int)), LSSpriteEditorClass, SLOT(SlideZoom()));
        QObject::connect(naviSpinBox, SIGNAL(valueChanged(double)), LSSpriteEditorClass, SLOT(SpinZoom()));
        QObject::connect(actionDraw_Normal, SIGNAL(triggered()), LSSpriteEditorClass, SLOT(DrawModeNormal()));
        QObject::connect(actionDraw_Fixed, SIGNAL(triggered()), LSSpriteEditorClass, SLOT(DrawModeFixed()));
        QObject::connect(actionDraw_Wand, SIGNAL(triggered()), LSSpriteEditorClass, SLOT(DrawModeWand()));

        toolBox_3->setCurrentIndex(0);
        toolBox->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(LSSpriteEditorClass);
    } // setupUi

    void retranslateUi(QMainWindow *LSSpriteEditorClass)
    {
        LSSpriteEditorClass->setWindowTitle(QApplication::translate("LSSpriteEditorClass", "L. Spiro Sprite Editor", 0));
        action_New->setText(QApplication::translate("LSSpriteEditorClass", "&New Project\342\200\246", 0));
#ifndef QT_NO_TOOLTIP
        action_New->setToolTip(QApplication::translate("LSSpriteEditorClass", "New Project\342\200\246", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        action_New->setStatusTip(QApplication::translate("LSSpriteEditorClass", "Create a new sprite project.", 0));
#endif // QT_NO_STATUSTIP
        action_New->setShortcut(QApplication::translate("LSSpriteEditorClass", "Ctrl+N", 0));
        action_Open->setText(QApplication::translate("LSSpriteEditorClass", "&Open Project\342\200\246", 0));
#ifndef QT_NO_TOOLTIP
        action_Open->setToolTip(QApplication::translate("LSSpriteEditorClass", "Open Project\342\200\246", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        action_Open->setStatusTip(QApplication::translate("LSSpriteEditorClass", "Open a sprite project.", 0));
#endif // QT_NO_STATUSTIP
        action_Open->setShortcut(QApplication::translate("LSSpriteEditorClass", "Ctrl+O", 0));
        action_Save->setText(QApplication::translate("LSSpriteEditorClass", "&Save Project\342\200\246", 0));
#ifndef QT_NO_TOOLTIP
        action_Save->setToolTip(QApplication::translate("LSSpriteEditorClass", "Save Project\342\200\246", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        action_Save->setStatusTip(QApplication::translate("LSSpriteEditorClass", "Save the current sprite project.", 0));
#endif // QT_NO_STATUSTIP
        action_Save->setShortcut(QApplication::translate("LSSpriteEditorClass", "Ctrl+S", 0));
        actionSave_As->setText(QApplication::translate("LSSpriteEditorClass", "Save Project &As\342\200\246", 0));
#ifndef QT_NO_TOOLTIP
        actionSave_As->setToolTip(QApplication::translate("LSSpriteEditorClass", "Save Project As\342\200\246", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        actionSave_As->setStatusTip(QApplication::translate("LSSpriteEditorClass", "Save the current sprite project as a new file.", 0));
#endif // QT_NO_STATUSTIP
        actionSave_As->setShortcut(QApplication::translate("LSSpriteEditorClass", "Ctrl+Shift+S", 0));
        actionZoom_In->setText(QApplication::translate("LSSpriteEditorClass", "Zoom &In", 0));
#ifndef QT_NO_TOOLTIP
        actionZoom_In->setToolTip(QApplication::translate("LSSpriteEditorClass", "Zoom In", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        actionZoom_In->setStatusTip(QApplication::translate("LSSpriteEditorClass", "Increases the zoom level.", 0));
#endif // QT_NO_STATUSTIP
        actionZoom_In->setShortcut(QApplication::translate("LSSpriteEditorClass", "Ctrl+[", 0));
        actionZoom_Out->setText(QApplication::translate("LSSpriteEditorClass", "Zoom &Out", 0));
#ifndef QT_NO_TOOLTIP
        actionZoom_Out->setToolTip(QApplication::translate("LSSpriteEditorClass", "Zoom Out", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        actionZoom_Out->setStatusTip(QApplication::translate("LSSpriteEditorClass", "Decreases the zoom level.", 0));
#endif // QT_NO_STATUSTIP
        actionZoom_Out->setShortcut(QApplication::translate("LSSpriteEditorClass", "Ctrl+]", 0));
        action_Undo->setText(QApplication::translate("LSSpriteEditorClass", "&Undo", 0));
#ifndef QT_NO_STATUSTIP
        action_Undo->setStatusTip(QApplication::translate("LSSpriteEditorClass", "Undo the last action.", 0));
#endif // QT_NO_STATUSTIP
        action_Undo->setShortcut(QApplication::translate("LSSpriteEditorClass", "Ctrl+Z", 0));
        action_Redo->setText(QApplication::translate("LSSpriteEditorClass", "&Redo", 0));
#ifndef QT_NO_STATUSTIP
        action_Redo->setStatusTip(QApplication::translate("LSSpriteEditorClass", "Redo the last action.", 0));
#endif // QT_NO_STATUSTIP
        action_Redo->setShortcut(QApplication::translate("LSSpriteEditorClass", "Ctrl+Y", 0));
        actionAdd_Image->setText(QApplication::translate("LSSpriteEditorClass", "Add &Image", 0));
        actionAdd_Image->setShortcut(QApplication::translate("LSSpriteEditorClass", "Ctrl+I", 0));
        action_Draw_Sprites->setText(QApplication::translate("LSSpriteEditorClass", "&Draw Sprites", 0));
#ifndef QT_NO_TOOLTIP
        action_Draw_Sprites->setToolTip(QApplication::translate("LSSpriteEditorClass", "Draw Sprites", 0));
#endif // QT_NO_TOOLTIP
        actionDraw_Normal->setText(QApplication::translate("LSSpriteEditorClass", "Draw &Normal", 0));
#ifndef QT_NO_TOOLTIP
        actionDraw_Normal->setToolTip(QApplication::translate("LSSpriteEditorClass", "Create Sprite Rectangles Manually", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        actionDraw_Normal->setStatusTip(QApplication::translate("LSSpriteEditorClass", "Manually draw the border around a sprite.", 0));
#endif // QT_NO_STATUSTIP
        actionDraw_Fixed->setText(QApplication::translate("LSSpriteEditorClass", "Draw &Fixed", 0));
#ifndef QT_NO_TOOLTIP
        actionDraw_Fixed->setToolTip(QApplication::translate("LSSpriteEditorClass", "Draw with a Fixed-Width Rectangle", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        actionDraw_Fixed->setStatusTip(QApplication::translate("LSSpriteEditorClass", "Position a fixed-width border around a sprite.", 0));
#endif // QT_NO_STATUSTIP
        actionDraw_Wand->setText(QApplication::translate("LSSpriteEditorClass", "Draw &Wand", 0));
#ifndef QT_NO_TOOLTIP
        actionDraw_Wand->setToolTip(QApplication::translate("LSSpriteEditorClass", "Automatically Find Sprite", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        actionDraw_Wand->setStatusTip(QApplication::translate("LSSpriteEditorClass", "Click anywhere on a sprite in the image and this tool tries to find the borders automatically.", 0));
#endif // QT_NO_STATUSTIP
#ifndef QT_NO_STATUSTIP
        imagePage_2->setStatusTip(QApplication::translate("LSSpriteEditorClass", "List of all the loaded images.", 0));
#endif // QT_NO_STATUSTIP
        toolBox_3->setItemText(toolBox_3->indexOf(imagePage_2), QApplication::translate("LSSpriteEditorClass", "Images", 0));
        toolBox->setItemText(toolBox->indexOf(naviPage), QApplication::translate("LSSpriteEditorClass", "Navigation", 0));
        toolBox->setItemText(toolBox->indexOf(page_2), QApplication::translate("LSSpriteEditorClass", "Page 2", 0));
        menu_File->setTitle(QApplication::translate("LSSpriteEditorClass", "&File", 0));
        menu_Edit->setTitle(QApplication::translate("LSSpriteEditorClass", "&Edit", 0));
        toolBar->setWindowTitle(QApplication::translate("LSSpriteEditorClass", "Tools", 0));
        mainToolBar->setWindowTitle(QApplication::translate("LSSpriteEditorClass", "Menu Bar", 0));
    } // retranslateUi

};

namespace Ui {
    class LSSpriteEditorClass: public Ui_LSSpriteEditorClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LSSESPRITEEDITOR_H
