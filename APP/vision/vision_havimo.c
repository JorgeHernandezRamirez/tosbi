/*

 __    __    __    ______    __________    ______
|  |  |  |  |  |  |   ___|  |___    ___|  |   ___|
|  |  |  |  |  |  |  |          |  |      |  |
|  |  |  |  |  |  |  |          |  |      |  |
|  |  |  |__|  |  |  |___       |  |      |  |___
|__|  |________|  |______|      |__|      |______|

													iuctc.es

*/

#include "vision_havimo.h"
//#include <mega128.h>

extern byte TxPacket(byte bID, byte bInstruction, byte bParameterLength);
extern byte RxPacket(byte bRxLength);

void CaptureImage (void)
{
      byte bTxPacketLength,bRxPacketLength;
      CLEAR_BUFFER;
      bTxPacketLength = TxPacket(100,14,0); //send Start_Capture_Process 
}

int ProcessImage (char *posX, char *posY)
{                  
  
  int i,j;                   
  byte bTxPacketLength,bRxPacketLength;

  CLEAR_BUFFER;

  TxPacket(100,1,0); //Pole and wait for the last image to be processed
  bRxPacketLength = RxPacket(DEFAULT_RETURN_PACKET_SIZE);
  if(bRxPacketLength != DEFAULT_RETURN_PACKET_SIZE)
  {
		return 0;
  }
      
  for(i=1;i<15;i++) // read the results from HaViMo and Save them in the region buffer.
  {             
    	gbpParameter[0] = i*16; //Address 
  		gbpParameter[1] = 16; //Number of bytes to read
	    bTxPacketLength = TxPacket(100,2,2); //Read Buff       
	    RxPacket(DEFAULT_RETURN_PACKET_SIZE+16);
	    
		
        RegBuf[i-1].Cat=gbpRxBuffer[5];
        RegBuf[i-1].Color=gbpRxBuffer[6];
        RegBuf[i-1].NumPix=((word)gbpRxBuffer[7]+((word)gbpRxBuffer[8]<<8));
        RegBuf[i-1].AvX=(((unsigned long int)gbpRxBuffer[9]+((unsigned long int)gbpRxBuffer[10]<<8)+((unsigned long int)gbpRxBuffer[11]<<16)))/RegBuf[i-1].NumPix;
        RegBuf[i-1].AvY=(((unsigned long int)gbpRxBuffer[13]+((unsigned long int)gbpRxBuffer[14]<<8)+((unsigned long int)gbpRxBuffer[15]<<16)))/RegBuf[i-1].NumPix;
        RegBuf[i-1].MaxX=gbpRxBuffer[17];
        RegBuf[i-1].MinX=gbpRxBuffer[18];
        RegBuf[i-1].MaxY=gbpRxBuffer[19];
        RegBuf[i-1].MinY=gbpRxBuffer[20];
  }        
  CaptureImage(); //Capture the next image as soon as the last one is read                      
  MaxcntBall=0;

  for(i=0;i<14;i++) // search in the results for objects
  {
		if (RegBuf[i].Cat!=0) // if the region is valid
		{
		  switch(RegBuf[i].Color)
		  {
		  	case(Ball):     // and if it is ball
		  	if(RegBuf[i].NumPix>MaxcntBall)   // and bigger than the last found one
		  	{
		  	  Ballx=RegBuf[i].AvX;
		  	  Bally=RegBuf[i].AvY;
		  	  MaxcntBall=RegBuf[i].NumPix;
		  	}
		  	break;
		  }
		}          
  }

  *posX = Ballx;
  *posY = Bally;            
  return MaxcntBall>0;
}