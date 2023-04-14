#include "Logger.h"
#include "Process.h"


int CreateLogServer(CProcess* proc)
{
	printf("%s(%d):<%s> pid=%d\n", __FILE__, __LINE__, __FUNCTION__, getpid());
	CLoggerServer server;
	int ret = server.Start();
	if (ret != 0) {
		printf("%s(%d):<%s> pid=%d errno:%d msg:%s ret:%d\n",
			__FILE__, __LINE__, __FUNCTION__, getpid(), errno, strerror(errno), ret);
	}
	int fd = 0;
	while (true) {
		ret = proc->RecvFD(fd);
		printf("%s(%d):<%s> fd=%d\n", __FILE__, __LINE__, __FUNCTION__, fd);
		if (fd <= 0)break;
	}
	ret = server.Close();
	printf("%s(%d):<%s> ret=%d\n", __FILE__, __LINE__, __FUNCTION__, ret);
	return 0;
}

int CreateClientServer(CProcess* proc)
{
	printf("%s(%d):<%s> pid=%d\n", __FILE__, __LINE__, __FUNCTION__, getpid());
	int fd = -1;
	int ret = proc->RecvFD(fd);
	printf("%s(%d):<%s> ret=%d\n", __FILE__, __LINE__, __FUNCTION__, ret);
	printf("%s(%d):<%s> fd=%d\n", __FILE__, __LINE__, __FUNCTION__, fd);
	sleep(1);
	char buf[10] = "";
	lseek(fd, 0, SEEK_SET);
	read(fd, buf, sizeof(buf));
	printf("%s(%d):<%s> buf=%s\n", __FILE__, __LINE__, __FUNCTION__, buf);
	close(fd);
	return 0;
}

int LogTest()
{
	char buffer[] = "hello world! ";
	usleep(1000 * 100);
	TRACEI("here is log %d %c %f %g %s ", 10, 'A', 1.0f, 2.0, buffer);
	DUMPD((void*)buffer, (size_t)sizeof(buffer));
	LOGE << 100 << " " << 'S' << " " << 0.12345f << " " << 1.23456789 << " " << buffer << " wodebiancheng";
	return 0;
}

int main()
{
	//CProcess::SwitchDeamon();
	CProcess proclog, procclients;
	printf("%s(%d):<%s> pid=%d\n", __FILE__, __LINE__, __FUNCTION__, getpid());
	proclog.SetEntryFunction(CreateLogServer, &proclog);
	int ret = proclog.CreateSubProcess();
	if (ret != 0) {
		printf("%s(%d):<%s> pid=%d\n", __FILE__, __LINE__, __FUNCTION__, getpid());
		return -1;
	}
	//LogTest();
	printf("%s(%d):<%s> pid=%d\n", __FILE__, __LINE__, __FUNCTION__, getpid());
	CThread thread(LogTest);
	thread.Start();
	procclients.SetEntryFunction(CreateClientServer, &procclients);
	ret = procclients.CreateSubProcess();
	if (ret != 0) {
		printf("%s(%d):<%s> pid=%d\n", __FILE__, __LINE__, __FUNCTION__, getpid());
		return -2;
	}
	printf("%s(%d):<%s> pid=%d\n", __FILE__, __LINE__, __FUNCTION__, getpid());
	usleep(100 * 000);
	int fd = open("./test.txt", O_RDWR | O_CREAT | O_APPEND);
	printf("%s(%d):<%s> fd=%d\n", __FILE__, __LINE__, __FUNCTION__, fd);
	if (fd == -1)return -3;
	ret = procclients.SendFD(fd);
	printf("%s(%d):<%s> ret=%d\n", __FILE__, __LINE__, __FUNCTION__, ret);
	if (ret != 0)printf("errno:%d msg:%s\n", errno, strerror(errno));
	write(fd, "edoyun", 6);
	close(fd);
	proclog.SendFD(-1);
	(void)getchar();
	return 0;
}