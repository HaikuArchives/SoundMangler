#ifndef _MVOLUME_H
#define _MVOLUME_H

#include "layout.h"

class MVolume : public MView, public BControl
{
	protected:	thread_id mousethread;
				bool ispressed;

	private:	float volume;
				BHandler *target;
				BPoint lastvoldot;
				BPoint clickpoint;
				
	public:		static bool	round;
				MVolume(BHandler*);
				virtual ~MVolume();
				virtual	minimax	layoutprefs();
				BRect layout(BRect);
				virtual void DetachedFromWindow();
				virtual void Draw(BRect);
				virtual void DrawVolume(void);
				inline float Volume() { return volume;};
						void SetVolume(float vol);
				virtual void MouseDown(BPoint);
				static	long _mousetracker(void *arg);
						void _MouseTracker();
				virtual void KeyDown(const char *bytes, int32 numbytes);
};

#endif
