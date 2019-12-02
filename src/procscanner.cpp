#include "procscanner.h"

procScanner::procScanner(){
	#ifndef VIBRANT_LINUX_NO_XCB
	connectedToX = useX = establishXCon();
	#endif
}

procScanner::~procScanner(){
	#ifndef VIBRANT_LINUX_NO_XCB
	if(connectedToX){
		xcb_disconnect(xcon.connection);
	}
	#endif
}

#ifndef VIBRANT_LINUX_NO_XCB
bool procScanner::establishXCon(){
	xcon.connection = xcb_connect(nullptr, nullptr);
	if(xcb_connection_has_error(xcon.connection)){
		xcb_disconnect(xcon.connection);
		return false;
	}

	xcb_intern_atom_cookie_t *cookies;
	cookies = xcb_ewmh_init_atoms(xcon.connection, &xcon);
	if(!xcb_ewmh_init_atoms_replies(&xcon, cookies, nullptr)){
		xcb_disconnect(xcon.connection);
		return false;
	}

	return true;
}

void procScanner::setUseX(bool use){
	if(use){
		if(connectedToX){
			useX = use;
		}
		else{
			connectedToX = useX = establishXCon();
		}
	}
	else{
		xcb_disconnect(xcon.connection);
		connectedToX = useX = use;
	}
}

bool procScanner::isUsingX(){
	return useX;
}

bool procScanner::isConnectedToX(){
	return connectedToX;
}
#endif

QListWidgetItem* procScanner::getVibrance(QListWidget *&watchList){
	QListWidgetItem *res = nullptr;

	#ifndef VIBRANT_LINUX_NO_XCB
	if(useX){
		//get the current active window
		xcb_get_property_cookie_t cookie;
		xcb_window_t activeWindow;
		xcb_generic_error_t *e = nullptr;

		cookie = xcb_ewmh_get_active_window(&xcon, 0);
		if(!xcb_ewmh_get_active_window_reply(&xcon, cookie, &activeWindow, &e)){
			return nullptr;
		}

		uint32_t pid = 0;
		cookie = xcb_ewmh_get_wm_pid(&xcon, activeWindow);
		if(!xcb_ewmh_get_wm_pid_reply(&xcon, cookie, &pid, &e)){
			return nullptr;
		}

		QString procPath = QFileInfo("/proc/"+QString::number(pid)+"/exe").canonicalFilePath();

		//check if the active window program is in our list
		for(int i = 0; i < watchList->count(); i++){
			QListWidgetItem *tmp = watchList->item(i);
			QString itemPath = getItemPath(tmp);
			if(procPath == getItemPath(tmp)){
				res = tmp;
				break;
			}
		}

		return res;
	}
	else{
	#endif
		processes.resize(0);

		QDir procDir("/proc");
		QStringList procs = procDir.entryList(QDir::Filter::Dirs);
		procs = procs.filter(QRegularExpression("^\\d+$"));

		for(auto proc: procs){
			//only program folders have numbers
			QFileInfo procInfo("/proc/"+proc+"/exe");
			processes.emplace_back(procInfo.canonicalFilePath());
		}

		//traverse in reverse. First items get priority in vibrance control
		for(int i = watchList->count()-1; i >= 0; i--){
			QListWidgetItem *item = watchList->item(i);
			for(auto &process: processes){
				if(process == getItemPath(item)){
					res = watchList->item(i);
				}
			}
		}
	#ifndef VIBRANT_LINUX_NO_XCB
	}
	#endif

	return res;
}
