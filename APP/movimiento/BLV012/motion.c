/* 
 * University of Plymouth Bioloid Robot Controller
 * for Atmel Mega 128
 * FIRA England Robot Football Team
 * - Based on Dynamixel Evaluation Example (Original Author : Byoung Soo Kim)
 * - Initial support for executing Motion Editor Pages
 * Author : Joerg Wolf, joerg.wolf -xaxtx- plymouth.ac.uk
 * 
 * Version 0.11
 */
#define ENABLE_BIT_DEFINITIONS 
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include <inttypes.h>
#include <string.h>
#include <stdio.h>
#include "motion.h"
#include "robot-dash.h"
#include <libCM-5.h>
#include <math.h>
#include <avr/delay.h>

extern volatile unsigned int ROBOTSTATUS;

uint16_t doPose(int page,char pose);
uint16_t doPage(int page);
void ContinueMotion_HeartBeat(void);
void MotionInitialise(void);


uint16_t abs(int16_t n)
{
	if(n<0){ return n*-1;}
	return n;
}

int _abs_int(int n)
{
	if(n<0){ return -n;}
	return n;
}

int getCurrentServoPos(uint8_t ServoNo)
{
	uint16_t POS_LB=0,POS_HB=0;
	gbpParameter[0] = P_PRESENT_POSITION_L;
	gbpParameter[1] = 2; //Read Length 2 byte L and H
	TxPacket(ServoNo,INST_READ,2);
	uint8_t bRxPacketLength = RxPacket(DEFAULT_RETURN_PACKET_SIZE+gbpParameter[1]);
	if(bRxPacketLength == DEFAULT_RETURN_PACKET_SIZE+gbpParameter[1]){
		if(gbpRxBuffer[4]==0){
			POS_LB=gbpRxBuffer[5];
			POS_HB=gbpRxBuffer[6];
		}else{
			// error
			return SERVO_NOT_CONNECTED;
		}
	}else{
		//error
		return SERVO_NOT_CONNECTED;
	}
	return (POS_HB<<8) + POS_LB;
}


void MotionInitialise(void){
	memset((void *)&MHD, 0, sizeof(MOTION_HEARTBEAT_DATA));
	MHD.CurrentPage = -1;
	uint8_t ServoNo;
	
	for( ServoNo=1;ServoNo < NUM_OF_SERVOS_ATTACHED; ServoNo++){
			ServoPos[ServoNo] = 0;
			ServoTrim[ServoNo] = 0;
			ServoPos[ServoNo] =  getCurrentServoPos(ServoNo);
			if(ServoPos[ServoNo] == SERVO_NOT_CONNECTED){
				char temp[30]; sprintf(temp,"E:Servo %i not connected\n\r",ServoNo); TxDString(temp);
			}
			/*
			// Get Lower Byte of Current Position
			uint16_t POS_LB=0,POS_HB=0;
			gbpParameter[0] = P_PRESENT_POSITION_L;
			gbpParameter[1] = 1; //Read Length
			TxPacket(ServoNo,INST_READ,2);
			uint8_t bRxPacketLength = RxPacket(DEFAULT_RETURN_PACKET_SIZE+gbpParameter[1]);
			if(bRxPacketLength == DEFAULT_RETURN_PACKET_SIZE+gbpParameter[1]){
				if(gbpRxBuffer[4]==0){
					POS_LB=gbpRxBuffer[5];
				}else{
					// error
					ServoPos[ServoNo] = SERVO_NOT_CONNECTED;
				}
			}else{
				//error
				ServoPos[ServoNo] = SERVO_NOT_CONNECTED;
			}
			
			// Get Higher Byte of Current Position
			gbpParameter[0] = P_PRESENT_POSITION_H;
			gbpParameter[1] = 1; //Read Length
			TxPacket(ServoNo,INST_READ,2);
			bRxPacketLength = RxPacket(DEFAULT_RETURN_PACKET_SIZE+gbpParameter[1]);
			if(bRxPacketLength == DEFAULT_RETURN_PACKET_SIZE+gbpParameter[1]){
				if(gbpRxBuffer[4]==0){
					POS_HB=gbpRxBuffer[5];
				}else{
					// error
					ServoPos[ServoNo] = SERVO_NOT_CONNECTED;
				}
			}else{
				//error
				ServoPos[ServoNo] = SERVO_NOT_CONNECTED;
			}
			
			// CODE 9999 means the servo is not attached
			if(ServoPos[ServoNo] != SERVO_NOT_CONNECTED){
				ServoPos[ServoNo] = (POS_HB<<8) + POS_LB;
				//char temp[50]; sprintf(temp," %i",ServoPos[ServoNo]); TxDString(temp);
			}else{
				char temp[50]; sprintf(temp,"E:Servo %i not connected\n\r",ServoNo); TxDString(temp);
			}
			*/
	}
}

void ContinueMotion_HeartBeat(void)
{
	uint8_t MOVING = FALSE;
	
	if(MHD.CurrentPage != -1){
		
		// Check if Servos are still in Motion
		/*
		gbpParameter[0] = P_MOVING;
		gbpParameter[1] = 1; //Read Length
		for( ServoNo=1;ServoNo < NUM_OF_SERVOS_ATTACHED; ServoNo++){	
			TxPacket(ServoNo,INST_READ,2);
			uint8_t bRxPacketLength = RxPacket(DEFAULT_RETURN_PACKET_SIZE+gbpParameter[1]);
			if(bRxPacketLength == DEFAULT_RETURN_PACKET_SIZE+gbpParameter[1]){
				if(gbpRxBuffer[4]==0){
					//TxDString("M:");TxD8Hex(gbpRxBuffer[5]);
					if(gbpRxBuffer[5]!=0){
						// Servo is still moving
						MOVING = TRUE;
					}
				}else{
					TxDString("Error:");//gbpRxBuffer[4]
				}
			}
		}
		*/
		//TxDString(1);
		
		if(MHD.MoveFinishTime>0){MHD.MoveFinishTime--;}
		
		if(MHD.MoveFinishTime==0){
			MOVING = FALSE;
			//TxDString("M");
		}else{
			MOVING = TRUE;
		}
		
		if(MOVING == FALSE){ 
			if(MHD.PosePause>0){MHD.PosePause--;}
			
		if(MHD.PosePause==0){
				//TxDString("+\n\r");
				if(ROBOTSTATUS!=STOP_POSE){
					if(MHD.CurrentPose+1<MHD.NoOfPoses){
						// next pose
						MHD.CurrentPose++;
						//TxDString("---Next Pose---\n\r");
						MHD.PosePause = doPose(MHD.CurrentPage,MHD.CurrentPose);
					}else{
						// finished
						//TxDString("Done Page\n\r");
						char txt[11]; txt[0]='\0';
						sprintf(txt,"[pg%02X]",MHD.CurrentPage);
						TxDString(txt);
						#ifdef ROBOT_DASH_MODE
						if(MHD.CurrentPage==FORWARD_STEP_COMPLETED_PAGE_NO){
							Forward_Dash_StepNo++;
						}
						if(MHD.CurrentPage==BACKWARD_STEP_COMPLETED_PAGE_NO){
							Backward_Dash_StepNo++;
						}
						#endif
						cli();
						uint8_t NextPage= pgm_read_byte_far  ((uint32_t)POSE_BASE_ADR + (uint32_t)POSE_PAGE_SIZE*(uint32_t)MHD.CurrentPage + (uint32_t)POSE_PAGE_NEXT_PAGE);
						sei();
						if(ROBOTSTATUS!=STOP_PAGE){
							//TxDString("NEXT PAGE\n\r");//NextPage;
							if(NextPage != 0){
								doPage(NextPage);
							}else{
								MHD.CurrentPage=-1;
							}
						}else{
							uint8_t ExitPage= pgm_read_byte_far  ((uint32_t)POSE_BASE_ADR + (uint32_t)POSE_PAGE_SIZE*(uint32_t)MHD.CurrentPage + (uint32_t)POSE_PAGE_EXIT_PAGE);
							if(ExitPage != 0){
								doPage(ExitPage);
							}else{
								MHD.CurrentPage=-1;
							}
						}
					}
				}
			}
		}
	}
}


uint16_t doPage(int page)
{

	if(page>127)
	   page=127;
	
	if(page!=-1)
	{
	   MHD.CurrentPage = page;
	   cli();
	   MHD.NoOfPoses= pgm_read_byte_far ((uint32_t)POSE_BASE_ADR +(uint32_t)POSE_PAGE_SIZE*(uint32_t)MHD.CurrentPage + (uint32_t)POSE_PAGE_NUM_OF_MOTIONS);
	   sei();
	   MHD.TotalTime=0;
	   MHD.CurrentPose = 0;		
	   MHD.PosePause = doPose(MHD.CurrentPage,MHD.CurrentPose);
	   return MHD.TotalTime;
	}
	
	return 0;
}


float PageSpeed2SpeedRate(uint16_t PageSpeed)
{
   if(PageSpeed == 10)
      return 3.0;
   
   if(PageSpeed == 11)
      return 2.9;
   
   return 30.0/PageSpeed;
}



void SendServoTargetPos(uint8_t ServoNo, int16_t ThetaTarget,uint16_t OmegaServo){
	uint8_t servo_value_LB = ThetaTarget & 0x00FF;
	uint8_t servo_value_HB = (ThetaTarget & 0xFF00)>>8;
	
	// send servo value
	gbpParameter[0] = P_GOAL_POSITION_L; 
	gbpParameter[1] = servo_value_LB; //Writing Data P_GOAL_POSITION_L
	gbpParameter[2] = servo_value_HB; //Writing Data P_GOAL_POSITION_H
	gbpParameter[3] = (OmegaServo & 0x00FF); //Writing Data P_GOAL_SPEED_L
	gbpParameter[4] = (OmegaServo & 0xFF00)>>8; //Writing Data P_GOAL_SPEED_H
	
	uint8_t TRANSM_OK = FALSE;
	uint8_t ATTEMPT=0;
	//do{
		TxPacket(ServoNo,INST_WRITE,5);
		uint8_t bRxPacketLength = RxPacket(DEFAULT_RETURN_PACKET_SIZE);
		ATTEMPT++;
		if(ATTEMPT>10){TRANSM_OK=TRUE;}// give up
		
		if(bRxPacketLength != DEFAULT_RETURN_PACKET_SIZE){
			/*char temp[30];
			sprintf(temp,"E:E1[%i]\n\r",ServoNo);TxDString(temp);*/

			// try to receive more bytes until finished to regain sync
			//RxPacket(DEFAULT_RETURN_PACKET_SIZE);
			// We should try to send the command again here
		}else if(gbpRxBuffer[4]!=0){
				char temp[30]; sprintf(temp,"E:E2[%i]\n\r",ServoNo);TxDString(temp);		
				// We should try to send the command again here
		}else{
			TRANSM_OK=TRUE;
		}
	//}while(TRANSM_OK!=TRUE);
}

void wait2finish(int ServoNo)
{
   //getCurrentServoPos(ServoNo);
}

void wait(float time)
{
   double code_time = 0.025 + 9; //0.000025 sg debe estar en ms
   
   double delay_time = time*1000.0 - code_time;
   
  /* char linea[10];
   sprintf(linea, "time = %d" , (int)(time*1000));
   TxDString(linea);
   
   for(;;){}*/
   
   _delay_ms(delay_time);
}


uint16_t doPose(int page,char pose)
{
   char temp[100];
   uint8_t PoseSpeed,ServoNo;
   uint16_t PosePause;
   uint32_t PoseAdr;
   float real_time;
	
	
   if(MHD.CurrentPage!=-1)
   {
      cli();
      uint8_t NoOfPosesinPage = pgm_read_byte_far ((uint32_t)POSE_BASE_ADR + (uint32_t)POSE_PAGE_SIZE*(uint32_t)page + (uint32_t)POSE_PAGE_NUM_OF_MOTIONS);
      sei();
	  
      if( pose + 1 > NoOfPosesinPage && page != 1)
	  {
         TxDString("E:Pose not on this page\n\r");
         return 0;
	  }
	  
	  cli();
	  PosePause = pgm_read_byte_far  ((uint32_t)POSE_BASE_ADR + (uint32_t)POSE_0_OFFSET + (uint32_t)POSE_PAGE_SIZE*(uint32_t)page + (uint32_t)POSE_SIZE*(uint32_t)pose +(uint32_t)POSE_PAUSE_ADR);
	  PoseAdr =  (uint32_t)POSE_BASE_ADR + (uint32_t)POSE_0_OFFSET + (uint32_t)POSE_PAGE_SIZE*(uint32_t)page + (uint32_t)POSE_SIZE*(uint32_t)pose;
				
	  PoseSpeed = pgm_read_byte_far  ((uint32_t)POSE_BASE_ADR + (uint32_t)POSE_0_OFFSET + (uint32_t)POSE_PAGE_SIZE*(uint32_t)page + (uint32_t)POSE_SIZE*(uint32_t)pose + (uint32_t)POSE_SPEED_ADR);
	  uint16_t PageSpeed = pgm_read_byte_far ((uint32_t)POSE_BASE_ADR +(uint32_t)POSE_PAGE_SIZE*(uint32_t)page + (uint32_t)POSE_PAGE_MOTION_SPEED);		
	  uint16_t PageFast = pgm_read_byte_far ((uint32_t)POSE_BASE_ADR +(uint32_t)POSE_PAGE_SIZE*(uint32_t)page + (uint32_t)POSE_PAGE_FAST_FLAG);
		
	  sei();
		
	  uint16_t ThetaMax = 0;

	  for(ServoNo=1;ServoNo < NUM_OF_SERVOS_ATTACHED; ServoNo++)
	  {
	     if(ServoPos[ServoNo] != SERVO_NOT_CONNECTED)
		 {
		    cli();
		    uint8_t servo_value_LB = pgm_read_byte_far  (PoseAdr + (uint32_t)ServoNo*2 );
		    uint16_t servo_value_HB = pgm_read_byte_far  (PoseAdr + (uint32_t)ServoNo*2 +1);
		    sei();
				
		    int16_t ThetaTarget = (servo_value_HB<<8) + servo_value_LB;
				
		    if(ThetaTarget < 0) 
			   ThetaTarget = 0;
		    if(ThetaTarget > 0x3ff) 
			   ThetaTarget=0x03ff;
	
		    int ThetaDiff = ThetaTarget - ServoPos[ServoNo];
            ThetaDiff = _abs_int(ThetaDiff);
				
            if(ThetaDiff > ThetaMax)
			   ThetaMax = ThetaDiff;

         }
      }

				
      float time = (PoseSpeed - 9) * 0.008 + 0.072;
      real_time = (float)time / (float)PageSpeed2SpeedRate(PageSpeed);  
		
      MHD.MoveFinishTime = real_time;

      float Omega = (float)((float)(ThetaMax/3.41) / (float)real_time);		
      uint16_t OmegaServoMax = (uint16_t)(Omega * 1023.0 / 684.0);

      OmegaServoMax = abs(OmegaServoMax);
				
      if(OmegaServoMax > 0x3ff)
	  {
	     OmegaServoMax = 200;
         TxDString("Motions: Velocidad Fuera de Rango Superior\r\n"); 
	  }
		
	  if(OmegaServoMax < 0.00)
	  {
	     OmegaServoMax = 200;
         TxDString("Motions: Velocidad Fuera de Rango Inferior\r\n"); 
	  }
		
	  PosePause = (PosePause * 32)/PageSpeed;	

	  for(ServoNo=1;ServoNo < NUM_OF_SERVOS_ATTACHED; ServoNo++)
	  {
		 if(ServoPos[ServoNo] != SERVO_NOT_CONNECTED)
		 {
		    cli();
			uint8_t servo_value_LB = pgm_read_byte_far  (PoseAdr + (uint32_t)ServoNo*2 );
			uint8_t servo_value_HB = pgm_read_byte_far  (PoseAdr + (uint32_t)ServoNo*2 +1);
			sei();
			if(servo_value_HB > 0x03 ) { servo_value_HB=0x03;}
			uint16_t HB = servo_value_HB;
			int16_t ThetaTarget = (HB<<8) + servo_value_LB;
				
			ThetaTarget = ThetaTarget + ServoTrim[ServoNo];
				
			wait2finish(ServoNo);
				
			if(ThetaTarget > 0x3ff){ThetaTarget = 0x3ff;}
			if(ThetaTarget < 0){ThetaTarget = 0;}

			int ThetaDiff = ThetaTarget - ServoPos[ServoNo];
			
			ThetaDiff = _abs_int(ThetaDiff);

			uint16_t OmegaServo = (uint16_t)((float)OmegaServoMax * ((float)ThetaDiff/(float)ThetaMax));
				
			if(OmegaServo>0x3ff){OmegaServo=0x3ff;}

            SendServoTargetPos(ServoNo,ThetaTarget,OmegaServo);

			ServoPos[ServoNo]= ThetaTarget ;
		 }
      }
		
   }
   else
      return 0;

   wait(real_time);
   
   return PosePause;
   
}