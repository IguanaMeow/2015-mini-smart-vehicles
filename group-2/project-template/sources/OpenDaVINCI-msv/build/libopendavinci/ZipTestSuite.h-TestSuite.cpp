/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#define _CXXTEST_HAVE_STD
#define _CXXTEST_HAVE_EH
#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>
#include <cxxtest/XmlFilePrinter.h>

int main() {
 return CxxTest::XmlFilePrinter("/home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/build/libopendavinci/ZipTestSuite.h-TestSuite.cpp-TestSuiteReport.xml").run();
}
#include "/home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/libopendavinci/testsuites/ZipTestSuite.h"

static ZipTest suite_ZipTest;

static CxxTest::List Tests_ZipTest = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_ZipTest( "/home/msv/Desktop/project-template/sources/OpenDaVINCI-msv/libopendavinci/testsuites/ZipTestSuite.h", 36, "ZipTest", suite_ZipTest, Tests_ZipTest );

static class TestDescription_ZipTest_testDecompression : public CxxTest::RealTestDescription {
public:
 TestDescription_ZipTest_testDecompression() : CxxTest::RealTestDescription( Tests_ZipTest, suiteDescription_ZipTest, 38, "testDecompression" ) {}
 void runTest() { suite_ZipTest.testDecompression(); }
} testDescription_ZipTest_testDecompression;

#include <cxxtest/Root.cpp>
