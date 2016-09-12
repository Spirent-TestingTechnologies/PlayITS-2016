#include "PhyIOHelp.h"

Help::Help(){}

bool Help::is_helpRequest(){
	//look at the first entry of the Serial Input and check if it's an question Mark or
	//"help"
	if((char)Serial.peek() == '?'){
		return true; 
	}else{
		return false;
	}
}

void Help::Print_Help(){
	String a = Serial.readStringUntil(',');
	//vergleiche alle Eingaben mit den jeweiligen Moduleinträgen
	
	#define pr XSERIAL.println
	
	if(a == "?"){
		pr("----------------------------------- Help -----------------------------------\n");
		pr("If you want to enter a command over the Serial Input, ");
		pr("you simply have to enter the numbers in the following order:");
		pr("\n  ID, MODULE, FUNCION, PARAMETER1, ..., PARAMETERN");
		pr("\nWhere ID stands for the Component you want to give the command to,");
		pr("Module stands for the module you want to use,");
		pr("Functions stands for the function you want to use in cunjunction");
		pr("with the module,");
		pr("and PARAMETER1 - N are all the parameter one needs for the given function.");
		
		pr("\nIf you need more specific help for a certain module, you need to type in:");
		pr("\n  ? + M");
		pr("\nWhere M stands for either the module name or -number");
		pr("For example: ?LED01 or ?30 will give you more specific information on");
		pr("how to use the LED module and which IDs correspond to which LEDs.");
		pr("This also tells you whether the module is even active or not.");
		
		pr("\nIf you need a reminder on which modules there even are, simply type:");
		pr("?Module");
	}
	else if (a == "?Module"){
		pr("-------------------------------- Modules --------------------------------\n");
		pr("");
		#ifdef ColorView_Module
			pr("ColorView : CV01 or 10");
		#endif
		#ifdef PingEcho_Module
			pr("Ping Echo 2 : PE02  or 21");
		#endif
		#ifdef LED_Module
			pr("LED         : LED01 or 30");
		#endif
		#ifdef Relay_Module
			pr("Relay       : REL01 or 40");
		#endif
		#ifdef Motor_Module
			pr("Motor       : MM01  or 50");
		#endif
		#ifdef Button_Module
			pr("Button      : DR01  or 60");
		#endif
		#ifdef Lightsensor_Module
			pr("Lightsensor : LS01  or 70");
		#endif
		#ifdef RFID_Module
			pr("RFID        : RF01  or 80");
		#endif
		#ifdef Theft_Module
			pr("LED-Strip   : TD01  or 85");
		#endif
		#ifdef Stripe_Module
			pr("LED-Strip   : IR01  or 90");
		#endif
	}			
	else if (a == "?CV01" || a == "?10"){
		pr("spezifische Hilfe");
	}
	else if (a == "?PE02" || a == "?21"){
		pr("spezifische Hilfe");
	}
	else if (a == "?LED01" || a == "?30"){
		pr("spezifische Hilfe");
	}
	else if (a == "?REL01" || a == "?40"){
		pr("spezifische Hilfe");
	}
	else if (a == "?MM01" || a == "?50"){
		pr("spezifische Hilfe");
	}
	else if (a == "?DR01" || a == "?60"){
		pr("spezifische Hilfe");
	}
	else if (a == "?LS01" || a == "?70"){
		pr("spezifische Hilfe");
	}
	else if (a == "?RF01" || a == "?80"){
		pr("spezifische Hilfe");
	}
	else if (a == "?IR01" || a == "?90"){
		pr("spezifische Hilfe");
	}
	else if (a == "?TD01" || a == "?85"){
		pr("spezifische Hilfe");
	}
	
	#undef pr
}
