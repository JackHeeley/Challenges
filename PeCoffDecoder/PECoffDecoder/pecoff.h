#ifndef _MY_PORTABLE_PECOFF_H_
#define _MY_PORTABLE_PECOFF_H_
//
// pecoff.h : pecoff image file decoder 
//         this is a pure C++11 header
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
#include <exception>
#include <string>
#include <memory>

namespace my_portable
{
   /// <summary>strong type enumerating the recognized executable types</summary>
   enum class ExecutableType : int
   {
      FileExeTypeNone = 0,
      FileExeTypeDll,
      FileExeTypeExe,
      FileExeTypeSys,
      FileExeTypeDosExe
   };

#pragma warning(disable : 4251 4290)
   ///<summary>pecoff decoder</summary>
   class pecoff
   {
   public:
      ///<summary>construct with fileName</summary>
      ///<param name='aFileName'>full path to a file</param>
      pecoff(std::string aFileName) throw(/*noexcept*/);
      ~pecoff();

      ///<summary> checks if pecoff instance initialized properly.
      /// this condition is met if some file data was presented, even if that 
      /// content is not a valid executable type.</summary>
      ///<returns> true if some file data was presented. false indicates a problem
      /// reading file data (e.g. invalid file path or insufficient access rights)
      /// when false is returned is returned then subsequent calls to get64bit() or 
      /// getExecutableType() with throw exceptions. </returns>
      bool getInitialized() throw(/*noexcept*/);

      ///<summary>indicate 64-bit file attribute</summary>
      ///<returns>true if file contains 64-bit code</returns>
      ///<exception cref='std::exception'>if the class instance failed to initialize</exception>
      bool get64bit() throw(std::exception);

      ///<summary>indicate executable type file attribute</summary>
      ///<returns>the executable type of the file</returns>
      ///<exception cref='std::exception'>if the class instance failed to initialize</exception>
      ExecutableType getExecutableType() throw(std::exception);

   private:
      pecoff() throw(/*noexcept*/);
      class pecoff_impl;
      std::unique_ptr<pecoff_impl> pimpl;
   };
#pragma warning(default : 4251 4290)

}

#endif //_MY_PORTABLE_PECOFF_H_

