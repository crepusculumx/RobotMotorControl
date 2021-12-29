//
// Created by crepusculumx on 2021/12/20.
//

#ifndef ROBOTMOTORCONTROL_USER_SRC_COMPROTOCOL_H_
#define ROBOTMOTORCONTROL_USER_SRC_COMPROTOCOL_H_

#include <stdbool.h>
#include <string.h>

#include "TypeChanger.h"
#include "CrcCheck.h"
#include "MotorController.h"

#define AXLE_NUM 18

#define MAX_PACKET_LENGTH 170

// 包头标识
#define HD_NUM 0xFA
#define ID_NUM 0x01

// 数据头标识
#define PRE 0xFB
#define BID 0x00

#define MSG_LENGTH 158

typedef struct AxleCmd {
  int16_t mode;
  int16_t current;
  int16_t velocity;
  int16_t position;
} AxleCmd, AxleState;

// 通信协议
typedef struct RxProtocolInner {
  uint8_t head[2];
  uint16_t cur;
  uint8_t temp[10];
  AxleCmd axle_cmd[AXLE_NUM];
} RxProtocolInner;

// 包结构
typedef struct RxProtocolPacket {
  uint8_t head[2];
  uint8_t length;
  RxProtocolInner inner_msg;
  uint16_t crc;
} RxProtocolPacket;

typedef struct TxProtocolInner {
  uint8_t head[2];
  uint16_t cur;
  uint8_t temp[10];
  AxleState axle_cmd[AXLE_NUM];
} TxProtocolInner;

typedef struct TxProtocolPacket {
  uint8_t head[2];
  uint8_t length;
  TxProtocolInner inner_msg;
  uint16_t crc;
} TxProtocolPacket;
/*
 * 解析通信协议
 * 根据data[0]-data[length - 1]内容，将解析完成内容写入*inner_msg
 */
int parse_rx_msg(uint8_t data[], size_t length, RxProtocolInner *inner_msg);

/*
 * 解析通信包
 * 根据data[0]-data[length - 1]内容，将解析完成内容写入*data_packet
 */
int parse_packet(const uint8_t data[], size_t length, RxProtocolPacket *data_packet);

int dump_packet(uint8_t data[], MotorState *motor_state[]);

#endif //ROBOTMOTORCONTROL_USER_SRC_COMPROTOCOL_H_
