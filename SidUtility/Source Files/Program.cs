//
// Program.cs:  the entry point for the SidUtility console application.
//              this is a c# source file
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
using System;
using System.ComponentModel;
using System.Security.Principal;

namespace Challenge1
{
    class CMainProgram
    {
 
        [STAThread]
        static void Main(string[] args)
        {
            Console.WriteLine("SidUtility.exe Copyright(c) 2015-2020 Jack Heeley.");
            Console.WriteLine("This program comes with ABSOLUTELY NO WARRANTY; for details refer to GPL 3.0.");
            Console.WriteLine("This is free software, and you are welcome to redistribute it");
            Console.WriteLine("under certain conditions; refer to GPL 3.0 for details.");
            Console.WriteLine("");
            Console.WriteLine("You should have received a copy of the GNU General Public License (GPL 3.0)");
            Console.WriteLine("along with this program. If not, see < http://www.gnu.org/licenses/ >.");
            Console.WriteLine("");

            try
            {
                Console.WriteLine(SidUtility.GetFormattedSid(WindowsIdentity.GetCurrent().User));
            }
            catch (Win32Exception w)
            {
                Console.WriteLine(@"Win32Exception ErrorCode: {0} Message: {1}", w.NativeErrorCode, w.Message);
            }
            catch(Exception e)
            {
                Console.WriteLine(@"Last ditch exception: {0}", e.Message);
            }
        }
    }
}
