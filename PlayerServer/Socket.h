#pragma once
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <fcntl.h>

class Buffer :public std::string
{
public:
	Buffer() :std::string() {}
	Buffer(size_t size) :std::string() { resize(size); }
	Buffer(const std::string& str) :std::string(str) {}
	Buffer(const char* str) :std::string(str) {}
	operator char* () { return (char*)c_str(); }
	operator char* () const { return (char*)c_str(); }
	operator const char* () const { return c_str(); }
};

enum SockAttr {
	SOCK_ISSERVER = 1,//�Ƿ������ 1��ʾ�� 0��ʾ�ͻ���
	SOCK_ISNONBLOCK = 2,//�Ƿ����� 1��ʾ������ 0��ʾ����
	SOCK_ISUDP = 4,//�Ƿ�ΪUDP 1��ʾudp 0��ʾtcp
};

class CSockParam {
public:
	CSockParam() {
		bzero(&addr_in, sizeof(addr_in));
		bzero(&addr_un, sizeof(addr_un));
		port = -1;
		attr = 0;//Ĭ���ǿͻ��ˡ�������tcp
	}
	CSockParam(const Buffer& ip, short port, int attr) {
		this->ip = ip;
		this->port = port;
		this->attr = attr;
		addr_in.sin_family = AF_INET;
		addr_in.sin_port = port;
		addr_in.sin_addr.s_addr = inet_addr(ip);
	}
	CSockParam(const Buffer& path, int attr) {
		ip = path;
		addr_un.sun_family = AF_UNIX;
		strcpy(addr_un.sun_path, path);
		this->attr = attr;
	}
	~CSockParam() {}
	CSockParam(const CSockParam& param) {
		ip = param.ip;
		port = param.port;
		attr = param.attr;
		memcpy(&addr_in, &param.addr_in, sizeof(addr_in));
		memcpy(&addr_un, &param.addr_un, sizeof(addr_un));
	}
public:
	CSockParam& operator=(const CSockParam& param) {
		if (this != &param) {
			ip = param.ip;
			port = param.port;
			attr = param.attr;
			memcpy(&addr_in, &param.addr_in, sizeof(addr_in));
			memcpy(&addr_un, &param.addr_un, sizeof(addr_un));
		}
		return *this;
	}
	sockaddr* addrin() { return (sockaddr*)&addr_in; }
	sockaddr* addrun() { return (sockaddr*)&addr_un; }
public:
	//��ַ
	sockaddr_in addr_in;
	sockaddr_un addr_un;
	//ip
	Buffer ip;
	//�˿�
	short port;
	//�ο�SockAttr
	int attr;
};

class CSocketBase
{
public:
	CSocketBase() {
		m_socket = -1;
		m_status = 0;//��ʼ��δ���
	}
	//������������
	virtual ~CSocketBase() {
		Close();
	}
public:
	//��ʼ�� ������ �׽��ִ�����bind��listen  �ͻ��� �׽��ִ���
	virtual int Init(const CSockParam& param) = 0;
	//���� ������ accept �ͻ��� connect  ����udp������Ժ���
	virtual int Link(CSocketBase** pClient = NULL) = 0;
	//��������
	virtual int Send(const Buffer& data) = 0;
	//��������
	virtual int Recv(Buffer& data) = 0;
	//�ر�����
	virtual int Close() {
		m_status = 3;
		if (m_socket != -1) {
			unlink(m_param.ip);
			int fd = m_socket;
			m_socket = -1;
			close(fd);
		}
		return 0;
	};
	virtual operator int() { return m_socket; }
	virtual operator int()const { return m_socket; }
protected:
	//�׽�����������Ĭ����-1
	int m_socket;
	//״̬ 0��ʼ��δ��� 1��ʼ����� 2������� 3�Ѿ��ر�
	int m_status;
	//��ʼ������
	CSockParam m_param;
};

class CLocalSocket
	:public CSocketBase
{
public:
	CLocalSocket() :CSocketBase() {}
	CLocalSocket(int sock) :CSocketBase() {
		m_socket = sock;
	}
	//������������
	virtual ~CLocalSocket() {
		Close();
	}
public:
	//��ʼ�� ������ �׽��ִ�����bind��listen  �ͻ��� �׽��ִ���
	virtual int Init(const CSockParam& param) {
		if (m_status != 0)return -1;
		m_param = param;
		int type = (m_param.attr & SOCK_ISUDP) ? SOCK_DGRAM : SOCK_STREAM;
		if (m_socket == -1)
			m_socket = socket(PF_LOCAL, type, 0);
		else
			m_status = 2;//accept�����׽��֣��Ѿ���������״̬
		if (m_socket == -1)return -2;
		int ret = 0;
		if (m_param.attr & SOCK_ISSERVER) {
			ret = bind(m_socket, m_param.addrun(), sizeof(sockaddr_un));
			if (ret == -1) return -3;
			ret = listen(m_socket, 32);
			if (ret == -1)return -4;
		}
		if (m_param.attr & SOCK_ISNONBLOCK) {
			int option = fcntl(m_socket, F_GETFL);
			if (option == -1)return -5;
			option |= O_NONBLOCK;
			ret = fcntl(m_socket, F_SETFL, option);
			if (ret == -1)return -6;
		}
		if (m_status == 0)
			m_status = 1;
		return 0;
	}
	//���� ������ accept �ͻ��� connect  ����udp������Ժ���
	virtual int Link(CSocketBase** pClient = NULL) {
		if (m_status <= 0 || (m_socket == -1))return -1;
		int ret = 0;
		if (m_param.attr & SOCK_ISSERVER) {
			if (pClient == NULL)return -2;
			CSockParam param;
			socklen_t len = sizeof(sockaddr_un);
			int fd = accept(m_socket, param.addrun(), &len);
			if (fd == -1)return -3;
			*pClient = new CLocalSocket(fd);
			if (*pClient == NULL)return -4;
			ret = (*pClient)->Init(param);
			if (ret != 0) {
				delete (*pClient);
				*pClient = NULL;
				return -5;
			}
		}
		else {
			ret = connect(m_socket, m_param.addrun(), sizeof(sockaddr_un));
			if (ret != 0)return -6;
		}
		m_status = 2;
		return 0;
	}
	//��������
	virtual int Send(const Buffer& data) {
		if (m_status < 2 || (m_socket == -1))return -1;
		ssize_t index = 0;
		while (index < (ssize_t)data.size()) {
			ssize_t len = write(m_socket, (char*)data + index, data.size() - index);
			if (len == 0)return -2;
			if (len < 0)return -3;
			index += len;
		}
		return 0;
	}
	//�������� �����㣬��ʾ���ճɹ� С�� ��ʾʧ�� ����0 ��ʾû���յ����ݣ���û�д���
	virtual int Recv(Buffer& data) {
		if (m_status < 2 || (m_socket == -1))return -1;
		ssize_t len = read(m_socket, data, data.size());
		if (len > 0) {
			data.resize(len);
			return (int)len;//�յ�����
		}
		if (len < 0) {
			if (errno == EINTR || (errno == EAGAIN)) {
				data.clear();
				return 0;//û�������յ�
			}
			return -2;//���ʹ���
		}
		return -3;//�׽��ֱ��ر�
	}
	//�ر�����
	virtual int Close() {
		return CSocketBase::Close();
	}
};