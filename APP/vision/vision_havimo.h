/*

 __    __    __    ______    __________    ______
|  |  |  |  |  |  |   ___|  |___    ___|  |   ___|
|  |  |  |  |  |  |  |          |  |      |  |
|  |  |  |  |  |  |  |          |  |      |  |
|  |  |  |__|  |  |  |___       |  |      |  |___
|__|  |________|  |______|      |__|      |______|

													iuctc.es

*/

#ifndef VISION_HAVIMO
#define VISION_HAVIMO

#include <libCM-5.h>

#define CLEAR_BUFFER gbRxBufferReadPointer = gbRxBufferWritePointer
#define DEFAULT_RETURN_PACKET_SIZE 6
#define RS485_TXD PORTE &= ~_BV(PE3),PORTE |= _BV(PE2)  //_485_DIRECTION = 1
#define RS485_RXD PORTE &= ~_BV(PE2),PORTE |= _BV(PE3)  //PORT_485_DIRECTION = 0

struct Regions
{
  unsigned char Cat,Color;
  unsigned int NumPix;
  unsigned char AvX,AvY;
  unsigned char MaxX,MaxY,MinX,MinY;
} RegBuf[15];               

enum Colors {Unknown,Ball,Field,MyGoal,OppGoal,Robot,Cyan,Magenta};

unsigned int MaxcntBall;
unsigned char Ballx,Bally;
char BallFound;

#endif