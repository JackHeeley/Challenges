#include "stdafx.h"
//
// main.cpp : pecoff image file decoder - the entry point for the PECoffDecoder console application.
//				this is a pure C++11 source file
//				builds and runs on x86 & x64 architectures
//
// Copyright (c) 2015 Jack Heeley
// 
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
		fileName = std::move(argv[argc-1]); // test with own executable, or run with cmdline parameter supplied
		break;
	default:
		cout << "Usage ConsoleApplication1 <fileName>" << endl;
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
