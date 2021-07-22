/*

 __    __    __    ______    __________    ______
|  |  |  |  |  |  |   ___|  |___    ___|  |   ___|
|  |  |  |  |  |  |  |          |  |      |  |
|  |  |  |  |  |  |  |          |  |      |  |
|  |  |  |__|  |  |  |___       |  |      |  |___
|__|  |________|  |______|      |__|      |______|

													iuctc.es

*/


#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include "BLV012/motion.h"
#include "movimiento.h"


// ---------- extern functions from motion.c --------------
extern uint16_t doPose(int page,char pose);
extern uint16_t doPage(int page);

extern volatile unsigned int ROBOTSTATUS;

void Behaviour_HeartBeat(void)
{
	//MeasureGyro();
	static int tick=0;
	static int tick_tilt=0;
	tick++;
	tick_tilt++;
	
	if(tick==6){
		if((Balance)&&(MHD.CurrentPage==-1)){
			int CP[20];
			for(uint8_t ServoNo=13;ServoNo<17;ServoNo++){
				CP[ServoNo]=getCurrentServoPos(ServoNo);
			}
			for(uint8_t ServoNo=13;ServoNo<17;ServoNo++){
			//if((ServoNo==11)||(ServoNo==12)||(ServoNo==13)||(ServoNo==14)){
				int error = ServoPos[ServoNo] - CP[ServoNo];
				int ThetaTarget = ServoPos[ServoNo] + error;
					char txt[25];
					//if((abs(error)>0)||(abs(error)>0)){
					//	sprintf(txt,"he%i\r\n",error); TxDString(txt);
					//}
				SendServoTargetPos(ServoNo,ThetaTarget,400);	
			}
		}	
	}

	
	if(tick==6)
	{
		tick=0;
	}
	

}

/*
   bioloid - iuctc
   
   valor_retardo.-
   
   Indica la velocidad de las movimientos
   
*/

volatile int valor_retardo = 0;

/*
   bioloid - iuctc
   
   retardo.-
   
*/

void retardo(int valor)
{
   int i;
   int j;
   int k;
   int z;
   int x;
   
   int aux = 0;
   /*sprintf(linea , "valor: %d\n" , valor);
   TxDString(linea);*/
      
   for(i = 0; i < valor; i++)
   {
      TxDString("Retardo\n");
      aux++;
      aux--;

   }
}

/*
   bioloid - iuctc
   
   doPage_.-
   
   Función que ejecuta los poses de una página
*/

void doPage_(int page)
{
   int i;  
   
   ROBOTSTATUS = DO_PAGE; 
   
   doPage(page);
   
   for(i = 0; i < MHD.NoOfPoses - 1; i++)
   {
      retardo(valor_retardo); 
	  Behaviour_HeartBeat();
      MHD.CurrentPose++;
      MHD.PosePause = doPose(MHD.CurrentPage,MHD.CurrentPose);  
   }
}

void posicion_inicial()
{
   doPage_(1);   
}

void adelante()
{
   //doPage_(60);  //Podría ir?
   doPage_(63);
   doPage_(64);
   doPage_(65);
   doPage_(62);
}

void atras()
{
   //doPage_(69);  
   doPage_(70);
   doPage_(73);
   doPage_(74);
   doPage_(71);
   doPage_(72);

}

void derecha()
{
   doPage_(87);
   doPage_(90);
   doPage_(91);
   doPage_(92);
   doPage_(89);
   //doPage_(90); //Esta debe ir?
}

void izquierda()
{
   doPage_(96);
   doPage_(99);
   doPage_(100);
   doPage_(101);
   doPage_(98);
   //doPage_(99);  //Esta debe ir?

}


void adelante_bien()
{
   //doPage_(60);  //Podría ir?
   doPage_(63);
   doPage_(64);
}


void levantarse_boca_abajo()
{
   doPage_(11); 
}

void levantarse_boca_arriba()
{
   doPage_(13); 
}

void tirarse_boca_abajo()
{
   doPage_(12);
}

void tirarse_boca_arriba()
{
   doPage_(12);
}

