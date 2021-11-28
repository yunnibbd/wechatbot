#include "wechatbot.h"
#include "cJSON.h"
#include "logic.h"
#include "mask.h"
#include "addreg.h"
#include <QMouseEvent>
#include <QDebug>
#include <QDir>
#include <QDateTime>
#include <QSettings>
#include <QTextCodec>
#include <QJsonDocument>
#include <cstring>
#include <QMessageBox>
#include <iostream>
#include <regex>
using namespace std;

WeChatBot::WeChatBot(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	//ȥ�����ڱ߿�
	setWindowFlags(Qt::FramelessWindowHint);
	//���ر���������Բ��
	setAttribute(Qt::WA_TranslucentBackground);

	setMouseTracking(true);

	ws_client_ = new QWebSocket();
	QObject::connect(ws_client_, SIGNAL(connected()), this, SLOT(ws_client_connected_cb()));
	QObject::connect(ws_client_, SIGNAL(disconnected()), this, SLOT(ws_client_connected_cb()));
	QObject::connect(ws_client_, SIGNAL(textMessageReceived(const QString &)), this, SLOT(ws_client_received_cb(const QString &)));

	show_log(QString::fromLocal8Bit("΢�Ż�����v1.0����"));

	qRegisterMetaType<std::vector<cJSON*>>("std::vector<cJSON*>");
	QObject::connect(&Logic::Instance(), SIGNAL(update_wxuser_list(std::vector<cJSON*>, std::vector<cJSON*>)), this, SLOT(update_wxuser_list(std::vector<cJSON*>, std::vector<cJSON*>)));
	Logic::Instance().set_instance(this);

	mask_ = new Mask(this);
	mask_->move(width() / 2 - mask_->width() / 2, height() / 2 - mask_->height() / 2);
	mask_->hide();

	add_reg_ = new AddReg();
	QObject::connect(add_reg_, SIGNAL(reload_ini_file()), this, SLOT(reload_ini_file()));

	f_add_reg_ = new FriendAddReg();
	QObject::connect(f_add_reg_, SIGNAL(reload_f_ini_file()), this, SLOT(reload_f_ini_file()));

	get_wxuser_timer_ = new QTimer(this);
	QObject::connect(get_wxuser_timer_, SIGNAL(timeout()), this, SLOT(get_wxuser_timer_slot()));
}

//��ǰ�����Ƿ�������һֱ����
static bool mouse_press = false;
static QPoint mouse_point;
//������־����
static int current_data_count = 0;

void WeChatBot::mouseMoveEvent(QMouseEvent *ev)
{
	//û�а��£�����ԭ�¼�
	if (!mouse_press)
	{
		QWidget::mouseMoveEvent(ev);
		return;
	}
	auto cur_pos = ev->globalPos();
	this->move(cur_pos - mouse_point);

}
void WeChatBot::mousePressEvent(QMouseEvent *ev)
{
	//���������¼�¼λ��
	if (ev->button() == Qt::LeftButton)
	{
		mouse_press = true;
		mouse_point = ev->pos();
	}
}

void WeChatBot::mouseReleaseEvent(QMouseEvent *ev)
{
	mouse_press = false;
}

void WeChatBot::show_log(const QString & log)
{
	if (ui.operator_list_Widget->count() > 500)
	{
		ui.operator_list_Widget->clear();
	}
	QString out_log = QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss]: ");
	out_log += log;
	ui.operator_list_Widget->addItem(out_log);
	ui.operator_list_Widget->setCurrentRow(ui.operator_list_Widget->count() - 1);
}

void WeChatBot::show_data(const QString & data)
{
	if (current_data_count >= 500)
	{
		ui.log_textEdit->clear();
		current_data_count = 0;
	}
	++current_data_count;
	QString out_data = QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss]: ");
	out_data += data;
	ui.log_textEdit->append(out_data);
}

void WeChatBot::send_msg(const char * msg)
{
	ws_client_->sendBinaryMessage(QByteArray::fromRawData(msg, strlen(msg)));
}

void WeChatBot::send_msg(std::string & msg)
{
	ws_client_->sendBinaryMessage(QByteArray::fromStdString(msg));
}

void WeChatBot::send_msg(QString & msg)
{
	ws_client_->sendTextMessage(msg);
}

void WeChatBot::on_close_Button_clicked()
{
	QApplication::quit();
}

void WeChatBot::on_clear_op_Button_clicked()
{
	ui.operator_list_Widget->clear();
}

void WeChatBot::on_clear_log_Button_clicked()
{
	ui.log_textEdit->clear();
	current_data_count = 0;
}

void WeChatBot::on_connect_Button_clicked()
{
	if (is_ws_client_connected_)
	{
		ws_client_->close();
		is_ws_client_connected_ = false;
		ui.connect_Button->setText(QString::fromLocal8Bit("����"));
		all_chatroom_.clear();
		all_friend_.clear();
		ui.friend_listWidget->clear();
		ui.chatroom_listWidget->clear();
	}
	else
	{
		server_ip_ = ui.server_ip_edit->text();
		server_port_ = ui.server_port_box->value();
		QString url = "ws://";
		url += server_ip_;
		url = url + ":" + QString::number(server_port_);
		ws_client_->open(QUrl(url));
		show_log(QString::fromLocal8Bit("��ʼ����") + server_ip_ + ":" + QString::number(server_port_));
	}
}

void WeChatBot::ws_client_connected_cb()
{
	switch (ws_client_->state())
	{
	case QAbstractSocket::ClosingState:
		break;
	case QAbstractSocket::ConnectedState:
	{
		is_ws_client_connected_ = true;
		show_log(QString::fromLocal8Bit("������") + server_ip_ + ":" + QString::number(server_port_) + QString::fromLocal8Bit("�ɹ�"));
		ui.connect_Button->setText(QString::fromLocal8Bit("�Ͽ�"));
		//��ȡͨѶ�б�
		send_msg(Logic::Instance().get_contact_list());
		send_msg(Logic::Instance().get_chatroom_memberlist());
		show_mask();
		show_log(QString::fromLocal8Bit("��ʼˢ��ͨѶ¼��Ϣ����ȴ�"));
		get_wxuser_timer_->start(3500);
	}
		break;
	case QAbstractSocket::UnconnectedState:
		show_log(QString::fromLocal8Bit("���ӶϿ���δ��������������"));
		break;
	default:
		break;
	}
}

void WeChatBot::ws_client_received_cb(const QString & msg)
{
	show_data(msg);
	cJSON * json = cJSON_Parse(msg.toStdString().c_str());
	Logic::Instance().dispatch_msg(json);
	cJSON_Delete(json);
}

void WeChatBot::ws_client_disconnected_cb()
{
	is_ws_client_connected_ = false;
	ui.connect_Button->setText(QString::fromLocal8Bit("����"));
	show_log("�����˶Ͽ�����");
}

void WeChatBot::on_edit_friend_Button_clicked()
{
	auto item = ui.friend_listWidget->currentItem();
	if (!item) 
	{
		QMessageBox::information(NULL, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("����ѡ��Ҫ�༭����ĺ���"));
		return;
	}
	std::regex check_reg("wxid_\\w*");
	QString will_set_win_title = item->text();
	if (!regex_match(will_set_win_title.toStdString(), check_reg))
	{
		auto iter = all_friend_.find(item->text().toStdString());
		if (iter == all_friend_.end())
		{//�߼��ϲ��ᷢ��
			QMessageBox::information(NULL, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("����ѡ��Ҫ�༭����ĺ���"));
			return;
		}
		will_set_win_title = QString::fromStdString(iter->second);
	}

	f_add_reg_->set_wxid(will_set_win_title);
	f_add_reg_->delay_load();
	f_add_reg_->show();
}

void WeChatBot::on_edit_chatroom_Button_clicked()
{
	auto item = ui.chatroom_listWidget->currentItem();
	if (!item)
	{
		QMessageBox::information(NULL, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("����ѡ��Ҫ�༭����ĺ���"));
		return;
	}
	std::regex check_reg("\\d{11}@chatroom");
	QString will_set_win_title = item->text();
	if (!regex_match(will_set_win_title.toStdString(), check_reg))
	{//��Ⱥ�������ƣ�Ҫ�ҳ�����Ⱥid
		auto iter = all_chatroom_.find(will_set_win_title.toStdString());
		if (iter == all_chatroom_.end())
		{//�����������������ܱ�����
			QMessageBox::information(NULL, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("����δ֪�������������"));
			return;
		}
		will_set_win_title = QString::fromStdString(iter->second);
	}

	add_reg_->set_wxid(will_set_win_title);
	add_reg_->delay_load();
	add_reg_->show();
}

void WeChatBot::update_wxuser_list(std::vector<cJSON *> items, std::vector<cJSON *> items2)
{
	if (get_wxuser_timer_->isActive())
		get_wxuser_timer_->stop();
	ui.friend_listWidget->clear();
	for (auto & item : items)
	{
		string key = cJSON_GetObjectItem(item, "wxid")->valuestring;
		string val = cJSON_GetObjectItem(item, "name")->valuestring;
		all_friend_.insert(make_pair(key, val));
		all_friend_.insert(make_pair(val, key));
		if (val != "")
			ui.friend_listWidget->addItem(val.c_str());
		else
			ui.friend_listWidget->addItem(key.c_str());
	}

	ui.chatroom_listWidget->clear();
	for (auto & item : items2)
	{
		string key = cJSON_GetObjectItem(item, "wxid")->valuestring;
		string val = cJSON_GetObjectItem(item, "name")->valuestring;
		all_chatroom_.insert(make_pair(key, val));
		all_chatroom_.insert(make_pair(val, key));
		if (val != "")
			ui.chatroom_listWidget->addItem(val.c_str());
		else
			ui.chatroom_listWidget->addItem(key.c_str());
	}

	hide_mask();
	show_log(QString::fromLocal8Bit("ˢ�º�����Ⱥ���б�ɹ�"));

	//����ͨѶ¼�ɹ����ȡ�ظ�����
	reload_ini_file();
	reload_f_ini_file();
}

void WeChatBot::show_mask()
{
	setEnabled(false);
	mask_->show();
}

void WeChatBot::hide_mask()
{
	setEnabled(true);
	mask_->hide();
}

void WeChatBot::reload_ini_file()
{
	QSettings iniFile(QDir::currentPath() + "/wechat.ini", QSettings::IniFormat);
	iniFile.setIniCodec(QTextCodec::codecForName("utf-8"));
	all_chatoom_regs_.clear();
	//��ȡȺ�Ļظ�����
	for (auto & item : all_chatroom_)
	{
		iniFile.beginGroup(QString::fromStdString(item.first));
		int reply_count = iniFile.value("reg_count", -1).toInt();
		do
		{
			if (reply_count <= 0)
			{
				break;
			}
			vector<unordered_map<string, string>> all_item;

			for (int i = 1; i <= reply_count; ++i)
			{
				QString reply = iniFile.value(QString("reply") + QString::number(i)).toString();
				QString regex = iniFile.value(QString("regex") + QString::number(i)).toString();
				bool is_file = iniFile.value(QString("is_file") + QString::number(i)).toBool();
				bool is_at = iniFile.value(QString("is_at") + QString::number(i)).toBool();

				//�����ڴ�
				unordered_map<string, string> one_item;
				one_item.insert(make_pair(string("reply"), reply.toStdString()));
				one_item.insert(make_pair(string("regex"), regex.toStdString()));
				one_item.insert(make_pair(string("is_file"), string(is_file ? "1" : "0")));
				one_item.insert(make_pair(string("is_at"), string(is_at ? "1" : "0")));
				all_item.push_back(one_item);
			}
			all_chatoom_regs_.insert(make_pair(item.first, all_item));
		} while (0);
		iniFile.endGroup();

		//��ȡ�³�Ա��Ⱥ�ظ�����
		iniFile.beginGroup(QString::fromStdString(item.first) + "_nm");
		int nm_reply_count = iniFile.value("reg_count", -1).toInt();
		do
		{
			if (nm_reply_count <= 0)
			{
				break;
			}
			vector<unordered_map<string, string>> all_item;
			for (int i = 1; i <= nm_reply_count; ++i)
			{
				QString reply = iniFile.value(QString("reply") + QString::number(i)).toString();
				bool is_file = iniFile.value(QString("is_file") + QString::number(i)).toBool();
				bool is_at = iniFile.value(QString("is_at") + QString::number(i)).toBool();

				//�����ڴ�
				unordered_map<string, string> one_item;
				
				one_item.insert(make_pair(string("reply"), reply.toStdString()));
				one_item.insert(make_pair(string("is_file"), string(is_file ? "1" : "0")));
				one_item.insert(make_pair(string("is_at"), string(is_at ? "1" : "0")));
				all_item.push_back(one_item);
			}
			all_chatoom_regs_.insert(make_pair(item.first + "_nm", all_item));
		} while (0);
		iniFile.endGroup();
	}
}

void WeChatBot::reload_f_ini_file()
{
	QSettings iniFile(QDir::currentPath() + "/wechat.ini", QSettings::IniFormat);
	iniFile.setIniCodec(QTextCodec::codecForName("utf-8"));
	all_friend_regs_.clear();
	//��ȡȺ�Ļظ�����
	for (auto & item : all_friend_)
	{
		iniFile.beginGroup(QString::fromStdString(item.first));
		int reply_count = iniFile.value("reg_count", -1).toInt();
		do
		{
			if (reply_count <= 0)
			{
				break;
			}
			vector<unordered_map<string, string>> all_item;

			for (int i = 1; i <= reply_count; ++i)
			{
				QString reply = iniFile.value(QString("reply") + QString::number(i)).toString();
				QString regex = iniFile.value(QString("regex") + QString::number(i)).toString();
				bool is_file = iniFile.value(QString("is_file") + QString::number(i)).toBool();

				//�����ڴ�
				unordered_map<string, string> one_item;
				one_item.insert(make_pair(string("reply"), reply.toStdString()));
				one_item.insert(make_pair(string("regex"), regex.toStdString()));
				one_item.insert(make_pair(string("is_file"), string(is_file ? "1" : "0")));
				all_item.push_back(one_item);
			}
			all_friend_regs_.insert(make_pair(item.first, all_item));
		} while (0);
		iniFile.endGroup();
	}
}

void WeChatBot::get_wxuser_timer_slot()
{
	hide_mask();
	show_log(QString::fromLocal8Bit("ˢ�º�����Ⱥ���б�ʱ��������"));
}
