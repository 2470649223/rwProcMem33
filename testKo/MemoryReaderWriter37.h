﻿#ifndef MEMORY_READER_WRITER_H_
#define MEMORY_READER_WRITER_H_

#ifdef __linux__
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <errno.h>
#include <malloc.h>
#include "cvector.h"

//默认驱动文件名
#define RWPROCMEM_FILE_NODE "/dev/rwProcMem37"

//安静输出模式
//#define QUIET_PRINTF

#ifdef QUIET_PRINTF
#undef TRACE
#define TRACE(fmt, ...)
#else
#define TRACE(fmt, ...) printf(fmt, ##__VA_ARGS__)
#endif


#ifndef __cplusplus
#define true 1
#define false 0
typedef int bool;
#endif

typedef int BOOL;
#define TRUE 1
#define FALSE 0

#define PAGE_NOACCESS 1
#define PAGE_READONLY 2
#define PAGE_READWRITE 4
#define PAGE_WRITECOPY 8
#define PAGE_EXECUTE 16
#define PAGE_EXECUTE_READ 32
#define PAGE_EXECUTE_READWRITE 64

#define MEM_MAPPED 262144
#define MEM_PRIVATE 131072

#define MAJOR_NUM 100
#define IOCTL_SET_MAX_DEV_FILE_OPEN				_IOWR(MAJOR_NUM, 0, char*) //设置驱动设备接口文件允许同时被使用的最大值
#define IOCTL_HIDE_KERNEL_MODULE						_IOWR(MAJOR_NUM, 1, char*) //隐藏驱动（卸载驱动需重启机器）
#define IOCTL_OPEN_PROCESS 								_IOWR(MAJOR_NUM, 2, char*) //打开进程
#define IOCTL_CLOSE_HANDLE 								_IOWR(MAJOR_NUM, 3, char*) //关闭进程
#define IOCTL_GET_PROCESS_MAPS_COUNT			_IOWR(MAJOR_NUM, 4, char*) //获取进程的内存块地址数量
#define IOCTL_GET_PROCESS_MAPS_LIST				_IOWR(MAJOR_NUM, 5, char*) //获取进程的内存块地址列表
#define IOCTL_CHECK_PROCESS_ADDR_PHY			_IOWR(MAJOR_NUM, 6, char*) //检查进程内存是否有物理内存位置
#define IOCTL_GET_PROCESS_PID_LIST					_IOWR(MAJOR_NUM, 7, char*) //获取进程PID列表
#define IOCTL_GET_PROCESS_GROUP						_IOWR(MAJOR_NUM, 8, char*) //获取进程权限等级
#define IOCTL_SET_PROCESS_ROOT							_IOWR(MAJOR_NUM, 9, char*) //提升进程权限到Root
#define IOCTL_GET_PROCESS_RSS							_IOWR(MAJOR_NUM, 10, char*) //获取进程的物理内存占用大小
#define IOCTL_GET_PROCESS_CMDLINE_ADDR		_IOWR(MAJOR_NUM, 11, char*) //获取进程cmdline的内存地址

#endif /*__linux__*/

#pragma pack(1)
typedef struct {
	uint64_t baseaddress;
	uint64_t size;
	uint32_t protection;
	uint32_t type;
	char name[4096];
} DRIVER_REGION_INFO, *PDRIVER_REGION_INFO;
#pragma pack()

#ifdef __linux__
//声明
//////////////////////////////////////////////////////////////////////////
//C语言形式接口：
/////////////////////////////////////////////////////////////////////////

//连接驱动（驱动节点文件路径名），返回值：驱动连接句柄，>=0代表成功
static inline int rwProcMemDriver_Connect(const char* lpszDriverFileNode);

//断开驱动（驱动连接句柄），返回值：TRUE成功，FALSE失败
static inline BOOL rwProcMemDriver_Disconnect(int nDriverLink);


//驱动_设置驱动接口文件允许同时被使用的最大值（驱动连接句柄，最大值），返回值：TRUE成功，FALSE失败
static inline BOOL rwProcMemDriver_SetMaxDevFileOpen(int nDriverLink, uint64_t max);


//驱动_隐藏驱动（驱动连接句柄），返回值：TRUE成功，FALSE失败
static inline BOOL rwProcMemDriver_HideKernelModule(int nDriverLink);


//驱动_打开进程（驱动连接句柄，进程PID），返回值：进程句柄，0为失败
static inline uint64_t rwProcMemDriver_OpenProcess(int nDriverLink, uint64_t pid);

//驱动_读取进程内存（驱动连接句柄，进程句柄，进程内存地址，读取结果缓冲区，读取结果缓冲区大小，实际读取字节数，是否暴力读取），返回值：TRUE成功，FALSE失败
static inline BOOL rwProcMemDriver_ReadProcessMemory(
	int nDriverLink,
	uint64_t hProcess,
	uint64_t lpBaseAddress,
	void * lpBuffer,
	size_t nSize,
	size_t * lpNumberOfBytesRead,
	BOOL bIsForceRead = FALSE
);

//驱动_读取进程内存_单线程极速版（驱动连接句柄，进程句柄，进程内存地址，读取结果缓冲区，读取结果缓冲区大小，实际读取字节数，是否暴力读取），返回值：TRUE成功，FALSE失败
static inline BOOL rwProcMemDriver_ReadProcessMemory_Fast(
	int nDriverLink,
	uint64_t hProcess,
	uint64_t lpBaseAddress,
	void * lpBuffer,
	size_t nSize,
	size_t * lpNumberOfBytesRead,
	BOOL bIsForceRead = FALSE
);



//驱动_写入进程内存（驱动连接句柄，进程句柄，进程内存地址，写入数据缓冲区，写入数据缓冲区大小，实际写入字节数，是否暴力写入），返回值：TRUE成功，FALSE失败
static inline BOOL rwProcMemDriver_WriteProcessMemory(
	int nDriverLink,
	uint64_t hProcess,
	uint64_t lpBaseAddress,
	void * lpBuffer,
	size_t nSize,
	size_t * lpNumberOfBytesWritten,
	BOOL bIsForceWrite = FALSE
);

//驱动_写入进程内存_单线程极速版（驱动连接句柄，进程句柄，进程内存地址，写入数据缓冲区，写入数据缓冲区大小，实际写入字节数，是否暴力写入），返回值：TRUE成功，FALSE失败
static inline BOOL rwProcMemDriver_WriteProcessMemory_Fast(
	int nDriverLink,
	uint64_t hProcess,
	uint64_t lpBaseAddress,
	void * lpBuffer,
	size_t nSize,
	size_t * lpNumberOfBytesWritten,
	BOOL bIsForceWrite = FALSE
);

//驱动_关闭进程（驱动连接句柄，进程句柄），返回值：TRUE成功，FALSE失败
static inline BOOL rwProcMemDriver_CloseHandle(int nDriverLink, uint64_t hProcess);

//驱动_获取进程内存块列表（驱动连接句柄，进程句柄，是否仅显示物理内存，输出缓冲区，输出是否完整），返回值：TRUE成功，FALSE失败
//（参数showPhy说明: FALSE为显示全部内存，TRUE为只显示在物理内存中的内存，注意：如果进程内存不存在于物理内存中，驱动将无法读取该内存位置的值）
//（参数bOutListCompleted说明: 若输出FALSE，则代表输出缓冲区里的进程内存块列表不完整，若输出TRUE，则代表输出缓冲区里的进程内存块列表完整可靠）
static BOOL rwProcMemDriver_VirtualQueryExFull(int nDriverLink, uint64_t hProcess, BOOL showPhy, cvector vOutput, BOOL * bOutListCompleted);


//驱动_获取进程PID列表（驱动连接句柄，获取方式，输出缓冲区，输出是否完整），返回值：TRUE成功，FALSE失败
//（参数bIsSpeedMode说明: FALSE为稳定模式，TRUE为极速模式）
//（参数bOutListCompleted说明: 若输出FALSE，则代表输出缓冲区里的进程PID列表不完整，若输出TRUE，则代表输出缓冲区里的进程PID列表完整可靠）
static inline BOOL rwProcMemDriver_GetProcessPidList(int nDriverLink, BOOL bIsSpeedMode, cvector vOutput, BOOL * bOutListCompleted);

//驱动_获取进程权限等级（驱动连接句柄，进程句柄，输出UID，输出SUID，输出EUID，输出FSUID，输出GID，输出SGID，输出EGID，输出FSGID），返回值：TRUE成功，FALSE失败
static inline BOOL rwProcMemDriver_GetProcessGroup(int nDriverLink, uint64_t hProcess,
	uint64_t *nOutUID,
	uint64_t *nOutSUID,
	uint64_t *nOutEUID,
	uint64_t *nOutFSUID,
	uint64_t *nOutGID,
	uint64_t *nOutSGID,
	uint64_t *nOutEGID,
	uint64_t *nOutFSGID);

//驱动_提升进程权限到Root（驱动连接句柄，进程句柄），返回值：TRUE成功，FALSE失败
static inline BOOL rwProcMemDriver_SetProcessRoot(int nDriverLink, uint64_t hProcess);


//驱动_获取进程占用物理内存大小（驱动连接句柄，进程句柄，输出的占用物理内存大小），返回值：TRUE成功，FALSE失败
static inline BOOL rwProcMemDriver_GetProcessRSS(int nDriverLink, uint64_t hProcess, uint64_t *outRss);


//驱动_获取进程命令行（驱动连接句柄，进程句柄，输出缓冲区，输出缓冲区的大小），返回值：TRUE成功，FALSE失败
static inline BOOL rwProcMemDriver_GetProcessCmdline(int nDriverLink, uint64_t hProcess, char *lpOutCmdlineBuf, size_t bufSize);
#endif

//////////////////////////////////////////////////////////////////////////
//C++语言形式接口：
/////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
#include <vector>
#include <mutex>
#include "IMemReaderWriterProxy.h"
class CMemoryReaderWriter : public IMemReaderWriterProxy
{
public:

	CMemoryReaderWriter()
	{

	}
	~CMemoryReaderWriter()
	{
		DisconnectDriver();
	}

	//连接驱动（驱动节点文件路径名，错误代码），返回值：驱动连接句柄，>=0代表成功
	BOOL ConnectDriver(const char* lpszDriverFileNode, int & err)
	{
#ifdef __linux__
		if (m_nDriverLink >= 0) { return TRUE; }
		m_nDriverLink = rwProcMemDriver_Connect(lpszDriverFileNode);
		if (m_nDriverLink < 0)
		{
			err = m_nDriverLink;
			return FALSE;
		}
		else
		{
			err = 0;
		}
		return TRUE;
#else 
		return FALSE;
#endif
	}

	//断开驱动，返回值：TRUE成功，FALSE失败
	BOOL DisconnectDriver()
	{
#ifdef __linux__
		if (m_nDriverLink >= 0)
		{
			rwProcMemDriver_Disconnect(m_nDriverLink);
			m_nDriverLink = -1;
			return TRUE;
		}
#endif
		return FALSE;
	}

	//驱动是否连接正常，返回值：TRUE已连接，FALSE未连接
	BOOL IsDriverConnected()
	{
#ifdef __linux__
		return m_nDriverLink >= 0 ? TRUE : FALSE;
#else
		return FALSE;
#endif
	}


	//驱动_设置驱动接口文件允许同时被使用的最大值（最大值），返回值：TRUE成功，FALSE失败
	BOOL SetMaxDevFileOpen(uint64_t max)
	{
#ifdef __linux__
		return rwProcMemDriver_SetMaxDevFileOpen(m_nDriverLink, max);
#else
		return FALSE;
#endif
	}


	//驱动_隐藏驱动（），返回值：TRUE成功，FALSE失败
	BOOL HideKernelModule()
	{
#ifdef __linux__
		return rwProcMemDriver_HideKernelModule(m_nDriverLink);
#else
		return FALSE;
#endif
	}

	//驱动_打开进程（进程PID），返回值：进程句柄，0为失败
	uint64_t OpenProcess(uint64_t pid)
	{
#ifdef __linux__
		return rwProcMemDriver_OpenProcess(m_nDriverLink, pid);
#else
		return FALSE;
#endif
	}

	//驱动_读取进程内存（进程句柄，进程内存地址，读取结果缓冲区，读取结果缓冲区大小，实际读取字节数，是否暴力读取），返回值：TRUE成功，FALSE失败
	BOOL ReadProcessMemory(
			uint64_t hProcess,
			uint64_t lpBaseAddress,
			void *lpBuffer,
			size_t nSize,
			size_t * lpNumberOfBytesRead,
			BOOL bIsForceRead = FALSE) override
	{
#ifdef __linux__
		return rwProcMemDriver_ReadProcessMemory(
			m_nDriverLink,
			hProcess,
			lpBaseAddress,
			lpBuffer,
			nSize,
			lpNumberOfBytesRead,
			bIsForceRead);
#else
		return FALSE;
#endif
	}

	//驱动_读取进程内存_单线程极速版（进程句柄，进程内存地址，读取结果缓冲区，读取结果缓冲区大小，实际读取字节数，是否暴力读取），返回值：TRUE成功，FALSE失败
	BOOL ReadProcessMemory_Fast(
		uint64_t hProcess,
		uint64_t lpBaseAddress,
		void *lpBuffer,
		size_t nSize,
		size_t * lpNumberOfBytesRead,
		BOOL bIsForceRead = FALSE) override
	{
#ifdef __linux__
		return rwProcMemDriver_ReadProcessMemory_Fast(
			m_nDriverLink,
			hProcess,
			lpBaseAddress,
			lpBuffer,
			nSize,
			lpNumberOfBytesRead,
			bIsForceRead);
#else
		return FALSE;
#endif
	}

	//驱动_写入进程内存（进程句柄，进程内存地址，写入数据缓冲区，写入数据缓冲区大小，实际写入字节数，是否暴力写入），返回值：TRUE成功，FALSE失败
	BOOL WriteProcessMemory(
		uint64_t hProcess,
		uint64_t lpBaseAddress,
		void * lpBuffer,
		size_t nSize,
		size_t * lpNumberOfBytesWritten,
		BOOL bIsForceWrite = FALSE) override
	{
#ifdef __linux__
		return rwProcMemDriver_WriteProcessMemory(
			m_nDriverLink,
			hProcess,
			lpBaseAddress,
			lpBuffer,
			nSize,
			lpNumberOfBytesWritten,
			bIsForceWrite);
#else
		return FALSE;
#endif
	}



	//驱动_写入进程内存_单线程极速版（进程句柄，进程内存地址，写入数据缓冲区，写入数据缓冲区大小，实际写入字节数，是否暴力写入），返回值：TRUE成功，FALSE失败
	BOOL WriteProcessMemory_Fast(
		uint64_t hProcess,
		uint64_t lpBaseAddress,
		void * lpBuffer,
		size_t nSize,
		size_t * lpNumberOfBytesWritten,
		BOOL bIsForceWrite = FALSE) override
	{
#ifdef __linux__
		return rwProcMemDriver_WriteProcessMemory_Fast(
			m_nDriverLink,
			hProcess,
			lpBaseAddress,
			lpBuffer,
			nSize,
			lpNumberOfBytesWritten,
			bIsForceWrite);
#else
		return FALSE;
#endif
	}

	//驱动_关闭进程（进程句柄），返回值：TRUE成功，FALSE失败
	BOOL CloseHandle(uint64_t hProcess)
	{
#ifdef __linux__
		return rwProcMemDriver_CloseHandle(m_nDriverLink, hProcess);
#else
		return FALSE;
#endif
	}


	//驱动_获取进程内存块列表（进程句柄，是否仅显示物理内存，输出缓冲区，输出是否完整），返回值：TRUE成功，FALSE失败
	//（参数showPhy说明: FALSE为显示全部内存，TRUE为只显示在物理内存中的内存，注意：如果进程内存不存在于物理内存中，驱动将无法读取该内存位置的值）
	//（参数bOutListCompleted说明: 若输出FALSE，则代表输出缓冲区里的进程内存块列表不完整，若输出TRUE，则代表输出缓冲区里的进程内存块列表完整可靠）
	BOOL VirtualQueryExFull(uint64_t hProcess, BOOL showPhy, std::vector<DRIVER_REGION_INFO> & vOutput, BOOL & bOutListCompleted) override
	{
#ifdef __linux__
		cvector cvOutput = cvector_create(sizeof(DRIVER_REGION_INFO));
		BOOL b = rwProcMemDriver_VirtualQueryExFull(m_nDriverLink, hProcess, showPhy, cvOutput, &bOutListCompleted);
		for (citerator iter = cvector_begin(cvOutput); iter != cvector_end(cvOutput); iter = cvector_next(cvOutput, iter))
		{
			DRIVER_REGION_INFO *rinfo = (DRIVER_REGION_INFO*)iter;
			vOutput.push_back(*rinfo);
		}
		cvector_destroy(cvOutput);
		return b;
#else
		return FALSE;
#endif
	}

	//驱动_获取进程PID列表（获取方式，获取方式，输出缓冲区，输出是否完整），返回值：TRUE成功，FALSE失败
	//（参数bIsSpeedMode说明: FALSE为稳定模式，TRUE为极速模式）
	//（参数bOutListCompleted说明: 若输出FALSE，则代表输出缓冲区里的进程PID列表不完整，若输出TRUE，则代表输出缓冲区里的进程PID列表完整可靠）
	BOOL GetProcessPidList(std::vector<int> & vOutput, BOOL bIsSpeedMode, BOOL & bOutListCompleted)
	{
#ifdef __linux__
		cvector cvOutput = cvector_create(sizeof(int));
		BOOL b = rwProcMemDriver_GetProcessPidList(m_nDriverLink, bIsSpeedMode, cvOutput, &bOutListCompleted);
		for (citerator iter = cvector_begin(cvOutput); iter != cvector_end(cvOutput); iter = cvector_next(cvOutput, iter))
		{
			int *p = (int*)iter;
			vOutput.push_back(*p);
		}
		cvector_destroy(cvOutput);
		return b;
#else
		return FALSE;
#endif
	}


	//驱动_获取进程权限等级（驱动连接句柄，进程句柄，输出UID，输出SUID，输出EUID，输出FSUID，输出GID，输出SGID，输出EGID，输出FSGID），返回值：TRUE成功，FALSE失败
	BOOL GetProcessGroup(uint64_t hProcess,
		uint64_t & nOutUID,
		uint64_t & nOutSUID,
		uint64_t & nOutEUID,
		uint64_t & nOutFSUID,
		uint64_t & nOutGID,
		uint64_t & nOutSGID,
		uint64_t & nOutEGID,
		uint64_t & nOutFSGID)
	{
#ifdef __linux__
		return rwProcMemDriver_GetProcessGroup(m_nDriverLink, hProcess,
			&nOutUID,
			&nOutSUID,
			&nOutEUID,
			&nOutFSUID,
			&nOutGID,
			&nOutSGID,
			&nOutEGID,
			&nOutFSGID);
#else
		return FALSE;
#endif
	}

	//驱动_提升进程权限到Root（进程句柄），返回值：TRUE成功，FALSE失败
	BOOL SetProcessRoot(uint64_t hProcess)
	{
#ifdef __linux__
		return rwProcMemDriver_SetProcessRoot(m_nDriverLink, hProcess);
#else
		return FALSE;
#endif
	}

	//驱动_获取进程占用物理内存大小（进程句柄，输出的占用物理内存大小），返回值：TRUE成功，FALSE失败
	BOOL GetProcessRSS(uint64_t hProcess, uint64_t & outRss)
	{
#ifdef __linux__
		return rwProcMemDriver_GetProcessRSS(m_nDriverLink, hProcess, &outRss);
#else
		return FALSE;
#endif
	}

	//驱动_获取进程命令行（进程句柄，输出缓冲区，输出缓冲区的大小），返回值：TRUE成功，FALSE失败
	BOOL GetProcessCmdline(uint64_t hProcess, char *lpOutCmdlineBuf, size_t bufSize)
	{
#ifdef __linux__
		return rwProcMemDriver_GetProcessCmdline(m_nDriverLink, hProcess, lpOutCmdlineBuf, bufSize);
#else
		return FALSE;
#endif
	}

	//获取驱动连接FD，返回值：驱动连接的FD
	int GetLinkFD()
	{
#ifdef __linux__
		return m_nDriverLink;
#else
		return -1;
#endif
	}

	//设置驱动连接FD
	void SetLinkFD(int fd)
	{
#ifdef __linux__
		m_nDriverLink = fd;
#endif
	}


private:
	int m_nDriverLink = -1;
};

#endif

#ifdef __linux__
//实现
//////////////////////////////////////////////////////////////////////////


static inline int rwProcMemDriver_Connect(const char * lpszDriverFileNode)
{
	int nDriverLink = open(lpszDriverFileNode, O_RDWR);
	if (nDriverLink < 0)
	{
		int last_err = errno;
		TRACE("open error():%s\n", strerror(last_err));
		return -last_err;
	}
	return nDriverLink;
}
static inline BOOL rwProcMemDriver_Disconnect(int nDriverLink)
{
	if (nDriverLink < 0) { return FALSE; }
	//�Ͽ���������
	close(nDriverLink);
	return TRUE;
}
static inline BOOL rwProcMemDriver_SetMaxDevFileOpen(int nDriverLink, uint64_t max)
{
	if (nDriverLink < 0) { return FALSE; }
	char buf[8] = { 0 };
	*(uint64_t*)&buf[0] = max;

	int res = ioctl(nDriverLink, IOCTL_SET_MAX_DEV_FILE_OPEN, (unsigned long)&buf, sizeof(buf));
	if (res != 0)
	{
		TRACE("SetMaxDevFileOpen ioctl():%s\n", strerror(errno));
		return FALSE;
	}
	return TRUE;
}


static inline BOOL rwProcMemDriver_HideKernelModule(int nDriverLink)
{
	if (nDriverLink < 0) { return FALSE; }
	int res = ioctl(nDriverLink, IOCTL_HIDE_KERNEL_MODULE, 0, 0);
	if (res != 0)
	{
		TRACE("HideKernelModule ioctl():%s\n", strerror(errno));
		return FALSE;
	}
	return TRUE;
}



static inline uint64_t rwProcMemDriver_OpenProcess(int nDriverLink, uint64_t pid)
{
	if (nDriverLink < 0) { return 0; }
	char buf[8] = { 0 };
	*(uint64_t*)&buf[0] = pid;

	int res = ioctl(nDriverLink, IOCTL_OPEN_PROCESS, (unsigned long)&buf, sizeof(buf));
	if (res != 0)
	{
		TRACE("OpenProcess ioctl():%s\n", strerror(errno));
		return 0;
	}
	uint64_t ptr = *(uint64_t*)&buf[0];
	return ptr;
}
static inline BOOL rwProcMemDriver_ReadProcessMemory(
	int nDriverLink,
	uint64_t hProcess,
	uint64_t lpBaseAddress,
	void * lpBuffer,
	size_t nSize,
	size_t * lpNumberOfBytesRead,
	BOOL bIsForceRead)
{

	if (lpBaseAddress <= 0) { return FALSE; }
	if (nDriverLink < 0) { return FALSE; }
	if (!hProcess) { return FALSE; }
	if (nSize <= 0) { return FALSE; }
	int bufSize = nSize < 17 ? 17 : nSize;

	char *buf = (char*)malloc(bufSize);
	memset(buf, 0, bufSize);
	*(uint64_t*)&buf[0] = hProcess;
	*(uint64_t*)&buf[8] = lpBaseAddress;
	buf[16] = bIsForceRead == TRUE ? '\x01' : '\x00';
	ssize_t realRead = read(nDriverLink, buf, nSize);
	if (realRead <= 0)
	{
		TRACE("read(): %s\n", strerror(errno));
		free(buf);
		return FALSE;
	}
	if (realRead > 0)
	{
		memcpy(lpBuffer, buf, realRead);
	}

	if (lpNumberOfBytesRead)
	{
		*lpNumberOfBytesRead = realRead;
	}
	free(buf);
	return TRUE;

}

static inline BOOL rwProcMemDriver_ReadProcessMemory_Fast(
	int nDriverLink,
	uint64_t hProcess,
	uint64_t lpBaseAddress,
	void * lpBuffer,
	size_t nSize,
	size_t * lpNumberOfBytesRead,
	BOOL bIsForceRead)
{

	if (lpBaseAddress <= 0) { return FALSE; }
	if (nDriverLink < 0) { return FALSE; }
	if (!hProcess) { return FALSE; }
	if (nSize <= 0) { return FALSE; }
	int bufSize = nSize < 17 ? 17 : nSize;


	//上一次读内存申请的缓冲区，下一次继续用，可以提速
	static char* lastMallocReadMemBuf = NULL;
	static size_t lastMallocReadMemSize = 0;

	if (lastMallocReadMemSize < bufSize)
	{
		if (lastMallocReadMemBuf) { free(lastMallocReadMemBuf); }
		lastMallocReadMemBuf = (char*)malloc(bufSize);
		lastMallocReadMemSize = bufSize;
	}
	memset(lastMallocReadMemBuf, 0, bufSize);
	*(uint64_t*)&lastMallocReadMemBuf[0] = hProcess;
	*(uint64_t*)&lastMallocReadMemBuf[8] = lpBaseAddress;
	lastMallocReadMemBuf[16] = bIsForceRead == TRUE ? '\x01' : '\x00';
	
	ssize_t realRead = read(nDriverLink, lastMallocReadMemBuf, nSize);

	if (realRead <= 0)
	{
		TRACE("read(): %s\n", strerror(errno));
		return FALSE;
	}
	if (realRead > 0)
	{
		memcpy(lpBuffer, lastMallocReadMemBuf, realRead);
	}

	if (lpNumberOfBytesRead)
	{
		*lpNumberOfBytesRead = realRead;
	}
	return TRUE;

}


static inline BOOL rwProcMemDriver_WriteProcessMemory(
	int nDriverLink,
	uint64_t hProcess,
	uint64_t lpBaseAddress,
	void * lpBuffer,
	size_t nSize,
	size_t * lpNumberOfBytesWritten,
	BOOL bIsForceWrite)
{
	if (lpBaseAddress <= 0) { return FALSE; }
	if (nDriverLink < 0) { return FALSE; }
	if (!hProcess) { return FALSE; }
	if (nSize <= 0) { return FALSE; }
	int bufSize = nSize + 17;


	char *buf = (char*)malloc(bufSize);
	memset(buf, 0, bufSize);
	*(uint64_t*)&buf[0] = hProcess;
	*(uint64_t*)&buf[8] = lpBaseAddress;
	buf[16] = bIsForceWrite == TRUE ? '\x01' : '\x00';
	memcpy((void*)((size_t)buf + (size_t)17), lpBuffer, nSize);

	ssize_t realWrite = write(nDriverLink, buf, nSize);
	if (realWrite <= 0)
	{
		TRACE("write(): %s\n", strerror(errno));
		free(buf);
		return FALSE;
	}

	if (lpNumberOfBytesWritten)
	{
		*lpNumberOfBytesWritten = realWrite;
	}
	free(buf);
	return TRUE;

}
static inline BOOL rwProcMemDriver_WriteProcessMemory_Fast(
	int nDriverLink,
	uint64_t hProcess,
	uint64_t lpBaseAddress,
	void * lpBuffer,
	size_t nSize,
	size_t * lpNumberOfBytesWritten,
	BOOL bIsForceWrite)
{
	if (lpBaseAddress <= 0) { return FALSE; }
	if (nDriverLink < 0) { return FALSE; }
	if (!hProcess) { return FALSE; }
	if (nSize <= 0) { return FALSE; }
	int bufSize = nSize + 17;


	//上一次读内存申请的缓冲区，下一次继续用，可以提速
	static char* lastMallocWriteMemBuf = NULL;
	static size_t lastMallocWriteMemSize = 0;

	if (lastMallocWriteMemSize < bufSize)
	{
		if (lastMallocWriteMemBuf) { free(lastMallocWriteMemBuf); }
		lastMallocWriteMemBuf = (char*)malloc(bufSize);
		lastMallocWriteMemSize = bufSize;
	}
	*(uint64_t*)&lastMallocWriteMemBuf[0] = hProcess;
	*(uint64_t*)&lastMallocWriteMemBuf[8] = lpBaseAddress;
	lastMallocWriteMemBuf[16] = bIsForceWrite == TRUE ? '\x01' : '\x00';
	memcpy((void*)((size_t)lastMallocWriteMemBuf + (size_t)17), lpBuffer, nSize);

	ssize_t realWrite = write(nDriverLink, lastMallocWriteMemBuf, nSize);
	if (realWrite <= 0)
	{
		TRACE("write(): %s\n", strerror(errno));
		return FALSE;
	}

	if (lpNumberOfBytesWritten)
	{
		*lpNumberOfBytesWritten = realWrite;
	}
	return TRUE;

}

static inline BOOL rwProcMemDriver_CloseHandle(int nDriverLink, uint64_t hProcess)
{
	if (nDriverLink < 0) { return FALSE; }
	if (!hProcess) { return FALSE; }

	char buf[8] = { 0 };
	*(uint64_t*)&buf[0] = hProcess;

	int res = ioctl(nDriverLink, IOCTL_CLOSE_HANDLE, (unsigned long)&buf, sizeof(buf));
	if (res != 0)
	{
		TRACE("CloseHandle ioctl():%s\n", strerror(errno));
		return FALSE;
	}
	return TRUE;
}


static BOOL rwProcMemDriver_VirtualQueryExFull(int nDriverLink, uint64_t hProcess, BOOL showPhy, cvector vOutput, BOOL * bOutListCompleted)
{
	if (nDriverLink < 0) { return FALSE; }
	if (!hProcess) { return FALSE; }
	char buf[8] = { 0 };
	*(uint64_t*)&buf[0] = hProcess;
	int count = ioctl(nDriverLink, IOCTL_GET_PROCESS_MAPS_COUNT, (unsigned long)&buf, sizeof(buf));
	TRACE("VirtualQueryExFull count %d\n", count);
	if (count <= 0)
	{
		TRACE("VirtualQueryExFull ioctl():%s\n", strerror(errno));
		return FALSE;
	}

	uint64_t big_buf_len = (1 + 8 + 8 + 4 + 4096) * (count + 50);
	char * big_buf = (char*)malloc(big_buf_len);
	memset(big_buf, 0, big_buf_len);
	*(uint64_t*)&big_buf[0] = hProcess;

	uint64_t name_len = 4096;
	*(uint64_t*)&big_buf[8] = name_len;
	*(uint64_t*)&big_buf[16] = big_buf_len;

	int res = ioctl(nDriverLink, IOCTL_GET_PROCESS_MAPS_LIST, (unsigned long)big_buf, big_buf_len);
	TRACE("VirtualQueryExFull res %d\n", res);
	if (res <= 0)
	{
		TRACE("VirtualQueryExFull ioctl():%s\n", strerror(errno));
		free(big_buf);
		return FALSE;
	}
	size_t copy_pos = (size_t)big_buf;
	unsigned char pass = *(unsigned char*)big_buf;
	if (pass == '\x01')
	{
		//数据不完整
		if (bOutListCompleted)
		{
			*bOutListCompleted = FALSE;
		}
		TRACE("VirtualQueryExFull pass %x\n", pass);

		//free(big_buf);
		//return FALSE;
	}
	else
	{
		//数据完整
		if (bOutListCompleted)
		{
			*bOutListCompleted = TRUE;
		}
	}
	copy_pos++;
	for (; res > 0; res--)
	{
		uint64_t vma_start = 0;
		uint64_t vma_end = 0;
		char vma_flags[4] = { 0 };
		char name[4096] = { 0 };

		vma_start = *(uint64_t*)copy_pos;
		copy_pos += 8;
		vma_end = *(uint64_t*)copy_pos;
		copy_pos += 8;
		memcpy(&vma_flags, (void*)copy_pos, 4);
		copy_pos += 4;
		memcpy(&name, (void*)copy_pos, 4096);
		name[sizeof(name) - 1] = '\0';
		copy_pos += 4096;

		DRIVER_REGION_INFO rInfo = { 0 };
		rInfo.baseaddress = vma_start;
		rInfo.size = vma_end - vma_start;
		if (vma_flags[2] == '\x01')
		{
			//executable
			if (vma_flags[1] == '\x01')
			{
				rInfo.protection = PAGE_EXECUTE_READWRITE;
			}
			else
			{
				rInfo.protection = PAGE_EXECUTE_READ;
			}
		}
		else
		{
			//not executable
			if (vma_flags[1] == '\x01')
			{
				rInfo.protection = PAGE_READWRITE;
			}
			else if (vma_flags[0] == '\x01')
			{
				rInfo.protection = PAGE_READONLY;
			}
			else
			{
				rInfo.protection = PAGE_NOACCESS;
			}
		}
		if (vma_flags[3] == '\x01')
		{
			rInfo.type = MEM_MAPPED;
		}
		else
		{
			rInfo.type = MEM_PRIVATE;
		}
		memcpy(&rInfo.name, &name, 4096);
		rInfo.name[sizeof(rInfo.name) - 1] = '\0';
		if (showPhy)
		{
			//只显示在物理内存中的内存
			DRIVER_REGION_INFO rPhyInfo = { 0 };

			char ptr_buf[16] = { 0 };
			*(uint64_t*)&ptr_buf[0] = hProcess;

			uint64_t addr;
			int isPhyRegion = 0;
			for (addr = vma_start; addr < vma_end; addr += getpagesize())
			{
				*(uint64_t*)&ptr_buf[8] = addr;
				if (ioctl(nDriverLink, IOCTL_CHECK_PROCESS_ADDR_PHY, (unsigned long)&ptr_buf, sizeof(ptr_buf)) == 1)
				{
					if (isPhyRegion == 0)
					{
						isPhyRegion = 1;
						rPhyInfo.baseaddress = addr;
						rPhyInfo.protection = rInfo.protection;
						rPhyInfo.type = rInfo.type;
						strcpy(rPhyInfo.name, rInfo.name);
					}

				}
				else
				{
					if (isPhyRegion == 1)
					{
						isPhyRegion = 0;
						rPhyInfo.size = addr - rPhyInfo.baseaddress;
						cvector_pushback(vOutput, &rPhyInfo);
					}
				}
			}

			if (isPhyRegion == 1)
			{
				//all vma region inside phy memory
				rPhyInfo.size = vma_end - rPhyInfo.baseaddress;
				cvector_pushback(vOutput, &rPhyInfo);
			}

		}
		else
		{
			//显示全部内存
			cvector_pushback(vOutput, &rInfo);
		}

	}
	free(big_buf);

	return (pass == '\x01') ? FALSE : TRUE;
}



static inline BOOL rwProcMemDriver_GetProcessPidList(int nDriverLink, BOOL bIsSpeedMode, cvector vOutput, BOOL * bOutListCompleted)
{
	if (nDriverLink < 0) { return FALSE; }
	uint64_t len = 0;
	int bufSize = 8 + 1 + len;
	char *buf = (char*)malloc(bufSize);
	memset(buf, 0, bufSize);
	*(uint64_t*)&buf[0] = len;
	buf[8] = bIsSpeedMode == TRUE ? '\x01' : '\x00'; //获取进程列表方式：0为稳定模式，1为极速模式

	int count1 = ioctl(nDriverLink, IOCTL_GET_PROCESS_PID_LIST, (unsigned long)buf, bufSize);
	free(buf);

	TRACE("GetProcessPidList count1:%d\n", count1);

	if (count1 <= 0)
	{
		TRACE("GetProcessPidList ioctl():%s\n", strerror(errno));
		*bOutListCompleted = FALSE;
		return FALSE;
	}

	len = count1 * sizeof(int);
	bufSize = 8 + 1 + len;
	buf = (char*)malloc(bufSize);
	memset(buf, 0, bufSize);
	*(uint64_t*)&buf[0] = len;
	//buf[8] = '\x00'; //获取进程列表方式：0为稳定模式，1为极速模式
	int count2 = ioctl(nDriverLink, IOCTL_GET_PROCESS_PID_LIST, (unsigned long)buf, bufSize);
	free(buf);
	TRACE("GetProcessPidList count2:%d\n", count2);
	if (count2 <= 0)
	{
		TRACE("GetProcessPidList ioctl():%s\n", strerror(errno));
		*bOutListCompleted = FALSE;
		return FALSE;
	}

	for (int i = 0; i < count1; i++)
	{
		int pid = *(int*)&buf[i * sizeof(int)];
		cvector_pushback(vOutput, &pid);
	}
	if(count2 == count1){*bOutListCompleted = TRUE;} else {*bOutListCompleted = FALSE;}
	return TRUE;
}


static inline BOOL rwProcMemDriver_GetProcessGroup(int nDriverLink, uint64_t hProcess,
	uint64_t *nOutUID,
	uint64_t *nOutSUID,
	uint64_t *nOutEUID,
	uint64_t *nOutFSUID,
	uint64_t *nOutGID,
	uint64_t *nOutSGID,
	uint64_t *nOutEGID,
	uint64_t *nOutFSGID)
{
	if (nDriverLink < 0) { return FALSE; }
	if (!hProcess) { return FALSE; }

	char buf[64] = { 0 };
	*(uint64_t*)&buf[0] = hProcess;

	int res = ioctl(nDriverLink, IOCTL_GET_PROCESS_GROUP, (unsigned long)&buf, sizeof(buf));
	if (res != 0)
	{
		TRACE("GetProcessGroup ioctl():%s\n", strerror(errno));
		return FALSE;
	}
	*nOutUID = *(uint64_t*)&buf[0];
	*nOutSUID = *(uint64_t*)&buf[8];
	*nOutEUID = *(uint64_t*)&buf[16];
	*nOutFSUID = *(uint64_t*)&buf[24];
	*nOutGID = *(uint64_t*)&buf[32];
	*nOutSGID = *(uint64_t*)&buf[40];
	*nOutEGID = *(uint64_t*)&buf[48];
	*nOutFSGID = *(uint64_t*)&buf[56];
	return TRUE;
}
static inline BOOL rwProcMemDriver_SetProcessRoot(int nDriverLink, uint64_t hProcess)
{
	if (nDriverLink < 0) { return FALSE; }
	if (!hProcess) { return FALSE; }

	char buf[8] = { 0 };
	*(uint64_t*)&buf[0] = hProcess;

	int res = ioctl(nDriverLink, IOCTL_SET_PROCESS_ROOT, (unsigned long)&buf, sizeof(buf));
	if (res != 0)
	{
		TRACE("SetProcessRoot ioctl():%s\n", strerror(errno));
		return FALSE;
	}
	return TRUE;
}

static inline BOOL rwProcMemDriver_GetProcessRSS(int nDriverLink, uint64_t hProcess, uint64_t *outRss)
{
	if (nDriverLink < 0) { return FALSE; }
	if (!hProcess) { return FALSE; }
	char buf[8] = { 0 };
	*(uint64_t*)&buf[0] = hProcess;
	int res = ioctl(nDriverLink, IOCTL_GET_PROCESS_RSS, (unsigned long)&buf, sizeof(buf));
	if (res != 0)
	{
		TRACE("GetProcessRSS ioctl():%s\n", strerror(errno));
		return FALSE;
	}
	*outRss = *(uint64_t*)&buf[0];
	return TRUE;
}
static inline BOOL rwProcMemDriver_GetProcessCmdline(int nDriverLink, uint64_t hProcess, char *lpOutCmdlineBuf, size_t bufSize)
{
	if (nDriverLink < 0) { return FALSE; }
	if (!hProcess) { return FALSE; }
	if (bufSize <= 0) { return FALSE; }
	char buf[16] = { 0 };
	*(uint64_t*)&buf[0] = hProcess;
	int res = ioctl(nDriverLink, IOCTL_GET_PROCESS_CMDLINE_ADDR, (unsigned long)&buf, sizeof(buf));
	if (res != 0)
	{
		TRACE("GetProcessCmdline ioctl():%s\n", strerror(errno));
		return FALSE;
	}
	uint64_t arg_start = *(uint64_t*)&buf[0], arg_end = *(uint64_t*)&buf[8];
	if (arg_start <= 0)
	{
		return FALSE;
	}
	uint64_t len = arg_end - arg_start;
	if (len <= 0)
	{
		return FALSE;
	}
	if (bufSize < len)
	{
		len = bufSize;
	}
	memset(lpOutCmdlineBuf, 0, bufSize);
	return rwProcMemDriver_ReadProcessMemory(nDriverLink, hProcess, arg_start, lpOutCmdlineBuf, len, NULL, false);
}
#endif /*__linux__*/






#endif /* MEMORY_READER_WRITER_H_ */
