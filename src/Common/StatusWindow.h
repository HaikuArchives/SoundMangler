#pragma once

class StatusWindow;

class StatusWindow : public BWindow
	{
	public:
		StatusWindow(BRect frame, const char* title);
		void UpdateStatus(float delta, char* label);
	protected:
		BStatusBar* status_bar;
	};
