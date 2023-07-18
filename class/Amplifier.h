
#include "Controller.h"

class Amplifier :public Controller {
public:
	Amplifier(DeviceType deviceType) :Controller(CONTROLLER_TYPE_AMPLIFIER, deviceType) {

	}
};