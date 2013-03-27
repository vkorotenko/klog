/*****************************************************************
/ Klog.h - KLog - small, fast and easy-to-use data logger designed to be embedded in C ++ projects. 
/ Created by: Korotenko Vladimir http://vkorotenko.com
/ 26.03.2013
******************************************************************/


#ifndef __KLOG_H__
#define __KLOG_H__


#pragma once
#include <Windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <atlstr.h>

template <class T>
class Singleton
{
	static T* _self;
	static int _refcount;
protected:
	Singleton(){}
	virtual ~Singleton(){_self=NULL;}
public:
	static T* Instance();
	void FreeInst();
};

template <class T>
T*  Singleton<T>::_self = NULL;

template <class T>
int  Singleton<T>::_refcount=0;

template <class T>
T*  Singleton<T>::Instance()
{
	if(!_self)
		_self=new T;
	_refcount++;
	return _self;
}

template <class T>
void  Singleton<T>::FreeInst()
{
	if(--_refcount==0)
		delete this;
}


class CKlog: public Singleton<CKlog>
{
private:
	HANDLE _file;
	int _level;
	// Read log level from registry and initialize log level 
	void ReadLevel(){

		DWORD lRv;
		const CHAR* runKey="Software\\KVN"; 
		DWORD val=0;
		DWORD sz= sizeof(DWORD);
		LPDWORD pdwType =0;
		lRv = RegGetValue(HKEY_CURRENT_USER,runKey,"level",RRF_RT_REG_DWORD,pdwType, &val,&sz);
		_level = (int) val;
	}


protected:

	// create log file and setup internal variables
	CKlog(){
		_file = CreateFile ("trace.log", FILE_APPEND_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL, 0);
		ReadLevel();
	}
	friend class Singleton<CKlog>;
public:
	// Write message to log file like a printf. You can use next constant's:  LOG_NOLOG, LOG_FATAL, LOG_ERROR, LOG_INFO,LOG_DEBUG
	// or use log level from range 0x00 - 0xFF
	// Log format is "TIMESTAMP TABULAR LOGLEVEL TABULAR MESSAGE CRLF"
	// log file name is "trace.log" 
	void Log(int level, const char* message, ...){

		if(level >= _level){
			va_list arglist;
			va_start(arglist, message);
			SYSTEMTIME time;
			GetSystemTime(&time);

			CAtlString strBuff;
			strBuff.Format("%02d/%02d/%4d %02d:%02d:%02d.%04d\t%d\t",time.wDay,
				time.wMonth,
				time.wYear, 
				time.wHour,
				time.wMinute,
				time.wSecond,
				time.wMilliseconds,
				level);
			CAtlString strMsg;
			strMsg.FormatV(message,arglist);
			strBuff.Append(strMsg);
			strBuff.Append("\r\n");
			DWORD numb_bytes;
			WriteFile(_file, strBuff, strBuff.GetLength(), &numb_bytes, NULL);
			va_end(arglist);
		}
	};
};

#define LOG(level,pszFmt, ...)   CKlog::Instance()->Log(level,pszFmt, __VA_ARGS__);

#define LOG_NOLOG	0x0
#define LOG_FATAL	0x50
#define LOG_ERROR	0x80
#define LOG_INFO	0xAA
#define LOG_DEBUG	0xFF

#endif // __KLOG_H__