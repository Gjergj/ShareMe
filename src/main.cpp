//
// main.cpp
// ~~~~~~~~
//
// Copyright (c) 2003-2014 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>
#include <string>
#include <asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include "server.hpp"

#include <iostream>
#include <windows.h>
#include <winbase.h>
#include <stdio.h>
#include <vector>
#include "configurations.h"
#include "global.h"
bool iskLowerThanWin7()
{
	OSVERSIONINFO osvi;
	BOOL bIsWindowsXPorLater, bIsWindows7orEarlier;
	
	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	GetVersionEx(&osvi);

	//bIsWindowsXPorLater =
	//	((osvi.dwMajorVersion > 5) ||
	//	((osvi.dwMajorVersion == 5) && (osvi.dwMinorVersion >= 1)));

	//bIsWindows7orEarlier =
	//	((osvi.dwMajorVersion < 6) ||
	//	((osvi.dwMajorVersion < 6) && (osvi.dwMinorVersion < 1)));

	float winVersion = osvi.dwMajorVersion + (((float)osvi.dwMinorVersion) / 10);
	std::cout << winVersion << std::endl;
	if (winVersion <= 6)
	{
		return true;
	}
	return false;
}

int checkRedistInstalled()
{
	PHKEY hKey = 0;
	LONG res;
	res = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_ALL_ACCESS, hKey);
	if (res != ERROR_SUCCESS)
	{
		MessageBox(0, "Error Opening Registry Key", "Error", 0);
		return -1;
	}
	return 0;
}

//int redistInstallXp()
//{
//	HRSRC hrsrc = NULL;
//	HGLOBAL hGlbl = NULL;
//	BYTE *pExeResource = NULL;
//	HANDLE hFile = INVALID_HANDLE_VALUE;
//	DWORD size = 0;//hardcoding the size of the exe resource (in bytes)
//	HINSTANCE hModule = NULL;
//	
//	//HMODULE hmodule = GetCurrentModule(NULL);
//	//hModule = GetModuleHandleA("Resource");
//	hrsrc = FindResource(hModule, (LPCSTR)IDR_RCDATA1, RT_RCDATA);
//	//hrsrc = ::FindResource(hmodule, MAKEINTRESOURCE(IDR_RCDATA1), RT_RCDATA);
//	if (hrsrc == NULL)
//		return FALSE;
//
//	hGlbl = LoadResource(NULL, hrsrc);
//	if (hGlbl == NULL)
//		return FALSE;
//
//	pExeResource = (BYTE*)LockResource(hGlbl);
//	if (pExeResource == NULL)
//		return FALSE;
//
//	hFile = CreateFile("vcredist_x86.exe", GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
//	size = SizeofResource(hModule, hrsrc);
//	if (hFile != INVALID_HANDLE_VALUE)
//	{
//		DWORD bytesWritten = 0;
//		WriteFile(hFile, pExeResource, size, &bytesWritten, NULL);
//		CloseHandle(hFile);
//	}
//}

int CopyToClipBoard(std::string clipBoardContent)
{
	if (!OpenClipboard(NULL))
	{
		std::cout << "Cannot open the Clipboard" << std::endl;
		return -1;
	}
	// Remove the current Clipboard contents 
	if (!EmptyClipboard())
	{
		std::cout << "Cannot empty the Clipboard" << std::endl;
		return -1;
	}
	// Get the currently selected data
	HGLOBAL hGlob = GlobalAlloc(GMEM_FIXED, clipBoardContent.size());
	memcpy((char*)hGlob, clipBoardContent.c_str(), clipBoardContent.size());
	// For the appropriate data formats... 
	if (::SetClipboardData(CF_TEXT, hGlob) == NULL)
	{
		std::cout << "Unable to set Clipboard data, error: %d" << GetLastError() << std::endl;
		CloseClipboard();
		GlobalFree(hGlob);
		return -1;
	}
	CloseClipboard();
	return 0;
}

int main(int argc, char* argv[])
{



	std::cout << "--------------- Perdorimi ---------------------" << std::endl;
	std::cout << "Nese hapet brenda nje direktorie programi automatikisht \n merr te gjitha file direktorise dhe nen direktorive.\n" << std::endl;
	std::cout << "Nese mbi ikonen e programit behet drag/drop nje ose me shume file atehere\n programi merr vetem ato file\n" << std::endl;
	std::cout << "Qe pala tjeter te merre dokumentat duhet qe ne browser shkruaj IP(tuaj):porten\n Psh. 192.168.1.2:" + boost::lexical_cast<std::string>(defaultPort) << std::endl;
	std::cout << " ose http://192.168.1.2:" + boost::lexical_cast<std::string>(defaultPort) << std::endl << std::endl;


	//std::this_thread::sleep_for(std::chrono::seconds(10));
	//iskLowerThanWin7();

	//checkRedistInstalled();

	//redistInstallXp();

	for (int i = 1; i < argc; i++)
	{
		global::dragAndDropFileName.push_back(argv[i]);
	}
  try
  {
    
	http::server2::server s("0.0.0.0",  boost::lexical_cast<std::string>(defaultPort) , "", 4/*num_threads*/);
	////////////////////////////////////////

	////////////////////////////////////////
	std::cout << "ShareMe v"+std::string(version)+"\n"+creator+"\n"+email+"\n\n" << std::endl;
    // Run the server until stopped.
	
	//////////////////////////

	std::string ipS;
	using asio::ip::tcp;

	asio::io_service io_service;
	tcp::resolver resolver(io_service);
	tcp::resolver::query query(asio::ip::host_name(), "");
	tcp::resolver::iterator iter = resolver.resolve(query);
	tcp::resolver::iterator end; // End marker.
	while (iter != end)
	{
		tcp::endpoint ep = *iter++;
		if (ep.address().is_v4())
		{ 
			std::cout << ep.address().to_string() << ":"+  boost::lexical_cast<std::string>(s.port()) << std::endl;
			ipS.append(ep.address().to_string() + ":" +  boost::lexical_cast<std::string>(s.port()) + "\r\n");
		}
	}
	if (CopyToClipBoard(ipS) == 0) std::cout << "IP u ruajten ne ClipBoard, mjafton t'i beni Paste diku.\n\n";
	
	//////////////////////////


    s.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "exception: " << e.what() << "\n";
  }
  

  return 0;
}
