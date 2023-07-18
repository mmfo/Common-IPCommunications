#pragma once
class Modem :public Controller {
public:
	Modem(DeviceType deviceType) :Controller(CONTROLLER_TYPE_MODEM, deviceType) {

	}
};
