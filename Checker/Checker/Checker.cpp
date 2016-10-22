// Checker.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <thread>
#include "Desktop_Monitor.h"
#include <iostream>

int main()
{
	RemoteDesktop::DesktopMonitor d;
	while (true) {
		std::chrono::milliseconds dura(1000);
		std::this_thread::sleep_for(dura);
		auto activedesktop = d.get_InputDesktop();
		if (activedesktop&RemoteDesktop::DesktopMonitor::Desktops::DEFAULT) {
			std::cout << "Dekstop thread is default" << std::endl;
		} else 	if (activedesktop&RemoteDesktop::DesktopMonitor::Desktops::SCREENSAVER) {
			std::cout << "Dekstop thread is screen-saver" << std::endl;
		}
		else if (activedesktop&RemoteDesktop::DesktopMonitor::Desktops::WINLOGON) {
			std::cout << "Dekstop thread is winlogon" << std::endl;
		}


		auto activedesktop1 = d.get_ThreadDesktop();
		if (activedesktop1&RemoteDesktop::DesktopMonitor::Desktops::DEFAULT) {
			std::cout << "get_ThreadDesktop thread is default" << std::endl;
		}
		else if (activedesktop1&RemoteDesktop::DesktopMonitor::Desktops::SCREENSAVER) {
			std::cout << "get_ThreadDesktop thread is screen-saver" << std::endl;
		}
		else if (activedesktop1&RemoteDesktop::DesktopMonitor::Desktops::WINLOGON) {
			std::cout << "get_ThreadDesktop thread is winlogon" << std::endl;
		}

		auto activesession = WTSGetActiveConsoleSessionId();
		std::cout << "activesession is " << activesession<< std::endl;
	}
    return 0;
}

