//
// main.cpp : pecoff image file decoder - the entry point for the PECoffDecoder console application.
//          this is a pure C++11 source file
//          builds and runs on x86 & x64 architectures
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
#include "pecoff.h"

using namespace std;
using namespace my_portable;

int main(int argc, char* argv[])
{
   std::string fileName;

   switch (argc)
   {
   case 1:
   case 2:
      std::cout << "PeCoffDecoder.exe Copyright(c) 2015-2020 Jack Heeley.\n";
      std::cout << "This program comes with ABSOLUTELY NO WARRANTY; for details refer to GPL 3.0.\n";
      std::cout << "This is free software, and you are welcome to redistribute it\n";
      std::cout << "under certain conditions; refer to GPL 3.0 for details.\n\n";

      std::cout << "You should have received a copy of the GNU General Public License (GPL 3.0)\n";
      std::cout << "along with this program. If not, see < http://www.gnu.org/licenses/" << " >.\n" << std::endl;

      fileName = std::move(argv[argc-1]); // test with own executable, or run with cmdline parameter supplied
      break;
   default:
      cout << "Usage PeCoffDecoder <fileName>" << endl;
      cout << "      where <fileName> is path to a (program type) file" << endl << endl;
      return 1;
   }

   my_portable::pecoff pecoff(fileName);
   if (!pecoff.getInitialized())
   {
      cout << "Can't read or decode " << fileName << endl;
   }
   else
   { 
      std::string fileType;
      switch (pecoff.getExecutableType())
      {
      case ExecutableType::FileExeTypeNone:
         fileType = "none";
         break;

      case ExecutableType::FileExeTypeDll:
         fileType = "dll";
         break;

      case ExecutableType::FileExeTypeExe:
         fileType = "executable";
         break;

      case ExecutableType::FileExeTypeSys:
         fileType = "system";
         break;

      case ExecutableType::FileExeTypeDosExe:
         fileType = "dos executable";
         break;

      default:
         fileType = "unrecognized"; // 'impossible' logic error

      }
      cout << "File " << fileName << (pecoff.get64bit() ? " is 64 bit" : " is not 64 bit") << " and is type " << fileType << endl;
   }

   return 0;
}
