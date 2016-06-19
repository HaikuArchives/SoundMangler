//	SMRefFilter
//	Matt Wronkiewicz 2/11/98

#pragma once

#include "SMApp.h"

class SMRefFilter : public BRefFilter
	{
	public:
		virtual bool Filter(const entry_ref* ref, BNode* node, struct stat* st, const char* filetype)
			{ return (strcmp(filetype, SM_FILE_SIGNATURE) == 0) || (strcmp(filetype, "application/x-vnd.Be-directory") == 0); }
	};
