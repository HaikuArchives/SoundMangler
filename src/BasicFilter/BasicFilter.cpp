// BasicFilter
// Be Users Group @ UIUC - SoundMangler Project
// 3/4/1998

#include "Colors.h"
#include "BasicFilter.h"

BasicFilter::BasicFilter(const char* name)
	:SMFilter(name) {
	Run();
}

BasicFilter::BasicFilter(BMessage* data)
	:SMFilter(data) {
	Run();
}

BasicFilter::~BasicFilter() {
}

status_t BasicFilter::Archive(BMessage* msg, bool deep) const { 
	BHandler::Archive(msg, deep);
	msg->AddString("class", "BasicFilter");
	return B_OK; 
}

BasicFilter* BasicFilter::Instantiate(BMessage* data) {
	if(validate_instantiation(data, "BasicFilter"))
		return new BasicFilter(data);
	else
		return NULL;
}

void BasicFilter::Filter(int16* left, int16* right, int32 count) {
}

BView* BasicFilter::PrefsView() {
	BView* prefs_view = new BView(BRect(2,2,242,247),"prefs_view", B_FOLLOW_ALL_SIDES, B_WILL_DRAW|B_NAVIGABLE);
	prefs_view->SetViewColor(BeBackgroundGrey);
	BStringView* title = new BStringView(BRect(10,10,220,30), "title", "Filter");
	title->SetFont(be_bold_font);
	prefs_view->AddChild(title);
	return prefs_view;
}

void BasicFilter::MessageReceived( BMessage *msg ) {
	if (msg->what != B_PULSE) {
		printf("BasicFilter:\n");
		msg->PrintToStream();
		printf("\n");
	}
	switch(msg->what) {
		default: {
			BHandler::MessageReceived(msg);
			break;
		}
	}
}

SMFilter* MakeNewFilter(const char* name) {
	return new BasicFilter(name);
}

BView* AboutView() {
	BView* about_view = new BView(BRect(2,2,242,247),"about_view", B_FOLLOW_ALL_SIDES, B_WILL_DRAW);
	about_view->SetViewColor(DarkMetallicBlue);
	BStringView* title = new BStringView(BRect(10,10,220,30), "title", "Filter");
	title->SetHighColor(BeInactiveControlGrey);
	title->SetFont(be_bold_font);
	about_view->AddChild(title);
	BTextView* credits1 = new BTextView(BRect(10,40,220,110), "credits1", BRect(0,0,210,70), B_FOLLOW_ALL_SIDES, B_WILL_DRAW|B_NAVIGABLE);
	credits1->SetText("This is a test filter.\n\nReplace this message with copyright info.");
	credits1->MakeEditable(false);
	credits1->MakeSelectable(false);
	credits1->SetViewColor(DarkMetallicBlue);
	credits1->SetFontAndColor(be_plain_font, B_FONT_ALL, &BeInactiveControlGrey);
	about_view->AddChild(credits1);
	return about_view;
}
