// EchoFilter
// Be Users Group @ UIUC - SoundMangler Project
// 3/4/1998

#pragma once

#include "MVolume.h"
#include "SMFilter.h"

#pragma export on

extern "C" {
	SMFilter* MakeNewFilter(const char* name);
	BView* AboutView();
}

class LPFView;

class EchoFilter : public SMFilter {
	public:
		EchoFilter(const char* name);
		EchoFilter(BMessage* data);
		~EchoFilter();
		static EchoFilter* Instantiate(BMessage *data);
		virtual status_t Archive(BMessage* msg, bool deep = true) const;
		virtual void MessageReceived(BMessage* msg);
		virtual void Filter( int16* left, int16* right, int32 count);
		virtual BView* PrefsView();
	protected:
		float level;
		int32 delay;
		int16* delay_queue;
		uint32 queue_tail;
		uint32 queue_head;
		MVolume* level_knob;
		BTextControl* delay_field;
		BCheckBox* activate_cb;
};

#pragma export off
