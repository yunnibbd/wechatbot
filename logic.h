#pragma once
#include <QObject>
#include <vector>
#include <string>
#include <unordered_map>
#include "cJSON.h"

class WeChatBot;

enum ServerCode
{
	HEART_BEAT = 5005,
	RECV_TXT_MSG = 1,
	RECV_PIC_MSG = 3,
	USER_LIST = 5000,
	GET_USER_LIST_SUCCSESS = 5001,
	GET_USER_LIST_FAIL = 5002,
	TXT_MSG = 555,
	PIC_MSG = 500,
	AT_MSG = 550,
	CHATROOM_MEMBER = 5010,	//����Ⱥ�ĳ�Ա�б�
	CHATROOM_MEMBER_NICK = 5020,
	PERSONAL_INFO = 6500,
	DEBUG_SWITCH = 6000,
	PERSONAL_DETAIL = 6550,
	DESTROY_ALL = 9999,
	NEW_FRIEND_REQUEST = 37,//΢�ź���������Ϣ
	AGREE_TO_FRIEND_REQUEST = 10000,//ͬ��΢�ź���������Ϣ
	ATTATCH_FILE = 5003,
};

class Logic : public QObject
{
	Q_OBJECT

signals:
	void update_wxuser_list(std::vector<cJSON *> items, std::vector<cJSON *> items2);
	void send_msg(std::string msg);
public:
	static Logic & Instance()
	{
		static Logic loc;
		return loc;
	}

	/**
	 * @brief ���������ʱ�����ʱ���ʽ��Ϊÿ��ͨ�ŵ�id
	 * @param
	 * @return
	 */
	QString get_id();

	/**
	 * @brief ����һ����ȡ������ϸ��Ϣ����Ϣ
	 * @param
	 * @return
	 */
	QString get_personal_info();

	/**
	 * @brief ����һ����ø�����ϸ��Ϣ����Ϣ
	 * @param wxid Ҫ�����ϸ��Ϣ�˵�΢��id
	 * @return
	 */
	QString  get_personal_detail(const char * wxid);

	/**
	 * @brief ����һ�����ͨѶ¼�б����Ϣ
	 * @param
	 * @return
	 */
	QString get_contact_list();

	/**
	 * @brief ����һ�������ı���Ϣ����Ϣ
	 * @param wxid Ҫ����˭
	 * @param content Ҫ��ʲô
	 * @return
	 */
	QString send_txt_msg(const char * wxid, const char * content);

	/**
	 * @brief ����һ�������ļ���Ϣ����Ϣ
	 * @param wxid Ҫ����˭
	 * @param img_path Ҫ���͵��ļ�·��
	 * @return
	 */
	QString send_pic_msg(const char * wxid, const char * img_path);

	/**
	 * @brief ����һ����Ⱥ��@������Ϣ����Ϣ
	 * @param wxid Ҫ@�˵�΢��id
	 * @param rooid Ⱥ��id
	 * @param content ��Ϣ����
	 * @param nickname Ҫ@���˵�Ⱥ�ǳ�
	 * @return
	 */
	QString send_at_msg(const char * wxid, const char *  roomid, const char *  content, const char *  nickname);

	/**
	 * @brief ����һ�����Ⱥ��Ա�ǳƵ���Ϣ, �˴����Խ��Ϊ΢��ֱ�ӱ���������̽��
	 * @param wxid �ó�Ա��wxid
	 * @param rooid �ó�Ա����Ⱥid
	 * @return
	 */
	QString get_chat_nick_p(const char * wxid, const char * roomid);

	/**
	 * @brief ����һ����ȡȺ���г�Ա�б����Ϣ
	 * @param
	 * @return
	 */
	QString get_chatroom_memberlist();

	/**
	 * @brief �����ı���Ϣ
	 * @param
	 * @return
	 */
	void handle_txt_msg(cJSON * json);

	/**
	 * @brief ����ͨѶ¼
	 * @param
	 * @return
	 */
	void handle_wxuser_list(cJSON * json);

	/**
	 * @brief �����º��ѻ��³�Ա
	 * @param
	 * @return
	 */
	void handle_new_member(cJSON * json);

	/**
	 * @brief �������г�Ա�б���Ϣ
	 * @param
	 * @return
	 */
	void handle_all_member_list(cJSON * json);

	/**
	 * @brief ������Ϣ���ö�Ӧ�Ĵ�����
	 * @param
	 * @return
	 */
	void dispatch_msg(cJSON * json);

	void set_instance(WeChatBot * ins)
	{
		instance_ = ins;
	}
private:
	Logic()
	{
	}

	//���ڻ�ȡȺ�ĺͺ��ѹ���
	WeChatBot * instance_;

	//�Լ���wxid�����ڻ�ȡ������Ϣ��Ⱥ�ǳƵĽӿ�һֱ������
	std::string my_wxid = "";

	//�洢���е�Ⱥ�б��Ա��Ϣ
	std::unordered_map<std::string,
		std::vector<std::unordered_map<std::string, std::string>>
	> all_chatroom_member_;
};
