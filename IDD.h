#pragma once
//For Data messages
#define DATA_MSG_OPCODE 0xD001 

//For Control Messages
#define MANAGER_TO_AMPLIFIER_CONTROL_MSG_OPCODE 0xA002
#define AMPLIFIER_TO_MANAGER_CONTROL_MSG_OPCODE 0xA003

#define MANAGER_TO_MODEM_CONTROL_MSG_OPCODE 0xA004
#define MODEM_TO_MANAGER_CONTROL_MSG_OPCODE 0xA005

#define MANAGER_TO_ANTENNA_CONTROL_MSG_OPCODE 0xA006
#define ANTENNA_TO_MANAGER_CONTROL_MSG_OPCODE 0xA007
#define MSG_SYNC 0xAA55

typedef struct {
    unsigned short msgSync;
    unsigned short Opcode;
    unsigned int msgCounter;
} Header;

typedef struct {
    Header header;
    char Ack; //This is the body message
}ControlMessage;

typedef struct {
    Header header;
    //There is no  body in data message
}DataMessage;

//Thursday

typedef union {
    Header header;
    ControlMessage controlMessage;
    DataMessage dataMessage;
}Message;

typedef enum {
    TRANSMITTER,
    RECEIVER
}DeviceType;

typedef enum ControllerType {
    CONTROLLER_TYPE_AMPLIFIER,
    CONTROLLER_TYPE_ANTENNA,
    CONTROLLER_TYPE_MODEM,
    CONTROLLER_TYPE_NUM
}ControllerType;


#define CONTROLLER_PORT(_ctype,_dev)(8880+_ctype+(_dev*CONTROLLER_TYPE_NUM))
#define MANAGER_PORT(_dev)(8080+_dev)
#define MSGGEN_PORT 8800

