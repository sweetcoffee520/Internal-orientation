#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QtGuiApplication1.h"
#include <qstandarditemmodel.h>

class QtGuiApplication1 : public QMainWindow
{
	Q_OBJECT

public:
	QtGuiApplication1(QWidget *parent = Q_NULLPTR);

private slots:
	void OpenFile();
	void On_Cal();

private:
	QStandardItemModel *newMode = new QStandardItemModel();
	Ui::QtGuiApplication1Class ui;
};
