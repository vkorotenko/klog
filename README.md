Klog
====

KLog - small, fast and easy-to-use data logger designed to be embedded in C ++ projects. 
For implementation see main.cpp


Usage
=====

* add to you project file CKlog.h
* add LOG(LEVEL, FORMATSTRING, ARGS)
 * FORMATSTRING - it's a string in printf style
 * ARGS - any argument

Sample usage
============

```cpp
#include "Klog.h"
int main(int argc, char** argv){
  
	LOG(255,"test %s", "hello" );
	LOG(255,"test");

}

```
Config file sintaxis
====================

```cfg
# comment string
level = 255 # log level
filename = klog_11.log # path to log file
```







