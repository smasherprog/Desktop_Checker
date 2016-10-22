#include "stdafx.h"
#include "Desktop_Monitor.h"
#include <thread>
#include "Handle_Wrapper.h"
#include "Wtsapi32.h"
#include <algorithm> 
#include <locale>
#include <iostream>

template<typename charT>
struct my_equal {
	my_equal(const std::locale& loc) : loc_(loc) {}
	bool operator()(charT ch1, charT ch2) {
		return std::toupper(ch1, loc_) == std::toupper(ch2, loc_);
	}
private:
	const std::locale& loc_;
};

template<typename T>
int find_substr(const T& str1, const T& str2, const std::locale& loc = std::locale())
{
	typename T::const_iterator it = std::search(str1.begin(), str1.end(),
		str2.begin(), str2.end(), my_equal<typename T::value_type>(loc));
	if (it != str1.end()) return it - str1.begin();
	else return -1; // not found
}

RemoteDesktop::DesktopMonitor::DesktopMonitor() {

}


RemoteDesktop::DesktopMonitor::~DesktopMonitor() {
	if (m_hDesk != NULL)CloseDesktop(m_hDesk);
}

HDESK Switch_to_Desktop(int desired_desktop, HDESK currentdesk) {
	HDESK desktop = nullptr;
	auto desiredaccess = DESKTOP_CREATEMENU | DESKTOP_CREATEWINDOW | DESKTOP_ENUMERATE | DESKTOP_HOOKCONTROL | DESKTOP_WRITEOBJECTS | DESKTOP_READOBJECTS | DESKTOP_SWITCHDESKTOP | GENERIC_WRITE;
	if (desired_desktop & RemoteDesktop::DesktopMonitor::Desktops::INPUT) {
		desktop = OpenInputDesktop(0, FALSE, desiredaccess);
	}
	else {
		std::string name = "default";
		if (desired_desktop & RemoteDesktop::DesktopMonitor::Desktops::WINLOGON) name = "winlogon";
		else if (desired_desktop & RemoteDesktop::DesktopMonitor::Desktops::SCREENSAVER) {
			name = "screen-saver";
		}
		desktop = OpenDesktopA(name.c_str(), 0, FALSE, desiredaccess);
	}

	if (desktop == NULL) return nullptr;
	if (!SetThreadDesktop(desktop))
	{
		CloseDesktop(desktop);
		return nullptr;
	}
	if (currentdesk != nullptr) {
		CloseDesktop(currentdesk);
	}
	return desktop;
}
bool RemoteDesktop::DesktopMonitor::Switch_to_Desktop(int desired_desktop) {
	auto h = ::Switch_to_Desktop(desired_desktop, m_hDesk);
	if (h != nullptr) m_hDesk = h;
	return h != nullptr;
}

int RemoteDesktop::DesktopMonitor::get_InputDesktop() {
	auto inputdesktop = RAIIHDESKTOP(OpenInputDesktop(0, FALSE, GENERIC_READ));
	if (inputdesktop.get() == NULL) {
		std::cout << "Error OpenInputDesktop GetLastError=" << GetLastError()<< std::endl;
		return RemoteDesktop::DesktopMonitor::Desktops::DEFAULT;
	}
	
	DWORD dummy;
	char inputname[256];
	if (!GetUserObjectInformationA(inputdesktop.get(), UOI_NAME, &inputname, 256, &dummy)) {
		std::cout << "Error GetUserObjectInformationA GetLastError=" << GetLastError() << std::endl;
		return RemoteDesktop::DesktopMonitor::Desktops::DEFAULT;
	}
	std::string name(inputname);

	if (find_substr(name, std::string("default")) != -1) return (RemoteDesktop::DesktopMonitor::Desktops::DEFAULT | RemoteDesktop::DesktopMonitor::Desktops::INPUT);
	else if (find_substr(name, std::string("winlogon")) != -1) return (RemoteDesktop::DesktopMonitor::Desktops::WINLOGON | RemoteDesktop::DesktopMonitor::Desktops::INPUT);
	return (RemoteDesktop::DesktopMonitor::Desktops::SCREENSAVER | RemoteDesktop::DesktopMonitor::Desktops::INPUT);
}
bool RemoteDesktop::DesktopMonitor::Is_InputDesktopSelected() {
	auto inp = get_InputDesktop();
	if ((inp&RemoteDesktop::DesktopMonitor::Desktops::INPUT) == 0) {
		return false;
	}
	return (inp & get_ThreadDesktop()) != 0;
}
int RemoteDesktop::DesktopMonitor::get_ThreadDesktop() {
	HDESK threaddesktop = GetThreadDesktop(GetCurrentThreadId());
	if (threaddesktop == NULL) return RemoteDesktop::DesktopMonitor::Desktops::DEFAULT;

	DWORD dummy;
	char inputname[256];
	if (!GetUserObjectInformationA(threaddesktop, UOI_NAME, &inputname, 256, &dummy)) return RemoteDesktop::DesktopMonitor::Desktops::DEFAULT;
	std::string name(inputname);
	if (find_substr(name, std::string("default")) != -1) return RemoteDesktop::DesktopMonitor::Desktops::DEFAULT;
	else if (find_substr(name, std::string("winlogon")) != -1) return RemoteDesktop::DesktopMonitor::Desktops::WINLOGON;
	return RemoteDesktop::DesktopMonitor::Desktops::SCREENSAVER;
}
