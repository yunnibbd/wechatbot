#pragma once

#include <QWidget>
#include <QTimer>
#include "ui_addreg.h"
#include "mask.h"

class AddReg : public QWidget
{
	Q_OBJECT

signals:
	void reload_ini_file();
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
	 * @brief ���һ���³�Ա��Ⱥ�ظ�
	 * @param
	 * @return
	 */
	void on_new_member_add_pushButton_clicked();

	/**
	 * @brief ɾ��ѡ�е��³�Ա��Ⱥ�ظ�
	 * @param
	 * @return
	 */
	void on_new_member_del_pushButton_clicked();

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
	AddReg(QWidget *parent = Q_NULLPTR);
	~AddReg();

	void show_mask();
	void hide_mask();

	void set_wxid(QString & id)
	{
		editing_wxid_ = id;
	}

	void delay_load();
private:
	//��ǰ���ڱ༭��wxid
	QString editing_wxid_;
	//maskָ��
	Mask * mask_ = nullptr;
	//�ӳټ��������ļ��Ķ�ʱ��
	QTimer * load_init_timer_ = nullptr;
	Ui::AddReg ui;
};
