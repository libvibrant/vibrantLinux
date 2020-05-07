#include "procscanner.h"

procScanner::procScanner(bool checkWindowFocus): checkWindowFocus(checkWindowFocus){
	if(checkWindowFocus){
		connectedToX = establishXCon();
		if(!connectedToX){
			checkWindowFocus = false;
		}
	}
}

procScanner::~procScanner(){
	if(connectedToX){
		xcb_disconnect(xcon.connection);
	}
}

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

void procScanner::setCheckWindowFocus(bool use){
	if(use){
		if(connectedToX){
			checkWindowFocus = use;
		}
		else{
			connectedToX = checkWindowFocus = establishXCon();
		}
	}
	else{
		xcb_disconnect(xcon.connection);
		connectedToX = checkWindowFocus = use;
	}
}

bool procScanner::isCheckingWindowFocus(){
	return checkWindowFocus;
}

const programInfo* procScanner::getSaturation(QListWidget* watchlist){
	if(checkWindowFocus){
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
		for(int i = 0; i < watchlist->count(); i++){
			auto info = watchlist->item(i)->data(Qt::UserRole).value<programInfo*>();
			if(procPath == info->path){
				return info;
			}
		}
	}
	else{
		processes.resize(0);

		QDir procDir("/proc");
		QStringList procs = procDir.entryList(QDir::Filter::Dirs);
		procs = procs.filter(QRegularExpression("^\\d+$"));

		for(auto proc: procs){
			//only program folders have numbers
			QFileInfo procInfo("/proc/"+proc+"/exe");
			processes.emplace_back(procInfo.canonicalFilePath());
		}

		for(int i = 0; i < watchlist->count(); i++){
			auto info = watchlist->item(i)->data(Qt::UserRole).value<programInfo*>();
			for(auto &process: processes){
				if(process == info->path){
					return info;
				}
			}
		}
	}

	return nullptr;
}
