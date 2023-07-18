#pragma once
#include "Controller.h"
//#include "../IDD.h"
class Antenna :public Controller
{
public:
	Antenna(DeviceType deviceType) :Controller(CONTROLLER_TYPE_ANTENNA,deviceType) {

	}	
};

