#include "QtGuiApplication1.h"
#include <qdebug.h>
#include <qstandarditemmodel.h>
#include <qtextstream.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <QFile>
#include "Matrix.h"

QtGuiApplication1::QtGuiApplication1(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	connect(ui.pushButton, &QPushButton::clicked, this, &QtGuiApplication1::On_Cal);
	//设置表格列数
	newMode->setColumnCount(4);
	//添加表头
	newMode->setHeaderData(0, Qt::Horizontal, QStringLiteral("I(列)"));
	newMode->setHeaderData(1, Qt::Horizontal, QStringLiteral("J(行)"));
	newMode->setHeaderData(2, Qt::Horizontal, QStringLiteral("x(mm)"));
	newMode->setHeaderData(3, Qt::Horizontal, QStringLiteral("y(mm)"));
	//设置表头文字居中
	ui.tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
	ui.tableView->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
	connect(ui.actionOpen_File, &QAction::triggered, this, &QtGuiApplication1::OpenFile);
	ui.tableView->setModel(newMode);
}

void QtGuiApplication1::OpenFile()
{
	int i, j;
	//打开文件
	QString filename = QFileDialog::getOpenFileName(this, QStringLiteral("打开数据文件"), "", tr("*.txt"));
	QFile file(filename);
	if (!file.open(QFile::ReadOnly | QFile::Text))
	{
		QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("文件未打开"));
	}
	else
	{
		QTextStream in(&file);
		QString s = in.readAll();
		int linenum = s.count("\n");
		//设置表格行数
		newMode->setRowCount(linenum);
		//文件指针归零
		in.seek(0);
		QStringList list;
		QString ss;
		for (i = 0; i < linenum; i++)
		{
			//读取一行
			ss = in.readLine();
			//分割存储进list
			list = ss.split(QRegExp("(\\s+|,+)"));  //正则表达式--空格或逗号为分隔符的文件
			for (j = 0; j < 4; j++)
			{
				//添加数据
				newMode->setItem(i, j, new QStandardItem(list.at(j)));
				//数据居中
				newMode->item(i, j)->setTextAlignment(Qt::AlignCenter);
			}
		}
	}
	file.close();
}

void QtGuiApplication1::On_Cal()
{
	int i, j, rowNum;
	QModelIndex index;
	double temp;
	rowNum = newMode->rowCount();
	//系数阵
	Matrix B(2 * rowNum, 6);
	//左值矩阵
	Matrix A(2 * rowNum, 1);
	//参数矩阵
	Matrix x(6, 1);
	//常熟阵
	Matrix L(2*rowNum, 1);
	//参数初始值矩阵
	Matrix x0(6, 1);
	//结果矩阵
	Matrix result;

	//计算参数初始值
	Matrix temp1(6, 6);
	Matrix temp2(6, 1);
	for (i = 0; i < 3; i++)
	{
		temp1.set(2 * i, 0, 1);
		temp1.set(2 * i + 1, 3, 1);
		for (j = 0; j < 2; j++)
		{
			index = newMode->index(i, j, QModelIndex());
			temp = index.data().toDouble();
			temp1.set(2 * i, j + 1, temp);
			temp1.set(2 * i + 1, j + 4, temp);
		}
			index = newMode->index(i, 2, QModelIndex());
			temp = index.data().toDouble();
			temp2.set(2 * i, 0, temp);
			index = newMode->index(i, 3, QModelIndex());
			temp = index.data().toDouble();
			temp2.set(2 * i + 1, 0, temp);
	}
	x0 = temp1.Inverse()*temp2;

	//计算系数阵
	for (i = 0; i < rowNum; i++)
	{
		B.set(2 * i, 0, 1);
		B.set(2 * i + 1, 3, 1);
		for (j = 0; j < 2; j++)
		{
			index = newMode->index(i, j, QModelIndex());
			temp = index.data().toDouble();
			B.set(2 * i, j + 1, temp);
			B.set(2 * i + 1, j + 4, temp);
		}
	}

	//左值矩阵
	for (i = 0; i < rowNum; i++)
	{
		index = newMode->index(i, 2, QModelIndex());
		temp = index.data().toDouble();
		A.set(2 * i, 0, temp);
		index = newMode->index(i, 3, QModelIndex());
		temp = index.data().toDouble();
		A.set(2 * i + 1, 0, temp);
	}

	//计算常数阵
	L = A - B * x0;
	x = (B.Trans()*B).Inverse()*(B.Trans()*L);
	Matrix X = x0 + x;
	
	ui.h0LineEdit->setText(QString::number(X.get(0, 0)));
	ui.h1LineEdit->setText(QString::number(X.get(1, 0)));
	ui.h2LineEdit->setText(QString::number(X.get(2, 0)));
	ui.k0LineEdit->setText(QString::number(X.get(3, 0)));
	ui.k1LineEdit->setText(QString::number(X.get(4, 0)));
	ui.k2LineEdit->setText(QString::number(X.get(5, 0)));
}