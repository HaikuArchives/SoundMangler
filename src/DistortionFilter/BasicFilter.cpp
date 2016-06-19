// BasicFilter
// Be Users Group @ UIUC - SoundMangler Project
// 3/4/1998

#include "Colors.h"
#include "BasicFilter.h"

BasicFilter::BasicFilter(const char* name)
	:SMFilter(name) {
	int32 map_size = SHRT_MAX * 2 + 1;
	map = new int16[map_size];
	int32 index = 0;
	while (index < (SHRT_MAX - 255)) {
		map[index] = -pow(float(SHRT_MAX - index) / float(SHRT_MAX),.25) * SHRT_MAX;
		index++;
	}
	index += 512;
	while (index < map_size) {
		map[index] = pow(float(index - SHRT_MAX) / float(SHRT_MAX),.25) * SHRT_MAX;
		index++;
	}
	level = .5;
	level_knob = NULL;
	activate_cb = NULL;
	Run();
}

BasicFilter::BasicFilter(BMessage* data)
	:SMFilter(data) {
	Run();
}

BasicFilter::~BasicFilter() {
	delete[] map;
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
	if (activate_cb == NULL || activate_cb->Value() == B_CONTROL_ON) {
		if (level_knob != NULL)
			level = level_knob->Volume() / 2.0;
		float normal_level = 1.0 - level;
		int32 current_sample = 0;
		while (current_sample < count) {
			left[current_sample] = map[left[current_sample] + SHRT_MAX] * level + left[current_sample] * normal_level;
			right[current_sample] = map[right[current_sample] + SHRT_MAX] * level + right[current_sample] * normal_level;
			current_sample++;
		}
	}
}

BView* BasicFilter::PrefsView() {
	BView* prefs_view = new BView(BRect(2,2,242,247),"prefs_view", B_FOLLOW_ALL_SIDES, B_WILL_DRAW|B_NAVIGABLE);
	prefs_view->SetViewColor(M_FILL_COLOR);
	BStringView* title = new BStringView(BRect(10,10,220,30), "title", "Distortion Filter");
	title->SetFont(be_bold_font);
	prefs_view->AddChild(title);
	level_knob = new MVolume(NULL);
	level_knob->layout(BRect(10,50,40,80));
	level_knob->round = true;
	prefs_view->AddChild(level_knob);
	prefs_view->AddChild(new BStringView(BRect(50,50,160,70),"level_label","Distortion Level"));
	activate_cb = new BCheckBox(BRect(10,90,110,110), "activate_cb", "Activate", NULL);
	activate_cb->SetValue(B_CONTROL_ON);
	prefs_view->AddChild(activate_cb);
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
	BStringView* title = new BStringView(BRect(10,10,220,30), "title", "Distortion Filter");
	title->SetHighColor(BeInactiveControlGrey);
	title->SetFont(be_bold_font);
	about_view->AddChild(title);
	BTextView* credits1 = new BTextView(BRect(10,40,220,110), "credits1", BRect(0,0,210,70), B_FOLLOW_ALL_SIDES, B_WILL_DRAW|B_NAVIGABLE);
	credits1->SetText("This filter overdrives the audio stream to create a distortion effect.\n\nCreated by the Be Users Group at the University of Illinois.");
	credits1->MakeEditable(false);
	credits1->MakeSelectable(false);
	credits1->SetViewColor(DarkMetallicBlue);
	credits1->SetFontAndColor(be_plain_font, B_FONT_ALL, &BeInactiveControlGrey);
	about_view->AddChild(credits1);
	return about_view;
}
