// EchoFilter
// Be Users Group @ UIUC - SoundMangler Project
// 3/4/1998

#include "Colors.h"
#include "EchoFilter.h"

EchoFilter::EchoFilter(const char* name)
	:SMFilter(name) {
	level = 1;
	delay = 10;
	level_knob = NULL;
	delay_field = NULL;
	activate_cb = NULL;
	delay_queue = new int16[1000000];
	queue_tail = 0;
	queue_head = 1;
	Run();
}

EchoFilter::EchoFilter(BMessage* data)
	:SMFilter(data) {
	data->FindFloat("level", &level);
	data->FindInt32("delay", &delay);
	level_knob = NULL;
	delay_field = NULL;
	activate_cb = NULL;
	delay_queue = new int16[1000000];
	queue_tail = 0;
	queue_head = 1;
	Run();
}

EchoFilter::~EchoFilter() {
	delete[] delay_queue;
}

status_t EchoFilter::Archive(BMessage* msg, bool deep) const { 
	BHandler::Archive(msg, deep);
	msg->AddString("class", "EchoFilter");
	msg->AddFloat("level", level);
	return B_OK; 
}

EchoFilter* EchoFilter::Instantiate(BMessage* data) {
	if(validate_instantiation(data, "EchoFilter"))
		return new EchoFilter(data);
	else
		return NULL;
}

void EchoFilter::Filter(int16* left, int16* right, int32 count) {
	if (activate_cb == NULL || activate_cb->Value() == B_CONTROL_ON) {
		if (activate_cb != NULL) {
			level = level_knob->Volume();
			delay = atol(delay_field->Text());
			if (delay > 999999)
				delay = 999999;
			if (delay < 2)
				delay = 2;
		}
		float out_balance = 1.0 + level;
		int32 current_sample = 0;
		while (current_sample < count) {
			left[current_sample] = (left[current_sample] + level * delay_queue[2 * queue_head]) / out_balance;
			right[current_sample] = (right[current_sample] + level * delay_queue[1 + 2 * queue_head]) / out_balance;
			delay_queue[2 * queue_tail] = left[current_sample];
			delay_queue[1 + 2 * queue_tail] = right[current_sample];
			current_sample++;
			queue_tail = (queue_tail + 1) % delay;
			queue_head = (queue_head + 1) % delay;
		}
	}
}

BView* EchoFilter::PrefsView() {
	BView* prefs_view = new BView(BRect(2,2,242,247),"prefs_view", B_FOLLOW_ALL_SIDES, B_WILL_DRAW|B_NAVIGABLE);
	prefs_view->SetViewColor(M_FILL_COLOR);
	BStringView* title = new BStringView(BRect(10,10,220,30), "title", "Echo Filter");
	title->SetFont(be_bold_font);
	prefs_view->AddChild(title);
	level_knob = new MVolume(NULL);
	level_knob->layout(BRect(10,50,40,80));
	level_knob->round = true;
	prefs_view->AddChild(level_knob);
	prefs_view->AddChild(new BStringView(BRect(50,50,120,70),"level_label","Filter Level"));
	delay_field = new BTextControl(BRect(10,90,110,110), "delay_field", "Delay:", "10", NULL);
	prefs_view->AddChild(delay_field);
	activate_cb = new BCheckBox(BRect(10,120,110,140), "activate_cb", "Activate", NULL);
	activate_cb->SetValue(B_CONTROL_ON);
	prefs_view->AddChild(activate_cb);
	return prefs_view;
}

void EchoFilter::MessageReceived( BMessage *msg ) {
	if (msg->what != B_PULSE) {
		printf("EchoFilter:\n");
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
	return new EchoFilter(name);
}

BView* AboutView() {
	BView* about_view = new BView(BRect(2,2,242,247),"about_view", B_FOLLOW_ALL_SIDES, B_WILL_DRAW);
	about_view->SetViewColor(DarkMetallicBlue);
	BStringView* title = new BStringView(BRect(10,10,220,30), "title", "Echo Filter");
	title->SetHighColor(BeInactiveControlGrey);
	title->SetFont(be_bold_font);
	about_view->AddChild(title);
	BTextView* credits1 = new BTextView(BRect(10,40,220,110), "credits1", BRect(0,0,210,70), B_FOLLOW_ALL_SIDES, B_WILL_DRAW|B_NAVIGABLE);
	credits1->SetText("This filter creates an echo in the sound stream.\n\nCreated by the Be Users Group at the University of Illinois at Urbana-Champaign.");
	credits1->MakeEditable(false);
	credits1->MakeSelectable(false);
	credits1->SetViewColor(DarkMetallicBlue);
	credits1->SetFontAndColor(be_plain_font, B_FONT_ALL, &BeInactiveControlGrey);
	about_view->AddChild(credits1);
	return about_view;
}
