//
// pecoff_unit_test.cpp : pecoff image file decoder unit tests  
//         this is a pure C++11 source file
//
// FULL DOCUMENTATION AT https://github.com/JackHeeley/Challenges/wiki
//
// Copyright (c) 2015-2020 Jack Heeley, all rights reserved. https://github.com/JackHeeley/Challenges
//
//    This program is free software : you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.If not, see < http://www.gnu.org/licenses/ >.
// 
#include "stdafx.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace my_portable;
using namespace utf8;

namespace UnitTest
{
   TEST_CLASS(pecoff_unit_test)
   {

   public:

#if _WIN64
      const std::string testDataPath = "..\\..\\UnitTestPeCoffDecoder\\TestData\\";
#else
      const std::string testDataPath = "..\\UnitTestPeCoffDecoder\\TestData\\";
#endif

      TEST_METHOD(TestPecoffConstructor)
      {
         const std::string fileName = testDataPath + "KeePassLibC64.dll";
         try
         {
            pecoff myPecoff(fileName);
            try
            {
               bool initialized = myPecoff.getInitialized();
               utf8::Assert::IsTrue(initialized, "couldn't read or decode file");
            }
            catch (...)
            {
               utf8::Assert::Fail("getInitialized threw something (invalid)");
            }
         }
         catch (...)
         {
            utf8::Assert::Fail("constructor threw something (invalid)");
         }

         try
         {
            pecoff myPecoff("garbage");
            bool initialized = myPecoff.getInitialized();
            utf8::Assert::IsFalse(initialized, "indicated initialized==true with a non-existant file");
         
            // test that when the initialized==false indication is ignored a recognizable exception is raised...
            try
            {
               bool _64bit = myPecoff.get64bit();
               utf8::Assert::Fail("indicated a _64bit value with a non-existant file");
            }
            catch (std::exception& e)
            {
               utf8::Assert::AreEqual(e.what(), "pecoff not initialized", "an unexpected failure reason was reported");
            }
            try
            {
               ExecutableType executableType = myPecoff.getExecutableType();
               utf8::Assert::Fail("indicated an ExecutableType with a non-existant file");
            }
            catch (std::exception& e)
            {
               utf8::Assert::AreEqual(e.what(), "pecoff not initialized", "an unexpected failure reason was reported");
            }
         }
         catch (...)
         {
            utf8::Assert::Fail("pecoff constructor test failed");
         }
      };

      TEST_METHOD(Test64BitDll)
      {
         const std::string fileName = testDataPath + "KeePassLibC64.dll";
         try
         {
            pecoff myPecoff(fileName);
            bool initialized = myPecoff.getInitialized();
            utf8::Assert::IsTrue(initialized, "couldn't read or decode file");
            bool _64bit = myPecoff.get64bit();
            utf8::Assert::IsTrue(_64bit, "64-bit attribute of 64-bit dll was not properly identified");
            ExecutableType executableType = myPecoff.getExecutableType();
            utf8::Assert::AreEqual((int)ExecutableType::FileExeTypeDll, (int)executableType, "DLL type of 64-bit dll was not properly identified");
         }
         catch (std::exception&)
         {
            utf8::Assert::Fail("pecoff Test64BitDll threw an exception");
         }
      };

      TEST_METHOD(Test32BitDll)
      {
         const std::string fileName = testDataPath + "KeePassLibC32.dll";
         try
         {
            pecoff myPecoff(fileName);
            bool initialized = myPecoff.getInitialized();
            utf8::Assert::IsTrue(initialized, "couldn't read or decode file");
            bool _64bit = myPecoff.get64bit();
            utf8::Assert::IsFalse(_64bit, "64-bit attribute of 32-bit dll was not properly identified");
            ExecutableType executableType = myPecoff.getExecutableType();
            utf8::Assert::AreEqual((int)ExecutableType::FileExeTypeDll, (int)executableType, "DLL type of 32-bit dll was not properly identified");
         }
         catch (std::exception&)
         {
            utf8::Assert::Fail("pecoff Test32BitDll threw an exception");
         }
      };

      TEST_METHOD(Test64BitExe)
      {
         const std::string fileName = testDataPath + "NotePad.exe";
         try
         {
            pecoff myPecoff(fileName);
            bool initialized = myPecoff.getInitialized();
            utf8::Assert::IsTrue(initialized, "couldn't read or decode file");
            bool _64bit = myPecoff.get64bit();
            utf8::Assert::IsTrue(_64bit, "64-bit attribute of 64-bit executable was not properly identified");
            ExecutableType executableType = myPecoff.getExecutableType();
            utf8::Assert::AreEqual((int)ExecutableType::FileExeTypeExe, (int)executableType, "Exe type of 64-bit executable was not properly identified");
         }
         catch (std::exception&)
         {
            utf8::Assert::Fail("pecoff Test64BitExe threw an exception");
         }
      };

      TEST_METHOD(Test32BitExe)
      {
         const std::string fileName = testDataPath + "KeePass.exe";
         try
         {
            pecoff myPecoff(fileName);
            bool initialized = myPecoff.getInitialized();
            utf8::Assert::IsTrue(initialized, "couldn't read or decode file");
            bool _64bit = myPecoff.get64bit();
            utf8::Assert::IsFalse(_64bit, "64-bit attribute of 32-bit executable was not properly identified");
            ExecutableType executableType = myPecoff.getExecutableType();
            utf8::Assert::AreEqual((int)ExecutableType::FileExeTypeExe, (int)executableType, "Exe type of 32-bit executable was not properly identified");
         }
         catch (std::exception&)
         {
            utf8::Assert::Fail("pecoff Test32BitExe threw an exception");
         }
      };

      TEST_METHOD(Test64BitDriver)
      {
         const std::string fileName = testDataPath + "libusb0_amd64.sys";
         try
         {
            pecoff myPecoff(fileName);
            bool initialized = myPecoff.getInitialized();
            utf8::Assert::IsTrue(initialized, "couldn't read or decode file");
            bool _64bit = myPecoff.get64bit();
            utf8::Assert::IsTrue(_64bit, "64-bit attribute of 64-bit driver was not properly identified");
            ExecutableType executableType = myPecoff.getExecutableType();
            utf8::Assert::AreEqual((int)ExecutableType::FileExeTypeSys, (int)executableType, "Sys type of 64-bit executable was not properly identified");
         }
         catch (std::exception&)
         {
            utf8::Assert::Fail("pecoff Test64BitDriver threw an exception");
         }
      };

      TEST_METHOD(Test32BitDriver)
      {
         const std::string fileName = testDataPath + "libusb0_x86.sys";
         try
         {
            pecoff myPecoff(fileName);
            bool initialized = myPecoff.getInitialized();
            utf8::Assert::IsTrue(initialized, "couldn't read or decode file");
            bool _64bit = myPecoff.get64bit();
            utf8::Assert::IsFalse(_64bit, "64-bit attribute of 32-bit driver was not properly identified");
            ExecutableType executableType = myPecoff.getExecutableType();
            utf8::Assert::AreEqual((int)ExecutableType::FileExeTypeSys, (int)executableType, "Sys type of 32-bit driver was not properly identified");
         }
         catch (std::exception&)
         {
            utf8::Assert::Fail("pecoff Test32BitDriver threw an exception");
         }
      };

      TEST_METHOD(TestDosExe)
      {
         const std::string fileName = testDataPath + "DOS\\CHKDSK.EXE";
         try
         {
            pecoff myPecoff(fileName);
            bool initialized = myPecoff.getInitialized();
            utf8::Assert::IsTrue(initialized, "couldn't read or decode file");
            bool _64bit = myPecoff.get64bit();
            utf8::Assert::IsFalse(_64bit, "64-bit attribute of 16-bit dos executable was not properly identified");
            ExecutableType executableType = myPecoff.getExecutableType();
            utf8::Assert::AreEqual((int)ExecutableType::FileExeTypeDosExe, (int)executableType, "DosExe type of 16-bit dos executable was not properly identified");
         }
         catch (std::exception&)
         {
            utf8::Assert::Fail("pecoff Test32BitDriver threw an exception");
         }
      };

      TEST_METHOD(TestDosCom)
      {
         const std::string fileName = testDataPath + "DOS\\FORMAT.COM";
         try
         {
            pecoff myPecoff(fileName);
            bool initialized = myPecoff.getInitialized();
            utf8::Assert::IsTrue(initialized, "couldn't read or decode file");
            bool _64bit = myPecoff.get64bit();
            utf8::Assert::IsFalse(_64bit, "64-bit attribute of 16-bit dos executable was not properly identified");
            ExecutableType executableType = myPecoff.getExecutableType();
            utf8::Assert::AreEqual((int)ExecutableType::FileExeTypeNone, (int)executableType, "None type of 16-bit dos com was not properly identified");
         }
         catch (std::exception&)
         {
            utf8::Assert::Fail("pecoff Test32BitDriver threw an exception");
         }
      };

      TEST_METHOD(TestDosSys)
      {
         const std::string fileName = testDataPath + "DOS\\IO.SYS";
         try
         {
            pecoff myPecoff(fileName);
            bool initialized = myPecoff.getInitialized();
            utf8::Assert::IsTrue(initialized, "couldn't read or decode file");
            bool _64bit = myPecoff.get64bit();
            utf8::Assert::IsFalse(_64bit, "64-bit attribute of 16-bit dos executable was not properly identified");
            ExecutableType executableType = myPecoff.getExecutableType();
            utf8::Assert::AreEqual((int)ExecutableType::FileExeTypeNone, (int)executableType, "None type of 16-bit sys was not properly identified");
         }
         catch (std::exception&)
         {
            utf8::Assert::Fail("pecoff Test32BitDriver threw an exception");
         }
      };
   };
}