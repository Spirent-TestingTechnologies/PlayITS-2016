#ifndef PhyIOAutonom
#define PhyIOAutonom

#include <CarSim.h>


class Autonom {
	
	private:
		bool time_check; // time checks are for checking, if a certain time stamp has passed


		int distance; //for checking the distance to an object
		int speed; //for setting the speed of the motor

		long unsigned current_time; // for checking timing intervalls in the Autonom mode
		
		Car* c;
		
		void update_time();

	public:
		
		Autonom(Car* car);
		void start();
		bool is_autonom();
		void reset();
		
};

#endif