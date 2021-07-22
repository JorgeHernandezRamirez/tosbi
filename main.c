#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include "TOSBI/tosbi.h"
#include "APP/movimiento/BLV012/motion.h"
#include "APP/movimiento/movimiento.h"
#include "APP/vision/vision_havimo.h"
#include "SYSTEM/tasks_interfaces.h"
#include <avr/pgmspace.h>
#include <libCM-5.h>

int ROBOTSTATUS;

void ver()
{
   char Ballx, Bally;
   char linea[18];      
   
   PortInitialize(); //Port In/Out Direction Definition
   RS485_RXD; //Set RS485 Direction to Input State.
   SerialInitialize(SERIAL_PORT0,1,RX_INTERRUPT);//RS485 Initializing(RxInterrupt)
   SerialInitialize(SERIAL_PORT1,DEFAULT_BAUD_RATE,RX_INTERRUPT); //RS232 Initializing(None Interrupt)
  
   gbRxBufferReadPointer = gbRxBufferWritePointer = 0;  //RS485 RxBuffer Clearing.
   sei();

   for(;;)
   {
      
      if(ProcessImage(&Ballx, &Bally)) 
      {     	   
         sprintf(linea, "Ball= (%d,%d)\r\n", Ballx, Bally);                              
         TxDString(linea);
      }

   }
}

void prueba()
{
   for(;;){TxDString("1\r\n");}
}

void prueba2()
{
         
   for(;;){TxDString("2\r\n");}
}

int main(void)
{
   int i;

   
   /*doPage(46);
   
   for(i = 0; i < MHD.NoOfPoses - 1; i++)
   { 
      MHD.CurrentPose++;
      MHD.PosePause = doPose(MHD.CurrentPage,MHD.CurrentPose);  
   }
   
   getCurrentServoPos(18);
   
   TxDString("Termine");

   
   doPage(48);
   
   for(i = 0; i < MHD.NoOfPoses - 1; i++)
   { 
      MHD.CurrentPose++;
      MHD.PosePause = doPose(MHD.CurrentPage,MHD.CurrentPose);  
   }*/
   
   
   while(1)
   {
   doPage(2);
   
   for(i = 0; i < MHD.NoOfPoses - 1; i++)
   { 
      MHD.CurrentPose++;
      MHD.PosePause = doPose(MHD.CurrentPage,MHD.CurrentPose);  
   }
   
   doPage(3);
   
   for(i = 0; i < MHD.NoOfPoses - 1; i++)
   { 
      MHD.CurrentPose++;
      MHD.PosePause = doPose(MHD.CurrentPage,MHD.CurrentPose);  
   }
   
   doPage(4);
   
   for(i = 0; i < MHD.NoOfPoses - 1; i++)
   { 
      MHD.CurrentPose++;
      MHD.PosePause = doPose(MHD.CurrentPage,MHD.CurrentPose);  
   }
   
   doPage(5);
   
   for(i = 0; i < MHD.NoOfPoses - 1; i++)
   { 
      MHD.CurrentPose++;
      MHD.PosePause = doPose(MHD.CurrentPage,MHD.CurrentPose);  
   }
   }


   /*char linea[150];
   
   uint16_t PoseSpeed;
   uint16_t PageSpeed;
   
   int pose = 1;
   int page = 2;
   
   for( pose = 0; pose < 7; pose ++)
   {

      PoseSpeed = pgm_read_byte_far  ((uint32_t)POSE_BASE_ADR + (uint32_t)POSE_0_OFFSET + (uint32_t)POSE_PAGE_SIZE*(uint32_t)page + (uint32_t)POSE_SIZE*(uint32_t)pose + (uint32_t)POSE_SPEED_ADR);
      PageSpeed = pgm_read_byte_far ((uint32_t)POSE_BASE_ADR +(uint32_t)POSE_PAGE_SIZE*(uint32_t)page + (uint32_t)POSE_PAGE_MOTION_SPEED);
   
      sprintf(linea, "(2,%d)\r\nPoseSpeed = %d\r\nPageSpeed = %d\r\n\r\n",pose,PoseSpeed, PageSpeed);
      TxDString(linea);
   }
   
   
   pose = 1;
   page = 4;

   for( pose = 0; pose < 7; pose ++)
   {

      PoseSpeed = pgm_read_byte_far  ((uint32_t)POSE_BASE_ADR + (uint32_t)POSE_0_OFFSET + (uint32_t)POSE_PAGE_SIZE*(uint32_t)page + (uint32_t)POSE_SIZE*(uint32_t)pose + (uint32_t)POSE_SPEED_ADR);
      PageSpeed = pgm_read_byte_far ((uint32_t)POSE_BASE_ADR +(uint32_t)POSE_PAGE_SIZE*(uint32_t)page + (uint32_t)POSE_PAGE_MOTION_SPEED);
   
      sprintf(linea, "(4,%d)\r\nPoseSpeed = %d\r\nPageSpeed = %d\r\n\r\n",pose,PoseSpeed, PageSpeed);
      TxDString(linea);
   }
   
   pose = 1;
   page = 3;
   
   for( pose = 0; pose < 7; pose ++)
   {

      PoseSpeed = pgm_read_byte_far  ((uint32_t)POSE_BASE_ADR + (uint32_t)POSE_0_OFFSET + (uint32_t)POSE_PAGE_SIZE*(uint32_t)page + (uint32_t)POSE_SIZE*(uint32_t)pose + (uint32_t)POSE_SPEED_ADR);
      PageSpeed = pgm_read_byte_far ((uint32_t)POSE_BASE_ADR +(uint32_t)POSE_PAGE_SIZE*(uint32_t)page + (uint32_t)POSE_PAGE_MOTION_SPEED);
   
      sprintf(linea, "(3,%d)\r\nPoseSpeed = %d\r\nPageSpeed = %d\r\n\r\n",pose,PoseSpeed, PageSpeed);
      TxDString(linea);
   }
   
   
   pose = 1;
   page = 5;

   for( pose = 0; pose < 7; pose ++)
   {

      PoseSpeed = pgm_read_byte_far  ((uint32_t)POSE_BASE_ADR + (uint32_t)POSE_0_OFFSET + (uint32_t)POSE_PAGE_SIZE*(uint32_t)page + (uint32_t)POSE_SIZE*(uint32_t)pose + (uint32_t)POSE_SPEED_ADR);
      PageSpeed = pgm_read_byte_far ((uint32_t)POSE_BASE_ADR +(uint32_t)POSE_PAGE_SIZE*(uint32_t)page + (uint32_t)POSE_PAGE_MOTION_SPEED);
   
      sprintf(linea, "(5,%d)\r\nPoseSpeed = %d\r\nPageSpeed = %d\r\n\r\n",pose,PoseSpeed, PageSpeed);
      TxDString(linea);
   }
   
      pose = 1;
   page = 6;

   for( pose = 0; pose < 7; pose ++)
   {

      PoseSpeed = pgm_read_byte_far  ((uint32_t)POSE_BASE_ADR + (uint32_t)POSE_0_OFFSET + (uint32_t)POSE_PAGE_SIZE*(uint32_t)page + (uint32_t)POSE_SIZE*(uint32_t)pose + (uint32_t)POSE_SPEED_ADR);
      PageSpeed = pgm_read_byte_far ((uint32_t)POSE_BASE_ADR +(uint32_t)POSE_PAGE_SIZE*(uint32_t)page + (uint32_t)POSE_PAGE_MOTION_SPEED);
   
      sprintf(linea, "(6,%d)\r\nPoseSpeed = %d\r\nPageSpeed = %d\r\n\r\n",pose,PoseSpeed, PageSpeed);
      TxDString(linea);
   }*/
   
   for(;;){}
   
   
}
