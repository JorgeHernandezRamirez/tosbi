#ifndef TASKS_INTERFACES
#define TASKS_INTERFACES

#include "../TOSBI/tosbi.h"

extern int valor_retardo;


struct tasks_interfaces
{
   char *task_movement;
   char *task_vision;
   char *task_strategy;
};

struct tasks_interfaces TI = 
{
   .task_movement = "main",
   .task_vision = "vision",
   .task_strategy = "strategy"
};

void adjust_values()
{
   if(SHD.count_tasks == 1)
   {
      set_priority(TI.task_movement ,1);
	  valor_retardo = 18;
	  return;
   }
   
   if(SHD.count_tasks == 2)
   {
      set_priority(TI.task_movement ,2);
	  valor_retardo = 2;
	  return;
   }
   
   if(SHD.count_tasks == 3)
   {
      set_priority(TI.task_movement ,2);
	  valor_retardo = 3;
	  return;
   }
   
   set_priority(TI.task_movement ,5);
   valor_retardo = 0;

}

#endif