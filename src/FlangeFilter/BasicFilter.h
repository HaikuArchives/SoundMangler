// BasicFilter
// Be Users Group @ UIUC - SoundMangler Project
// 3/4/1998

#pragma once

#include "SMFilter.h"
#include "MVolume.h"

#pragma export on

extern "C" {
	SMFilter* MakeNewFilter(const char* name);
	BView* AboutView();
}

class BasicFilter : public SMFilter {
	public:
		BasicFilter(const char* name);
		BasicFilter(BMessage* data);
		~BasicFilter();
		static BasicFilter* Instantiate(BMessage *data);
		virtual status_t Archive(BMessage* msg, bool deep = true) const;
		virtual void MessageReceived(BMessage* msg);
		virtual void Filter( int16* left, int16* right, int32 count);
		virtual BView* PrefsView();
	protected:
		int16* delay_queue;
		uint16 left_queue_tail;
		uint16 left_queue_head;
		uint16 right_queue_tail;
		uint16 right_queue_head;
		uint16 min_delay;
		uint16 max_delay;
		float left_delay;
		float right_delay;
		float ldelta;
		float rdelta;
		BTextControl* delay_field;
		BTextControl* sweep_field;
		BTextControl* wavelength_field;
		BCheckBox* activate_cb;
		MVolume* level_knob;
		MVolume* feedback_knob;
};

#pragma export off
