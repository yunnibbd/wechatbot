#include "friendaddreg.h"
#include <QSettings>
#include <QDir>
#include <QTableWidget>
#include <QTextCodec>
#include <QHeaderView>
#include <QMessageBox>
#include <iostream>
using namespace std;

void FriendAddReg::on_reply_add_pushButton_clicked()
{
	QString reply = ui.reply_textEdit->toPlainText();
	if (reply.isEmpty() || reply == "")
	{
		QMessageBox::information(NULL, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("�ظ����ݲ���Ϊ��"));
		return;
	}

	QString regex_str = ui.regex_textEdit->toPlainText();
	if (regex_str.isEmpty() || regex_str == "")
	{
		QMessageBox::information(NULL, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("����ƥ�䲻��Ϊ��"));
		return;
	}

	bool is_file_type = ui.reply_is_file_type_checkBox->isChecked();
	int rowIndex = ui.tableWidget->rowCount();//��ǰ��������
	ui.tableWidget->insertRow(rowIndex);//�����һ�еĺ������һ��
	ui.tableWidget->setItem(rowIndex, 0, new QTableWidgetItem(reply));
	ui.tableWidget->setItem(rowIndex, 1, new QTableWidgetItem(regex_str));
	auto c1 = new QCheckBox();
	c1->setChecked(is_file_type);
	ui.tableWidget->setCellWidget(rowIndex, 2, c1);
}

void FriendAddReg::on_reply_del_pushButton_clicked()
{
	int current_row_index = ui.tableWidget->currentRow();
	if (current_row_index == -1)
	{
		QMessageBox::information(NULL, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("��δѡ����"));
		return;
	}

	QMessageBox::StandardButton rb = QMessageBox::question(NULL, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("ȷ��Ҫɾ����"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
	if (rb != QMessageBox::Yes)
	{
		return;
	}

	for (int i = 0; i < 3; ++i)
	{
		auto item = ui.tableWidget->item(current_row_index, i);
		delete item;
	}

	ui.tableWidget->removeRow(current_row_index);
}


FriendAddReg::FriendAddReg(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	setWindowModality(Qt::ApplicationModal);
	//����ˮƽ�������ػ�����
	ui.tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	//����ͷ����ÿ�о�ռ��
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	setWindowFlags(windowFlags() & ~Qt::WindowMinMaxButtonsHint);
	setFixedSize(this->width(), this->height());

	load_init_timer_ = new QTimer(this);
	QObject::connect(load_init_timer_, SIGNAL(timeout()), this, SLOT(load_init_cb()));

	mask_ = new Mask(this);
	mask_->move(width() / 2 - mask_->width() / 2, height() / 2 - mask_->height() / 2);
	hide_mask();
}

FriendAddReg::~FriendAddReg()
{
}

void FriendAddReg::delay_load()
{
	load_init_timer_->start(1000);
	show_mask();
}

void FriendAddReg::show_mask()
{
	setEnabled(false);
	mask_->show();
}

void FriendAddReg::hide_mask()
{
	setEnabled(true);
	mask_->hide();
}

void FriendAddReg::load_init_cb()
{
	load_init_timer_->stop();
	QSettings iniFile(QDir::currentPath() + "/wechat.ini", QSettings::IniFormat);
	iniFile.setIniCodec(QTextCodec::codecForName("utf-8"));
	//�˴����ڴ�й¶�ķ��գ����������������й¶��С��������
	ui.tableWidget->setRowCount(0);
	//��ȡ˽�Ļظ�����
	iniFile.beginGroup(editing_wxid_);
	int reply_count = iniFile.value("reg_count", -1).toInt();
	do
	{
		if (reply_count <= 0)
		{
			break;
		}

		for (int i = 1; i <= reply_count; ++i)
		{
			QString reply = iniFile.value(QString("reply") + QString::number(i)).toString();
			QString regex = iniFile.value(QString("regex") + QString::number(i)).toString();
			bool is_file = iniFile.value(QString("is_file") + QString::number(i)).toBool();

			//��ӽ��б�
			int rowIndex = ui.tableWidget->rowCount();//��ǰ��������
			ui.tableWidget->insertRow(rowIndex);//�����һ�еĺ������һ��
			ui.tableWidget->setItem(rowIndex, 0, new QTableWidgetItem(reply));
			ui.tableWidget->setItem(rowIndex, 1, new QTableWidgetItem(regex));
			auto c1 = new QCheckBox();
			c1->setChecked(is_file);
			ui.tableWidget->setCellWidget(rowIndex, 2, c1);
		}
	} while (0);
	iniFile.endGroup();
	hide_mask();
}

void FriendAddReg::on_sync_file_pushButton_clicked()
{
	QSettings iniFile(QDir::currentPath() + "/wechat.ini", QSettings::IniFormat);
	iniFile.setIniCodec(QTextCodec::codecForName("utf-8"));

	//����˽�Ļظ�����
	int reply_count = ui.tableWidget->rowCount();
	do
	{
		if (reply_count == 0)
			break;
		iniFile.beginGroup(editing_wxid_);
		iniFile.setValue("reg_count", reply_count);
		for (int i = 1; i <= reply_count; ++i)
		{
			iniFile.setValue(QString("reply") + QString::number(i), ui.tableWidget->item(i - 1, 0)->text());
			iniFile.setValue(QString("regex") + QString::number(i), ui.tableWidget->item(i - 1, 1)->text());
			QCheckBox * is_file = (QCheckBox*)(ui.tableWidget->cellWidget(i - 1, 2));
			iniFile.setValue(QString("is_file") + QString::number(i), is_file->isChecked());
		}

		iniFile.endGroup();
	} while (0);

	emit reload_f_ini_file();
	QMessageBox::information(NULL, QString::fromLocal8Bit("��Ϣ"), QString::fromLocal8Bit("ͬ���ɹ�"));
}
