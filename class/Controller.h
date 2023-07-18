#pragma once
#include "../../COMMON/IDD.h"
#include <thread>
#include <iostream>
#include "../UdpSocket/UDPClient.h"
#include "../UdpSocket/UDPServer.h"
#include <unordered_map>
#include <chrono>
#include <string>
using namespace std;
static unordered_map<int, ControllerType> opcodeToCtype{
	{AMPLIFIER_TO_MANAGER_CONTROL_MSG_OPCODE,CONTROLLER_TYPE_AMPLIFIER},//A003, 0
	{ANTENNA_TO_MANAGER_CONTROL_MSG_OPCODE,CONTROLLER_TYPE_ANTENNA},//A007, 1
	{MODEM_TO_MANAGER_CONTROL_MSG_OPCODE,CONTROLLER_TYPE_MODEM}//A005, 2
};
static unordered_map< ControllerType, int> ctypeToOpcode{
	{CONTROLLER_TYPE_AMPLIFIER ,MANAGER_TO_AMPLIFIER_CONTROL_MSG_OPCODE},//0 ,A002
	{CONTROLLER_TYPE_ANTENNA,MANAGER_TO_ANTENNA_CONTROL_MSG_OPCODE},// 1 ,A006
	{CONTROLLER_TYPE_MODEM,MANAGER_TO_MODEM_CONTROL_MSG_OPCODE}//2 ,A004 
};
static unordered_map<ControllerType,int > opcodeToDevtype{
	{CONTROLLER_TYPE_AMPLIFIER,AMPLIFIER_TO_MANAGER_CONTROL_MSG_OPCODE},
	{CONTROLLER_TYPE_ANTENNA,ANTENNA_TO_MANAGER_CONTROL_MSG_OPCODE},
	{CONTROLLER_TYPE_MODEM,MODEM_TO_MANAGER_CONTROL_MSG_OPCODE}
};
class Controller
{
public:
	UDPClient client;
	UDPServer server;
	DeviceType devType;
	ControllerType controllerType;
	unsigned int count;
	//virtual void reciveMsgs() = 0;//
	Controller(ControllerType controllerType, DeviceType devType) {
		this->controllerType = controllerType;
		this->devType = devType;
		this->server = UDPServer((unsigned short)CONTROLLER_PORT(this->controllerType, this->devType));
		this->client = UDPClient(5);
		this->count = 0;
		cout << "xx\n";
		thread t1(&Controller::get_sendMessages, this);
		t1.detach();
	}
	void get_sendMessages() {
		Message msg = {};
		while (1) {
			getMessage(&msg);
			sendMessage(msg);
		}
	}
	void getMessage(Message* msg) {
		if (this->server.recieveData(msg, (unsigned short)CONTROLLER_PORT(this->controllerType, this->devType)) == -1)//which port am i
			cout << "Controller main recvfrom() failed with error code: %d" << endl;
		//add len
		if (msg->header.msgSync == MSG_SYNC) {
			if (msg->header.Opcode == DATA_MSG_OPCODE) {
				//DataMessage
				cout << "\n" << "DataMessage" << "\n";
				cout << "\n" << "sizeof(message) " << sizeof(msg) << "\n";
				cout << "Data Opcode: " << hex << msg->header.Opcode << "\n";
				cout << "Data msgSync: " << hex << msg->header.msgSync << "\n";
				cout << "Data msgCounter: " << (int)msg->header.msgCounter << "\n";
			}
			else {
				this->count++;
				cout << "\n" << "ControlMessage " << "\n";
				cout << "\n" << "sizeof(message) " << sizeof(msg) << "\n";
				cout << "Data Opcode: " << hex << msg->header.Opcode << "\n";
				cout << "Data msgSync: " << hex << msg->header.msgSync << "\n";
				cout << "Data msgCounter: " <<(int)msg->header.msgCounter << "\n";
				cout << "Data Ack: " << (int)(msg->controlMessage.Ack) << "\n";
			}
		}
		else {
			cout << "error Controller !!!!!!! msg->header.msgSync != MSG_SYNC) \n";

		}
	}
	/*unsigned short findKeyByValue() {
		for (unordered_map<int, ControllerType>::iterator it = opcodeToCtype.begin(); it != opcodeToCtype.end(); ++it) {
			if (it->second == this->controllerType) return it->first;
		}
	}*/
	void sendMessage(Message msg) {
		if (msg.header.msgSync == MSG_SYNC) {
			if (msg.header.Opcode != DATA_MSG_OPCODE) {
				//controll msg
				if (this->count == msg.header.msgCounter+1 && msg.header.Opcode == ctypeToOpcode[this->controllerType]) {
					msg.controlMessage.Ack = 1;
				}
				else
					cerr << this->devType << "  " << this->controllerType << "  Controller erorr 2" << endl;
				msg.controlMessage.header.Opcode = opcodeToDevtype[this->controllerType];
				this->client.sendTheMessage(msg, MANAGER_PORT(this->devType));//send to TRANSMITTER,RECIEVER manager
			}
			else {
				//data msg
				if (this->devType == TRANSMITTER){
					this->client.sendTheMessage(msg, CONTROLLER_PORT(CONTROLLER_TYPE_MODEM, RECEIVER));//CONTROLLER_PORT(CONTROLLER_TYPE_MODEM, RECEIVER)  modem TRANSMITTER send to modem RECIEVER 

				}
				else{
					this->client.sendTheMessage(msg, MANAGER_PORT(this->devType));//modem RECIEVER  send to manager RECIEVER

				}
			}
		}
		else
			cerr << this->devType << "  " << this->controllerType << "  Controller erorr 1" << endl;
	}
};