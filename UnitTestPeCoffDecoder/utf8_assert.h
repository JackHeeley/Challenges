#ifndef _UTF8_ASSERT_H_
#define _UTF8_ASSERT_H_
//
// utf8_assert.h : assert in utf8 everywhere style
//				this is a windows platform implementation
//				builds and runs on x86 & x64 architectures
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
#include <Windows.h>
#include <winnt.h>

#include <string>
#include <vector>
#include <memory>
#include <string>
#include <locale>
#include <codecvt>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace utf8
{

   ///<summary>convert to and from utf8</summary>
   class convert
   {
   public:
   
      ///<summary>convert utf16 string to utf8 string</summary>
      ///<param name='wstr'>utf16 encoded string</param>
      ///<returns>utf8 encoded string representation of wstr</returns>
      static std::string from_utf16(const std::wstring& wstr)
      {
         std::wstring_convert<std::codecvt_utf8_utf16 <wchar_t>, wchar_t> cv;    // TODO : deprecated (security) c++17
         return cv.to_bytes(wstr);
      }
   
      ///<summary>convert utf8 string to utf16 string</summary>
      ///<param name='str'>utf8 encoded string</param>
      ///<returns>utf16 encoded string representation of str</returns>
      static std::wstring to_utf16(const std::string& str)
      {
         std::wstring_convert<std::codecvt_utf8_utf16 <wchar_t>, wchar_t> cv;    // TODO : deprecated (security) c++17
         return cv.from_bytes(str);
      }
   };

   // TODO: check for opportunities to simplify this using perfect fowarding 
   class Assert : public Microsoft::VisualStudio::CppUnitTestFramework::Assert
   {
   public:

      // Verify that two objects are equal.
      template<typename T> static void AreEqual(const T& expected, const T& actual, const char* message = NULL, const __LineInfo* pLineInfo = NULL)
      {
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(expected, actual, message ? utf8::convert::to_utf16(message).c_str() : NULL, pLineInfo);
      }

      // double equality comparison:
      static void AreEqual(double expected, double actual, double tolerance, const char* message = NULL, const __LineInfo* pLineInfo = NULL)
      {
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(expected, actual, tolerance, message ? utf8::convert::to_utf16(message).c_str() : NULL, pLineInfo);
      }

      // float equality comparison:
      static void AreEqual(float expected, float actual, float tolerance, const char* message = NULL, const __LineInfo* pLineInfo = NULL)
      {
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(expected, actual, tolerance, message ? utf8::convert::to_utf16(message).c_str() : NULL, pLineInfo);
      }

      // char* string equality comparison:
      static void AreEqual(const char* expected, const char* actual, const char* message, const __LineInfo* pLineInfo = NULL)
      {
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(expected, actual, message ? utf8::convert::to_utf16(message).c_str() : NULL, pLineInfo);
      }

      // char* string equality comparison:
      static void AreEqual(const char* expected, const char* actual, bool ignoreCase = false, const char* message = NULL, const __LineInfo* pLineInfo = NULL)
      {
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(expected, actual, ignoreCase, message ? utf8::convert::to_utf16(message).c_str() : NULL, pLineInfo);
      }

      // wchar_t* string equality comparison:
      static void AreEqual(const wchar_t* expected, const wchar_t* actual, const char* message, const __LineInfo* pLineInfo = NULL)
      {
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(expected, actual, message ? utf8::convert::to_utf16(message).c_str() : NULL, pLineInfo);
      }

      // wchar_t* string equality comparison:
      static void AreEqual(const wchar_t* expected, const wchar_t* actual, bool ignoreCase = false, const char* message = NULL, const __LineInfo* pLineInfo = NULL)
      {
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(expected, actual, ignoreCase, message ? utf8::convert::to_utf16(message).c_str() : NULL, pLineInfo);
      }

      // Verify that two objects are not equal.
      template<typename T> static void AreNotEqual(const T& notExpected, const T& actual, const char* message = NULL, const __LineInfo* pLineInfo = NULL)
      {
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreNotEqual(notExpected, actual, message ? utf8::convert::to_utf16(message).c_str() : NULL, pLineInfo);
      }

      // double AreNotEqual comparison:
      static void AreNotEqual(double notExpected, double actual, double tolerance, const char* message = NULL, const __LineInfo* pLineInfo = NULL)
      {
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreNotEqual(notExpected, actual, tolerance, message ? utf8::convert::to_utf16(message).c_str() : NULL, pLineInfo);
      }

      // float AreNotEqual comparison:
      static void AreNotEqual(float notExpected, float actual, float tolerance, const char* message = NULL, const __LineInfo* pLineInfo = NULL)
      {
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreNotEqual(notExpected, actual, tolerance, message ? utf8::convert::to_utf16(message).c_str() : NULL, pLineInfo);
      }

      // char* string AreNotEqual comparison with char* message:
      static void AreNotEqual(const char* notExpected, const char* actual, const char* message, const __LineInfo* pLineInfo = NULL)
      {
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreNotEqual(notExpected, actual, message ? utf8::convert::to_utf16(message).c_str() : NULL, pLineInfo);
      }

      // use base class implementation...
      //// char* string AreNotEqual comparison with wchar_t*  message:
      //static void AreNotEqual(const char* notExpected, const char* actual, const wchar_t* message, const __LineInfo* pLineInfo = NULL)
      //{
      //	Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreNotEqual(notExpected, actual, message, pLineInfo); 
      //}

      // char* string AreNotEqual comparison:
      static void AreNotEqual(const char* notExpected, const char* actual, bool ignoreCase = false, const char* message = NULL, const __LineInfo* pLineInfo = NULL)
      {
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreNotEqual(notExpected, actual, ignoreCase, message ? utf8::convert::to_utf16(message).c_str() : NULL, pLineInfo);
      }

      // use base class implementation...
      //// wchar_t* string AreNotEqual comparison with char* message:
      //static void AreNotEqual(const wchar_t* notExpected, const wchar_t* actual, const char* message, const __LineInfo* pLineInfo = NULL)
      //{
      //	Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreNotEqual(notExpected, actual, message, pLineInfo);
      //}

      // use base class implementation...
      // wchar_t* string AreNotEqual comparison with wchar_t* message:
      //static void AreNotEqual(const wchar_t* notExpected, const wchar_t* actual, const wchar_t* message, const __LineInfo* pLineInfo = NULL)
      //{
      //	Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreNotEqual(notExpected, actual, message, pLineInfo)
      //}

      // wchar_t* string AreNotEqual comparison:
      static void AreNotEqual(const wchar_t* notExpected, const wchar_t* actual, bool ignoreCase = false, const char* message = NULL, const __LineInfo* pLineInfo = NULL)
      {
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreNotEqual(notExpected, actual, ignoreCase, message ? utf8::convert::to_utf16(message).c_str() : NULL, pLineInfo);
      }

      // Verify that a condition is true:
      static void IsTrue(bool condition, const char* message = NULL, const __LineInfo* pLineInfo = NULL)
      {
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsTrue(condition, message ? utf8::convert::to_utf16(message).c_str() : NULL, pLineInfo);
      }

      // Verify that a conditon is false:
      static void IsFalse(bool condition, const char* message = NULL, const __LineInfo* pLineInfo = NULL)
      {
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsFalse(condition, message ? utf8::convert::to_utf16(message).c_str() : NULL, pLineInfo);
      }

      // Force the test case result to be Failed:
      static void Fail(const char* message = NULL, const __LineInfo* pLineInfo = NULL)
      {
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::Fail(message ? utf8::convert::to_utf16(message).c_str() : NULL, pLineInfo);
      }
   };
}
#endif
