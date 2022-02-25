#pragma once

#include <QtWidgets/QWidget>
#include <QWebSocket>
#include <string>
#include <unordered_map>
#include <vector>
#include <string>
#include <QTimer>
#include "ui_wechatbot.h"
#include "addreg.h"
#include "friendaddreg.h"
#include "mask.h"

struct cJSON;

class WeChatBot : public QWidget
{
	Q_OBJECT

public:
	friend class Logic;

	WeChatBot(QWidget *parent = Q_NULLPTR);

	void mouseMoveEvent(QMouseEvent *ev) override;

	void mousePressEvent(QMouseEvent *ev) override;

	void mouseReleaseEvent(QMouseEvent *ev) override;

	/**
	 * @brief ��ӵ�������־
	 * @param log ��־��Ϣ
	 * @return
	 */
	void show_log(const QString & log);

	/**
	 * @brief ���յ���json������ʾ�����ݿ���
	 * @param data ��΢�ŷ���������
	 * @return
	 */
	void show_data(const QString & data);

	/**
	 * @brief ��΢�ŷ���˷�����Ϣ
	 * @param
	 * @return
	 */
	void send_msg(const char * msg);
	void send_msg(std::string & msg);
	void send_msg(QString & msg);
	
public slots:

	/**
	 * @brief ������Ͻ�x��ť�ر�����qt����
	 * @param
	 * @return
	 */
	void on_close_Button_clicked();

	/**
	 * @brief ���������־����¼�
	 * @param
	 * @return
	 */
	void on_clear_op_Button_clicked();

	/**
	 * @brief ���������־����¼�
	 * @param
	 * @return
	 */
	void on_clear_log_Button_clicked();

	/**
	 * @brief ������Ӱ�ť�¼�
	 * @param
	 * @return
	 */
	void on_connect_Button_clicked();

	/**
	 * @brief ��С����ť����¼�
	 * @param
	 * @return
	 */
	void on_min_Button_clicked();

	/**
	 * @brief ws�ͻ������ӳɹ��ص�
	 * @param
	 * @return
	 */
	void ws_client_connected_cb();

	/**
	 * @brief ws�ͻ����յ����ݻص�
	 * @param msg �յ�������
	 * @return
	 */
	void ws_client_received_cb(const QString & msg);

	/**
	 * @brief ��ws�ͻ��˶Ͽ�����ʱ�Ļص�����
	 * @param
	 * @return
	 */
	void ws_client_disconnected_cb();

	/**
	 * @brief ����༭���ѻظ������¼�
	 * @param
	 * @return
	 */
	void on_edit_friend_Button_clicked();

	/**
	 * @brief ����༭Ⱥ�Ļظ������¼�
	 * @param
	 * @return
	 */
	void on_edit_chatroom_Button_clicked();

	/**
	 * @brief ���º��Ѻ�Ⱥ���б�
	 * @param item ����json���ݶ���
	 * @param item2 Ⱥ��json���ݶ���
	 * @return
	 */
	void update_wxuser_list(std::vector<cJSON *> items, std::vector<cJSON *> items2);

	/**
	 * @brief �򿪼��ض���
	 * @param
	 * @return
	 */
	void show_mask();

	/**
	 * @brief �رռ��ض���
	 * @param
	 * @return
	 */
	void hide_mask();

	/**
	 * @brief �����лظ��Ĺ������¼������ڴ�
	 * @param
	 * @return
	 */
	void reload_ini_file();

	/**
	 * @brief �����к��ѻظ��Ĺ������¼������ڴ�
	 * @param
	 * @return
	 */
	void reload_f_ini_file();

	/**
	 * @brief ��ȡwxuser��ʱ�ص�����
	 * @param
	 * @return
	 */
	void get_wxuser_timer_slot();

private:
	Ui::WeChatBotClass ui;

	//��΢��ͨ�ŵ�ws�ͻ���
	QWebSocket * ws_client_ = nullptr;

	//��ʾws�ͻ����Ƿ����������������
	bool is_ws_client_connected_ = false;

	//�������ӷ���˵�ip�Ͷ˿�
	QString server_ip_ = "";
	int server_port_ = -1;

	//������е�Ⱥ����Ϣ
	std::unordered_map<std::string, std::string> all_chatroom_;

	//������еĺ�����Ϣ
	std::unordered_map<std::string, std::string> all_friend_;

public:
	//������е�Ⱥ�Ĺ���
	std::unordered_map<std::string,
		std::vector<std::unordered_map<std::string, 
					std::string>
		>
	> all_chatoom_regs_;
	//������еĺ��ѹ���
	std::unordered_map<std::string,
		std::vector<std::unordered_map<std::string, 
					std::string>
		>
	> all_friend_regs_;
private:
	//maskָ��
	Mask * mask_ = nullptr;

	//addRegָ��
	AddReg * add_reg_ = nullptr;

	//friend��ӹ������ָ��
	FriendAddReg * f_add_reg_ = nullptr;

	//������Ӻ���ȡͨѶ¼��Ϣ���������mask��ˢ����Ϣ��ر�mask�ĳ�ʱ��ʱ��
	QTimer * get_wxuser_timer_ = nullptr;
};
