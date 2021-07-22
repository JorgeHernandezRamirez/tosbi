/* 
   TOSBI - Tiny Operative System Bioloid IUCTC
*/

#include "tosbi.h"
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
       

/*
   TOSBI

   SIGNAL(SIG_OVERFLOW)

   Función en donde se rocogen los eventos del timer   
*/

__attribute__((signal, naked))
SIGNAL(SIG_OVERFLOW0)
{
   disable_multitask;
   
   //Salvamos registros
   asm volatile (
                 "push r0 \n\t"
                 "in r0, __SREG__ \n\t"
                 "push r0 \n\t"
                 "push r1 \n\t"
                 "push r2 \n\t"
                 "push r3 \n\t"
                 "push r4 \n\t"
                 "push r5 \n\t"
				 "push r6 \n\t"
                 "push r7 \n\t"
                 "push r8 \n\t"
                 "push r9 \n\t"
                 "push r10 \n\t"
				 "push r11 \n\t"
				 "push r12 \n\t"
                 "push r13 \n\t"
                 "push r14 \n\t"
                 "push r15 \n\t"
                 "push r16 \n\t"
                 "push r17 \n\t"
                 "push r18 \n\t"
				 "push r19 \n\t"
                 "push r20 \n\t"
                 "push r21 \n\t"
				 "push r22 \n\t"
				 "push r23 \n\t"
                 "push r24 \n\t"
                 "push r25 \n\t"
                 "push r26 \n\t"
                 "push r27 \n\t"
                 "push r28 \n\t"
                 "push r29 \n\t"
                 "push r30 \n\t"
                 "push r31 \n\t");
    
   /*Seguridad de la pila*/
   if((SPH << 8) + SPL <= SHD.tasks[SHD.actual_task].task_SP_init - STACK_TASK_SIZE)
   {
      TxDString("Fallo de Pila\r\n");
	  for(;;){}
   }
		   
   SHD.tasks[SHD.actual_task].task_SP_end = (SPH << 8) + SPL;

   //Se busca el siguiente a ejecutarse
   findnext2run();	
}

/*
   TOSBI

   remove_actual_task

   Función que elimina la tarea actual   
*/
void remove_actual_task()
{
   int i = SHD.actual_task;
   
   //liberamos la pila
   stacks_tasks[SHD.tasks[i].num_stack] = 0;
   
   while(i < SHD.count_tasks - 1)
   {
      SHD.tasks[i].STATE = SHD.tasks[i+1].STATE;
	  SHD.tasks[i].task_SP_init = SHD.tasks[i+1].task_SP_init;
	  SHD.tasks[i].task_SP_end = SHD.tasks[i+1].task_SP_end;
	  SHD.tasks[i].exe = SHD.tasks[i+1].exe;
	  
	  free(SHD.tasks[i].name);
	  SHD.tasks[i].name = (char *)malloc(strlen(SHD.tasks[i+1].name)*sizeof(char)); 
	  strcpy(SHD.tasks[i].name, SHD.tasks[i+1].name);
	  free(SHD.tasks[i+1].name);
	  
	  i++;
   }
   
   SHD.count_tasks--;

   findnext2run();
}


void update_actual_task()
{
   SHD.tasks[SHD.actual_task].actual_priority--;
   
   if(SHD.tasks[SHD.actual_task].actual_priority <= 0)
   {
      SHD.tasks[SHD.actual_task].actual_priority = SHD.tasks[SHD.actual_task].priority;
	  
      SHD.actual_task++;

      if(SHD.actual_task >= SHD.count_tasks)
         SHD.actual_task = 0;
   }
}

/*
   TOSBI

   findnext2run

   Función que encentra la siguiente tarea a ejecutarse   
*/

void findnext2run()
{
   
   update_actual_task();

   if(SHD.tasks[SHD.actual_task].STATE == SLEEP)
   {
      SHD.tasks[SHD.actual_task].STATE = RUN;

      /* Si el proceso va a entrar en ejecutación le asignamos un espacio*/
	  SPL = SHD.tasks[SHD.actual_task].task_SP_init;
	  SPH = SHD.tasks[SHD.actual_task].task_SP_init >> 8;
  
      enable_multitask;
      SHD.tasks[SHD.actual_task].exe();

      disable_multitask;
      remove_actual_task();
   }
   else
   {

      SPL = SHD.tasks[SHD.actual_task].task_SP_end;
      SPH = SHD.tasks[SHD.actual_task].task_SP_end >> 8;

	  asm volatile(
                   "pop r31 \n\t"
                   "pop r30 \n\t"
                   "pop r29 \n\t"
                   "pop r28 \n\t"
                   "pop r27 \n\t"
                   "pop r26 \n\t"
                   "pop r25 \n\t"
				   "pop r24 \n\t"
                   "pop r23 \n\t"
                   "pop r22 \n\t"
                   "pop r21 \n\t"
                   "pop r20 \n\t"
				   "pop r19 \n\t"
				   "pop r18 \n\t"
                   "pop r17 \n\t"
                   "pop r16 \n\t"
                   "pop r15 \n\t"
                   "pop r14 \n\t"
                   "pop r13 \n\t"
                   "pop r12 \n\t"
				   "pop r11 \n\t"
                   "pop r10 \n\t"
                   "pop r9 \n\t"
				   "pop r8 \n\t"
				   "pop r7 \n\t"
                   "pop r6 \n\t"
                   "pop r5 \n\t"
                   "pop r4 \n\t"
                   "pop r3 \n\t"
                   "pop r2 \n\t"
                   "pop r1 \n\t"
                   "pop r0 \n\t"
				   "out __SREG__ ,r0 \n\t"
                   "pop r0 \n\t"); //R1 siempre a 0? es del save_status

      //Porque al restaurar SREG puede que no tenga las interrupciones globales activadas
      enable_multitask;

	  asm volatile("ret"); 
	  
	  
   }
}

/*
   TOSBI

   set_priority

   Función que asigna una prioridad a una tarea  
*/

int set_priority(char *nm, uint8_t priority)
{
   int i = 0;

   while(i < SHD.count_tasks && strcmp(SHD.tasks[i].name, nm))	  
      i++;

   if(i < SHD.count_tasks)
   {
      SHD.tasks[i].priority = priority;
	  return 1;
   }
   else
      return 0;
}


/*
   TOSBI

   give_num_task

   Función que encuentra una pila no usada   
*/

uint8_t give_num_stack()
{
   int i = 0;
   
   while(i < MAX_TASKS && stacks_tasks[i] != 0)
      i++;
	  
   if(i < MAX_TASKS)
   {
      stacks_tasks[i] = 1;
      return i;
   }
   else
      return -1;
}

/*
   TOSBI

   new_task

   Función que registra una nueva tarea en TOSBI   
*/

int new_task(char *nm, int (*funcion)(), int ST, uint8_t priority)
{
   if(SHD.count_tasks >= MAX_TASKS)
      return 0;
   else
   {
      SHD.tasks[SHD.count_tasks].STATE = ST;
	  
	  SHD.tasks[SHD.count_tasks].priority = priority;
	  SHD.tasks[SHD.count_tasks].actual_priority = priority;
	  
      SHD.tasks[SHD.count_tasks].name = (char *)malloc((strlen(nm)+1)*sizeof(char));  
      strcpy(SHD.tasks[SHD.count_tasks].name, nm);
	  
      SHD.tasks[SHD.count_tasks].exe = funcion;
	  
	  uint8_t num_stack = give_num_stack();
	  SHD.tasks[SHD.count_tasks].num_stack = num_stack;
	  SHD.tasks[SHD.count_tasks].task_SP_init = SHD.global_SP_init - num_stack*STACK_TASK_SIZE;

      SHD.count_tasks++;

      return 1;
   }
}

/*
   TOSBI

   init_TOSBI

   Función que inicializa TOSBI   
*/

void init_TOSBI(int (*main)(), uint8_t priority)
{
   uint16_t aux = (SPH << 8) + SPL + 2;

   SHD.global_SP_init = aux;
   SHD.count_tasks = 0;
   SHD.actual_task = 0;
   
   /* Todas las posibles pilas están ocupadas */
   for(int i = 0; i < MAX_TASKS; i++)
      stacks_tasks[i] = 0;

   new_task("main", main, RUN, priority);
}

/*
   TOSBI

   run_tasks

   Función que inicializa el timer y las interrupciones
*/
void run_tasks()
{
   enable_multitask;
}
