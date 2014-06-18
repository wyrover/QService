#ifndef Q_TESTINCLUDE_H_
#define Q_TESTINCLUDE_H_

#include <fstream>
#include <cppunit/XmlOutputter.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/config/SourcePrefix.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include "../QBase/QBase.h"

#ifdef Q_OS_WIN32
#include "../vld/vld.h"
#endif

bool Q_CheckBuff(const char *pSrc, const char *pDes, const size_t ilen);

#endif//Q_TESTINCLUDE_H_
