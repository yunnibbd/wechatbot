#include "tools.h"
#ifdef _WIN32
#include <windows.h> 
#else
#include <iconv.h>
#endif
using namespace std;

std::string UTF8ToGBK(const char * data)
{
	string re = "";
	//1 UFT8 תΪunicode win utf16
#ifdef _WIN32
	//1.1 ͳ��ת�����ֽ���
	int len = MultiByteToWideChar(CP_UTF8,	//ת���ĸ�ʽ
		0,			//Ĭ�ϵ�ת����ʽ
		data,		//������ֽ�
		-1,			//������ַ�����С -1 ��\0
		0,//���
		0//����Ŀռ��С
	);
	if (len <= 0)
		return re;
	wstring udata;
	udata.resize(len);
	MultiByteToWideChar(CP_UTF8, 0, data, -1, (wchar_t*)udata.data(), len);

	//2 unicode תGBK
	len = WideCharToMultiByte(CP_ACP, 0, (wchar_t*)udata.data(), -1, 0, 0,
		0,	//ʧ��Ĭ������ַ�
		0	//�Ƿ�ʹ��Ĭ����� 
	);
	if (len <= 0)
		return re;
	re.resize(len);
	WideCharToMultiByte(CP_ACP, 0, (wchar_t*)udata.data(), -1, (char*)re.data(), len, 0, 0);
#else
	re.resize(1024);
	int inlen = strlen(data);
	Convert((char*)"utf-8", (char*)"gbk", (char*)data, inlen, (char*)re.data(), re.size());
	int outlen = strlen(re.data());
	re.resize(outlen);
#endif
	return re;
}

std::string GBKToUTF8(const char * data)
{
	string re = "";
#ifdef _WIN32	
	//GBKתunicode
	//1.1 ͳ��ת�����ֽ���
	int len = MultiByteToWideChar(CP_ACP,	//ת���ĸ�ʽ
		0,			//Ĭ�ϵ�ת����ʽ
		data,		//������ֽ�
		-1,			//������ַ�����С -1 ��\0
		0,//���
		0//����Ŀռ��С
	);
	if (len <= 0)
		return re;
	wstring udata;
	udata.resize(len);
	MultiByteToWideChar(CP_ACP, 0, data, -1, (wchar_t*)udata.data(), len);

	//2 unicode תutf-8
	len = WideCharToMultiByte(CP_UTF8, 0, (wchar_t*)udata.data(), -1, 0, 0,
		0,	//ʧ��Ĭ������ַ�
		0	//�Ƿ�ʹ��Ĭ����� 
	);
	if (len <= 0)
		return re;
	re.resize(len);
	WideCharToMultiByte(CP_UTF8, 0, (wchar_t*)udata.data(), -1, (char*)re.data(), len, 0, 0);
#else
	re.resize(1024);
	int inlen = strlen(data);
	Convert((char*)"gbk", (char*)"utf-8", (char*)data, inlen, (char*)re.data(), re.size());
	int outlen = strlen(re.data());
	re.resize(outlen);
#endif
	return re;
}
