// WobbleFilter
// Be Users Group @ UIUC - SoundMangler Project
// 3/4/1998

#include "Colors.h"
#include "WobbleFilter.h"

WobbleFilter::WobbleFilter(const char* name)
	:SMFilter(name) {
	wobble_data = new float[100000];
	int32 index = 0;
	while (index < 100000) {
		wobble_data[index] = sin(100000.0 * 3.14159265358 / float(index));
		index++;
	}
	wobble = 0;
	Run();
}

WobbleFilter::WobbleFilter(BMessage* data)
	:SMFilter(data) {
	Run();
}

WobbleFilter::~WobbleFilter() {
}

status_t WobbleFilter::Archive(BMessage* msg, bool deep) const { 
	BHandler::Archive(msg, deep);
	msg->AddString("class", "WobbleFilter");
	return B_OK; 
}

WobbleFilter* WobbleFilter::Instantiate(BMessage* data) {
	if(validate_instantiation(data, "WobbleFilter"))
		return new WobbleFilter(data);
	else
		return NULL;
}

void WobbleFilter::Filter(int16* left, int16* right, int32 count) {
		int32 current_sample = 0;
		while (current_sample < count) {
			left[current_sample] *= wobble_data[wobble];
			right[current_sample] *= wobble_data[wobble];
			wobble = (wobble + 1) % 100000;
			current_sample++;
		}
}

BView* WobbleFilter::PrefsView() {
	BView* prefs_view = new BView(BRect(2,2,242,247),"prefs_view", B_FOLLOW_ALL_SIDES, B_WILL_DRAW|B_NAVIGABLE);
	prefs_view->SetViewColor(BeBackgroundGrey);
	BStringView* title = new BStringView(BRect(10,10,220,30), "title", "Filter");
	title->SetFont(be_bold_font);
	prefs_view->AddChild(title);
	return prefs_view;
}

void WobbleFilter::MessageReceived( BMessage *msg ) {
	if (msg->what != B_PULSE) {
		printf("WobbleFilter:\n");
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
	return new WobbleFilter(name);
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
