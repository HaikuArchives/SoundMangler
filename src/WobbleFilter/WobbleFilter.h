// WobbleFilter
// Be Users Group @ UIUC - SoundMangler Project
// 3/4/1998

#pragma once

#include "SMFilter.h"

#pragma export on

extern "C" {
	SMFilter* MakeNewFilter(const char* name);
	BView* AboutView();
}

class WobbleFilter : public SMFilter {
	public:
		WobbleFilter(const char* name);
		WobbleFilter(BMessage* data);
		~WobbleFilter();
		static WobbleFilter* Instantiate(BMessage *data);
		virtual status_t Archive(BMessage* msg, bool deep = true) const;
		virtual void MessageReceived(BMessage* msg);
		virtual void Filter( int16* left, int16* right, int32 count);
		virtual BView* PrefsView();
	protected:
		float* wobble_data;
		int32 wobble;
};

#pragma export off
