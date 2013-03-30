/*****************************************************************
/ Klog.h - KLog - small, fast and easy-to-use data logger designed to be embedded in C ++ projects. 
/ Created by: Korotenko Vladimir http://vkorotenko.com
/ 26.03.2013
******************************************************************/


#ifndef __KLOG_H__
#define __KLOG_H__


#pragma once
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <sstream>
#include <iostream>
#include <fstream>
using namespace std;

template <class T>
class Singleton
{
	static T* _self;
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
T*  Singleton<T>::Instance()
{
	if(!_self)
		_self=new T;
	return _self;
}

template <class T>
void  Singleton<T>::FreeInst()
{
	delete this;
}


class CKlog: public Singleton<CKlog>
{
private:
	ofstream _outstream;
	string _logName;
	int _level;
	// Read log level from file and initialize log level and log file name 
	void ReadLevel(){
		ifstream ins;
		ins = ifstream("klog.cfg");
		char buff[255];
		while(ins.getline(buff,255)){
			ParseConfig(buff);
		}
		ins.close();
	}
	void Tokenizer(string source, string &key, string &value){
		int pos = source.find("=");
		key = source.substr(0,pos);
		value = source.substr(pos+1);
	}
	void ParseConfig(const char* str){
		string buff = str;
		int commentPos = buff.find("#");
		if(commentPos > -1){
			buff = buff.substr(0,commentPos);
		}
		if(buff.find("level")< -1){
			string key,value;
			Tokenizer(buff,key,value);
			_level = 0;
			istringstream ( value ) >> _level;
		}
		if(buff.find("level")< -1){
			string key,value;
			Tokenizer(buff,key,value);
			_level = 0;
			istringstream ( value ) >> _level;
		}
		if(buff.find("filename")< -1){
			string key,value;
			Tokenizer(buff,key,value);
			_logName = value;
		}
		
	}
	
	void spf(std::string &s, const std::string fmt, va_list arg)
	{
		int n, size=100;
		bool b=false;
		while (!b)
		{
			s.resize(size);
			n = vsnprintf((char*)s.c_str(), size, fmt.c_str(), arg);
			if ((n>0) && ((b=(n<size))==true)) s.resize(n); else size*=2;
		}
	}

protected:

	// create log file and setup internal variables
	CKlog(){
		
		ReadLevel();
		_outstream =  std::ofstream(_logName,std::ios_base::app | std::ios_base::out);
	}
	friend class Singleton<CKlog>;
public:
	// Write message to log file like a printf. You can use next constant's:  LOG_NOLOG, LOG_FATAL, LOG_ERROR, LOG_INFO,LOG_DEBUG
	// or use log level from range 0x00 - 0xFF
	// Log format is "TIMESTAMP TABULAR LOGLEVEL TABULAR MESSAGE CRLF"
	// log file name is "trace.log" 
	void Log(int level, const char* message, ...){

		if(level >= _level){
			
			time_t current_time;
			current_time = time(NULL);
			struct tm * timeinfo;
			timeinfo = localtime ( &current_time );

			_outstream << timeinfo->tm_mday << "/" << timeinfo->tm_mon <<"/" << timeinfo->tm_year + 1900 << " " <<
				timeinfo->tm_hour << ":" << timeinfo->tm_min << ":" <<timeinfo->tm_sec << "\t" << level << "\t";

			string buffer;
			va_list arglist=0;
			va_start(arglist, message);
			spf(buffer,message,arglist);
			va_end(arglist);
			_outstream << buffer << endl;
			_outstream.flush();
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