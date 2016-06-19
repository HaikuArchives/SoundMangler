// HighPassFilter
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

class LPFView;

class HighPassFilter : public SMFilter {
	public:
		HighPassFilter(const char* name);
		HighPassFilter(BMessage* data);
		~HighPassFilter();
		static HighPassFilter* Instantiate(BMessage *data);
		virtual status_t Archive(BMessage* msg, bool deep = true) const;
		virtual void MessageReceived(BMessage* msg);
		virtual void Filter( int16* left, int16* right, int32 count);
		virtual BView* PrefsView();
	protected:
		float level;
		int16* delay_queue;
		int16 queue_length;
		uint16 queue_tail;
		uint16 queue_head;
		MVolume* level_knob;
		BCheckBox* activate_cb;
};

#pragma export off
