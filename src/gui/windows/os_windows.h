/*
 * os_windows.h
 *
 *  Created on: 21/ott/2014
 *      Author: fhorse
 */

#ifndef OS_WINDOWS_H_
#define OS_WINDOWS_H_

#include <windowsx.h>
#include <shlobj.h>

double high_resolution_ms(void);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)) && defined (__WIN32__)
#include <QtCore/QtPlugin>
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin);
#endif

void gui_init(int argc, char **argv) {
	static int zero = 0;

	memset(&qt, 0, sizeof(qt));

	qt.app = new QApplication(zero, NULL);

	info.gui = TRUE;
	gui.in_update = FALSE;
	gui.main_win_lfp = TRUE;

	{
		OSVERSIONINFO win_info;

		info.gui = TRUE;
		ZeroMemory(&win_info, sizeof(OSVERSIONINFO));
		win_info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx(&win_info);
		gui.version_os = ((win_info.dwMajorVersion * 10) | win_info.dwMinorVersion);
	}

	/*
	// cerco il numero dei cores
	{
		SYSTEM_INFO info;

		GetSystemInfo(&info);
		gui.cpu_cores = info.dwNumberOfProcessors;
	}
	*/

	// cerco la Documents e imposto la directory base
	{
		switch (gui.version_os) {
			case WIN_EIGHTP1:
			case WIN_EIGHT:
			case WIN_SEVEN:
			case WIN_VISTA:
				// FIXME : non funziona sotto wingw
				//SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_CREATE, NULL, &gui.home);
				//break;
			case WIN_XP64:
			case WIN_XP:
			default:
				SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, 0, gui.home);
				break;
		}

		if (info.portable) {
			char path[sizeof(info.base_folder)], *dname;
			DWORD length = GetModuleFileName(NULL, (LPSTR) &path, sizeof(path));

			if (length == 0) {
				fprintf(stderr, "INFO: Error resolving exe path.\n");
				info.portable = FALSE;
			} else if (length == sizeof(info.base_folder)) {
				fprintf(stderr, "INFO: Path too long. Truncated.\n");
				info.portable = FALSE;
			}

			dname = dirname(path);
			strcpy(info.base_folder, dname);
		}

		if (!info.portable) {
			sprintf(info.base_folder, "%s/%s", gui.home, NAME);
		}
	}

	// avvio il contatore dei millisecondi
	{
		uint64_t pf;

		QueryPerformanceFrequency((LARGE_INTEGER *) &pf);
		gui.frequency = (double) pf;
		QueryPerformanceCounter((LARGE_INTEGER *) &pf);
		gui.counter_start = pf;
		gui_get_ms = high_resolution_ms;
	}
}
void gui_sleep(double ms) {
	if (ms > 0) {
		Sleep(ms);
	}
}
HWND gui_emu_frame_id(void) {
	HWND wid = (HWND) qt.screen->winId();

	return (wid);
}

double high_resolution_ms(void) {
	uint64_t time, diff;

	QueryPerformanceCounter((LARGE_INTEGER *) &time);
	diff = ((time - gui.counter_start) * 1000) / gui.frequency;

	return ((double) (diff & 0xffffffff));
}

#endif /* OS_WINDOWS_H_ */
