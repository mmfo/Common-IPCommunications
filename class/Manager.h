#include <vector>
#include <thread>
#include <list>
#include <mutex>
#include "../UdpSocket/UDPServer.h"
#include "Antenna.h"
#include "../IDD.h"
using namespace std;

typedef struct {
	unsigned short Opcode;
	time_t time;
	unsigned int msgNumber;
}MsgTime;

class Manager
{
public:
	vector<ControllerType> manager_controllers;
	DeviceType manager_deviceType;
	list< MsgTime>* manager_msgTracker;//arr that every element its list<MsgTime> 
	mutex manager_msgTrackerMutex;
	UDPClient client;
	UDPServer server;
	int count;

	Manager(vector<ControllerType>& controllers, DeviceType deviceType) {
		this->manager_controllers = controllers;// למי תשלח הודעה
		this->manager_deviceType = deviceType;// שם התקן TRANSMITTER || RECIVER

		this->server = UDPServer((unsigned short)MANAGER_PORT(this->manager_deviceType));
		this->client = UDPClient(5);
		thread t1(&Manager::sendControllMsgs, this);
		t1.detach();
		thread t2(&Manager::receiveMsgs, this);
		t2.detach();
	}
	~Manager() {
		delete(this->manager_msgTracker);
	}
	int getControllerInx(ControllerType ctype) {
		int i;
		for (size_t i = 0; i < manager_controllers.size(); i++)
		{
			if (manager_controllers[i] == ctype) {
				return i;
			}
		}
		return -1;
	}

	void sendControllMsgs() {//send to each controller message every 1 second
		Message msg = {};
		msg.header.msgSync = MSG_SYNC;
		msg.header.msgCounter = 0;
		msg.controlMessage.Ack = 0;
		MsgTime msgTime{};
		while (true)
		{
			//msgTracker
			for (int i = 0; i < this->manager_controllers.size(); i++)
			{
				ControllerType ctype = this->manager_controllers[i];
				msg.header.Opcode = ctypeToOpcode[ctype]; //A006: 40966
				msgTime.Opcode = ctypeToOpcode[ctype]; //A006: 40966
				msgTime.msgNumber = msg.header.msgCounter;
				msgTime.time = time(0); //time that you send the msg (from 1970)
				//lock
				this->manager_msgTrackerMutex.lock();
				this->manager_msgTracker[i].push_front(msgTime);      
				this->manager_msgTrackerMutex.unlock();
				//unlock
				this->client.sendTheMessage(msg, (unsigned short)CONTROLLER_PORT(ctype, this->manager_deviceType)/*port*/);//(unsigned short)CONTROLLER_PORT(ctype, this->manager_deviceType)
			}
			msg.header.msgCounter++;

			this->count = msg.header.msgCounter;

			this_thread::sleep_for(chrono::milliseconds(1000));
			//להודיע אחרי 5 שניות ההודעה הזו והזו לא התקבלה
			// 0 1 2
			//Go over msgTracker and find un acked messges
			time_t now = time(0);
			for (int i = 0; i < this->manager_controllers.size(); i++)
			{
				//lock
				this->manager_msgTrackerMutex.lock();
				while (!this->manager_msgTracker[i].empty())
				{
					MsgTime msgTime = this->manager_msgTracker[i].back();//return old value
					if (now - msgTime.time > 5)
						//if (msg.header.msgCounter > msgTime.msgNumber + 5)//5
					{
						this->manager_msgTracker[i].pop_back();
						cout << "controll msg not receive in the manager in 5 second" << " msgTime.msgNumber:" << msgTime.msgNumber << " msg.header.msgCounter:" << msg.header.msgCounter << " msgTime.Opcode:" << hex << msgTime.Opcode << endl;
					}
					else
						break;
				}
				this->manager_msgTrackerMutex.unlock();
				//unlock
			}
		}
	}
	void receiveMsgs() {//treated controll && data msg
		//msgTracker
		Message msg = {};
		while (true)
		{
			unsigned short myport = MANAGER_PORT(this->manager_deviceType);
			//cout << myport;
			//len  need to add check about the len if its correct 
			if (this->server.recieveData(&msg, myport) == -1)//which port am i
				cout << "Manager main recvfrom() failed with error code: %d" << endl;
			if (msg.header.msgSync == MSG_SYNC) {
				if (msg.header.Opcode != DATA_MSG_OPCODE) {
					//controll msg
					ControllerType ctype = opcodeToCtype[msg.header.Opcode];
					cout << "ctype    to manager    " << ctype << endl;
					cout << "opcode   to manager " << msg.header.Opcode << endl;
					cout << "ack  to manager " << (int)msg.controlMessage.Ack << endl;

					int idx = getControllerInx(ctype);
					if (msg.controlMessage.Ack == 0)
						cout << this->manager_deviceType << "  " << opcodeToCtype[msg.header.Opcode] << "   manager  erorr 100" << endl;
					else//valid ack
					{
						if (this->count > msg.header.msgCounter + 5)//time not valid
							cout << this->manager_deviceType << "  " << opcodeToCtype[msg.header.Opcode] << "   manager  erorr 101" << endl;
						else//pop from 	manager_msgTracker  msg to know msg time-5second valit
						{
							this->manager_msgTrackerMutex.lock();
							list< MsgTime> msglist = this->manager_msgTracker[/*opcodeToCtype[msg.header.Opcode]*/idx]; //index in the vector of the ctype
							list< MsgTime>::iterator iter = manager_msgTracker[idx].begin();//idx
							while (iter != manager_msgTracker[idx].end()) {
								if (iter->msgNumber == msg.header.msgCounter)
									iter = manager_msgTracker[idx].erase(iter);
								else
									iter++;
							}
							this->manager_msgTrackerMutex.unlock();
						}
					}
				}
				else//send data message
				{
					if (this->manager_deviceType == TRANSMITTER)//send to transmitter modem
						this->client.sendTheMessage(msg, CONTROLLER_PORT(CONTROLLER_TYPE_MODEM, this->manager_deviceType)/* this->controllersPort[CONTROLLER_TYPE_MODEM]*//*8883*/);//CONTROLLER_PORT(CONTROLLER_TYPE_MODEM, this->manager_deviceType)
					else
						this->client.sendTheMessage(msg, MSGGEN_PORT);//send to msg generator
				}
			}
			else
				cerr << "error manager !!!!!!!msg->header.msgSync == MSG_SYNC) \n";
		}
	}
};
