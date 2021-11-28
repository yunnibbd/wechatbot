#pragma once

#include <QWidget>
#include <QTimer>
#include "mask.h"
#include "ui_friendaddreg.h"

class FriendAddReg : public QWidget
{
	Q_OBJECT

signals:
	void reload_f_ini_file();

public slots:
	/**
	 * @brief ���һ��ƥ�����ť����¼�
	 * @param
	 * @return
	 */
	void on_reply_add_pushButton_clicked();

	/**
	 * @brief ɾ��һ��ƥ�����ť����¼�
	 * @param
	 * @return
	 */
	void on_reply_del_pushButton_clicked();

	/**
	 * @brief ͬ����ҳ�����ݰ�ť����¼�
	 * @param
	 * @return
	 */
	void on_sync_file_pushButton_clicked();

	/**
	 * @brief ��ʼ����ҳ�棬��ҪΪ��ȡ�����ļ��е����й���
	 * @param
	 * @return
	 */
	void load_init_cb();
public:
	FriendAddReg(QWidget *parent = Q_NULLPTR);
	~FriendAddReg();

	void delay_load();

	void show_mask();
	void hide_mask();

	void set_wxid(QString & id)
	{
		editing_wxid_ = id;
	}
private:
	//��ǰ���ڱ༭��wxid
	QString editing_wxid_;

	//�ӳټ��������ļ��Ķ�ʱ��
	QTimer * load_init_timer_ = nullptr;

	//maskָ��
	Mask * mask_ = nullptr;

	Ui::FriendAddReg ui;
};
