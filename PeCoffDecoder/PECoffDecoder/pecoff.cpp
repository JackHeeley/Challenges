#include "stdafx.h"
//
// pecoff.cpp : pecoff image file decoder 
//
//				this is a windows platform implementation (of a pure C++11 header)
//				builds and runs on x86 & x64 architectures
//
// Copyright (c) 2015 Jack Heeley
// 
#include <windows.h>
#include <winnt.h>

#include "pecoff.h"

using namespace my_portable;


/*
* ***************************************************************************
* PIMPL idiom - encapsulation of pecoff_impl class 
* ***************************************************************************
*/

///<summary>encapsulated pecoff implementation class (PIMPL idiom)
///
/// At construction time the contents of a named file are read into an image container (vector).
/// Initializers check that file content is valid PE COFF file format and initialize
/// references to expected header structures as found within the image container.
/// These header structures give direct access to certain PE COFF file attributes and also
/// support interrogating the wider content of the image. During class initialization certain key
/// attributes and characteristics of the image (as determined by the challenge) are fetched or inferred.
/// Image interrogation is in places quite indirect and complex (see Project Documentation Files for details).
///
/// Key concept: Image headers contain various rva's (relative virtual addresses) of DWORD size. These are
/// linear offsets from the base asddress where the image is loaded. Because we read the file into a container
/// rva's can be transformed into pointers by adding the virtual memory address of the beginning of the container 
/// data. In sections rva's have an extra level of indirection.
///
/// Key concept: code builds in Win32 and x64 solution platforms and interprets both 32 and 64 bit images. 
/// Winnt.h provides solution platform independent typedefs for header structures. 32 and 64 appearing in the 
/// names of these structures refer to the image being decoded and have nothing to do with solution platform architecture.
///
/// Class acquires and interprets data at construction time. Thereafter it is completely stateless and invariant.
/// Class currently only exposes getters to stateless invariant attributes. 
/// Image memory is released (by resizing vector to zero) when initializing is completed. 
///
/// RAAI constrains Ctor & Dtor: They must not throw exceptions.
/// Image container (vector) must remain invariant after image is read until no longer needed (end of constructor).
/// This is because we make references to addresses within the linear address space of the vector data.
/// Header structures used here are provided by winnt.h (couples this code to windows build environment).
///
/// uses PIMPL (pointer to implementation) and RAII (resource acquisition is initialization) concepts
///
///</summary>
class pecoff::pecoff_impl
{

public:

	///<summary>default ctor</summary>
	pecoff_impl() {	initialized = false; }	
	
	///<summary>ctor with fileName</summary>
	///<param name='aFileName'>filename of a candidate pecoff file</param>
	pecoff_impl(std::string aFileName) 	{ initialized = initialize(aFileName); }
	
	///<summary>default dtor</summary>
	~pecoff_impl() = default;

	///<summary>get initialized property</summary>
	bool inline getInitialized()	{ return initialized; }

	///<summary>get 64bit peoperty</summary>
	bool inline get64bit() 	{ if (!initialized)	throw std::exception("pecoff not initialized");	return _64bit; }

	///<asummary>get ExecutableType property</summary>
	ExecutableType inline getExecutableType() {	if (!initialized) throw std::exception("pecoff not initialized"); return executableType; }

private:

	//private invariant members backing the public pecoff properties 
	bool initialized;
	ExecutableType executableType;
	bool _64bit;

	// container for the file content 
	std::vector<BYTE> image;

	// all the architectural magic we need to work with the image is declared here (style here puts trust in optimizer) ...

	inline DWORD getRvaFromPtr(UINT_PTR ptr) { return (DWORD)(ptr - (UINT_PTR)&image[0]); }
	inline UINT_PTR getPtrFromRva(DWORD rva) { return (UINT_PTR)&image[rva]; }

	inline IMAGE_DOS_HEADER& getDosHeader() { return *(PIMAGE_DOS_HEADER)(&image[0]); }
	inline IMAGE_NT_HEADERS& getNtHeaders() { return *(PIMAGE_NT_HEADERS)(&image[getDosHeader().e_lfanew]); }
	inline IMAGE_OPTIONAL_HEADER& getOptionalHeader() { return *(PIMAGE_OPTIONAL_HEADER)(&getNtHeaders().OptionalHeader); }
	inline IMAGE_OPTIONAL_HEADER32& getOptionalHeader32() { return *(PIMAGE_OPTIONAL_HEADER32)(&getNtHeaders().OptionalHeader); }
	inline IMAGE_OPTIONAL_HEADER64& getOptionalHeader64() { return *(PIMAGE_OPTIONAL_HEADER64)(&getNtHeaders().OptionalHeader); }

	inline IMAGE_SECTION_HEADER& getFirstSection() { return *(PIMAGE_SECTION_HEADER)(&image[getDosHeader().e_lfanew + FIELD_OFFSET(IMAGE_NT_HEADERS, OptionalHeader) + getNtHeaders().FileHeader.SizeOfOptionalHeader]); }
	inline IMAGE_IMPORT_DESCRIPTOR& getImportDescriptor(DWORD rva)	{ return *(PIMAGE_IMPORT_DESCRIPTOR)(&image[getSectionRelativeRva(rva)]); }
	inline IMAGE_SECTION_HEADER& getSectionFromRva(DWORD rva) { return *(PIMAGE_SECTION_HEADER)(&image[rva]); }
	inline DWORD getSectionRelativeRva(DWORD rva) { return rva - (findEnclosingSection(rva).VirtualAddress - findEnclosingSection(rva).PointerToRawData); }
	inline IMAGE_SECTION_HEADER& findEnclosingSection(DWORD rva)
	{
		DWORD sectionRva = getRvaFromPtr((UINT_PTR)&getFirstSection());
		for (unsigned i = 0; i < getNtHeaders().FileHeader.NumberOfSections; i++)
		{
			IMAGE_SECTION_HEADER section = getSectionFromRva(sectionRva);
			if ((rva >= getSectionFromRva(sectionRva).VirtualAddress) && (rva < (getSectionFromRva(sectionRva).VirtualAddress + getSectionFromRva(sectionRva).Misc.VirtualSize)))
				return getSectionFromRva(sectionRva);

			sectionRva += sizeof(IMAGE_SECTION_HEADER);
		}
		throw std::exception("enclosing section not found");
	}

	// indicators of what was found present in the image...
	enum class image_quality : int
	{
		no_content = 0,
		no_dos_header,
		bad_dos_signature,
		no_nt_headers,
		bad_nt_signature,
		no_optional_headers,
		bad_optional_headers_signature,
		good_optional_header32,
		good_optional_header64
	};

	// actual imagQuality found by verifyImage()
	image_quality imageQuality;

	///<summary>RAII helper for constructor</summary>
	///<param name='aFileName'>the name of a candidate pecoff image file</param>
	///<returns>true if some image data was presented, otherwise false</returns> 
	bool initialize(std::string aFileName)
	{
		executableType = ExecutableType::FileExeTypeNone;
		_64bit = false;

		imageQuality = image_quality::no_content;

		try
		{
			std::ifstream file(aFileName, std::ios::in | std::ios::binary);
			file.unsetf(std::ios::skipws); // Stop eating new lines in binary mode!!!
			image.insert(image.begin(), std::istream_iterator<BYTE>(file), std::istream_iterator<BYTE>());

			imageQuality = verifyImage();

			if (imageQuality == image_quality::no_content)
				return false;

			decodeImage();

			image.resize(0); // image no longer needed, purge it.
			return true;
		}
		catch (...)
		{
			image.resize(0);
			return false;
		}
	}

	///<summary>indicator of trustworthy NtHeaders content</summary>
	bool checkNtHeadersMagic()
	{
		if (LOWORD(getNtHeaders().Signature) == IMAGE_OS2_SIGNATURE)
			return true;
		
		if (LOWORD(getNtHeaders().Signature) == IMAGE_OS2_SIGNATURE_LE)
			return true;

		if (getNtHeaders().Signature == IMAGE_NT_SIGNATURE)
			return true;

		return false;
	}

	///<summary>assess the condition of image data acquired</summary>
	image_quality verifyImage()
	{
		if (image.size() < sizeof(IMAGE_DOS_HEADER))
			return image_quality::no_content;

		const int cbyDataAvailable = static_cast<int>(image.size());

		int cbyDataRequired = sizeof(IMAGE_DOS_HEADER);
		if (cbyDataRequired > cbyDataAvailable)
			return image_quality::no_dos_header;
		
		if (getDosHeader().e_magic != IMAGE_DOS_SIGNATURE)
			return image_quality::bad_dos_signature;

		cbyDataRequired = getDosHeader().e_lfanew + sizeof(IMAGE_NT_HEADERS);
		if (cbyDataRequired > cbyDataAvailable)
			return image_quality::no_nt_headers;

		if (!checkNtHeadersMagic())
			return image_quality::bad_nt_signature;

		cbyDataRequired += getNtHeaders().FileHeader.SizeOfOptionalHeader;
		if (cbyDataRequired > cbyDataAvailable)
			return image_quality::no_optional_headers;

		if ((getNtHeaders().OptionalHeader.Magic) == IMAGE_NT_OPTIONAL_HDR32_MAGIC)
			return image_quality::good_optional_header32;

		if ((getNtHeaders().OptionalHeader.Magic) == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
			return image_quality::good_optional_header64;

		return image_quality::bad_optional_headers_signature;
	}

	///<summary>64-bit image is indicated directly (if optionalHeader is present)</summary>
	inline bool decode64bitIndication() { return (((getOptionalHeader().Magic) == IMAGE_NT_OPTIONAL_HDR64_MAGIC) ? true : false); }

	///<summary>guarded interpretation of image data. 
	/// imageQuality is used to guard against dereferencing data that is not available or trustworthy in the image.</summary>
	void decodeImage()
	{
		switch (imageQuality)
		{
		case image_quality::no_content:
		case image_quality::no_dos_header:
		case image_quality::bad_dos_signature:
			break;												//corrupt, unrecognized and unsupported here

		case image_quality::no_nt_headers:
		case image_quality::bad_nt_signature:
			executableType = ExecutableType::FileExeTypeDosExe;
			break;

		case image_quality::no_optional_headers:
			executableType = ExecutableType::FileExeTypeNone;	//corrupt, unrecognized and unsupported here
			break;

		case image_quality::good_optional_header32:
		case image_quality::good_optional_header64:
			executableType = decodeExecutableType();
			_64bit = decode64bitIndication();
			break;

		case image_quality::bad_optional_headers_signature:
			break;

		default:
			break;
		}
	}

	///<summary>subsystem is indicated directly (if NtHeaders is present)</summary>
	inline WORD decodeSubsystem() { return getNtHeaders().OptionalHeader.Subsystem; }

	///<summary>infer if the image is a kernel mode driver from image content</summary>
	bool decodeKernelMode()
	{
		const std::string nameToMatch = "ntoskrnl.exe";

		DWORD importsRva;

		switch (getNtHeaders().OptionalHeader.Magic)
		{
		case IMAGE_NT_OPTIONAL_HDR32_MAGIC:
			importsRva = getOptionalHeader32().DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
			break;

		case IMAGE_NT_OPTIONAL_HDR64_MAGIC:
			importsRva = getOptionalHeader64().DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
			break;

		default:
			throw std::exception("bad magic");
		}

		if (!importsRva)
			return false;

		while (true)
		{
			IMAGE_IMPORT_DESCRIPTOR& importDesc = getImportDescriptor(importsRva);

			if ((importDesc.TimeDateStamp == 0) && (importDesc.Name == 0))
				break;

			std::string nameOfEntry = (char*)getPtrFromRva(getSectionRelativeRva(importDesc.Name));
			std::transform(nameOfEntry.begin(), nameOfEntry.end(), nameOfEntry.begin(), ::tolower);
			if (nameToMatch.compare(nameOfEntry) == 0)
				return true;

			importsRva += sizeof(IMAGE_IMPORT_DESCRIPTOR);
		}

		return false;
	}

	///<summary>infer executableType property from image content</summary>
	ExecutableType decodeExecutableType()
	{
		if (getNtHeaders().FileHeader.Characteristics & IMAGE_FILE_DLL)
			return ExecutableType::FileExeTypeDll; // when dll is flagged type is always dll 

		if (decodeSubsystem() == IMAGE_SUBSYSTEM_NATIVE)
		{
			if (decodeKernelMode())
				return ExecutableType::FileExeTypeSys; // this is how drivers are indicated, type is sys

			return ExecutableType::FileExeTypeExe;
		}

		if (getNtHeaders().FileHeader.Characteristics & IMAGE_FILE_EXECUTABLE_IMAGE) // if none of above and still executable then type is exe
			return ExecutableType::FileExeTypeExe;

		return ExecutableType::FileExeTypeNone; //corrupt, unrecognized and unsupported here
	}

};

/*
* ***************************************************************************
* PIMPL idiom - implementation of pecoff class
*
* PIMPL means 'pointer to implementation' and decouples implementation from
* client code. Here we use a std::unique_ptr to bridge to the implementation
*
* ***************************************************************************
*/

pecoff::pecoff(std::string aFileName) : pimpl{ std::make_unique<pecoff_impl>(aFileName) } { }
pecoff::pecoff() : pimpl{ std::make_unique<pecoff_impl>() } { }
bool pecoff::getInitialized() { return pimpl->getInitialized(); }
pecoff::~pecoff() = default;
bool pecoff::get64bit() { return pimpl->get64bit(); }
ExecutableType pecoff::getExecutableType() { return pimpl->getExecutableType(); }

