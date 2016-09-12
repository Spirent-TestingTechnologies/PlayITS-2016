#ifndef PhyIOHelp
#define PhyIOHelp

#include "Arduino.h"
#include "CarSimDefine.h"

class Help {
	
	private:
		
	public:
		Help();
		
		bool is_helpRequest();
		void Print_Help();
		
};

#endif