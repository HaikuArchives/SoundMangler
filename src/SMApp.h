#pragma once
#include "SMWindow.h"

// The MIME type application signature
#define APP_SIGNATURE "application/x-vnd.BUGatUIUC-SoundMangler"


// The application object
class SMApp : public BApplication {
public:
	SMApp();
	~SMApp();
	virtual void RefsReceived(BMessage *message);
	virtual void MessageReceived(BMessage *message);
protected:
	BFilePanel* open_panel;
	BFilePanel* save_panel;
	SMWindow	*MainWindow;
};

