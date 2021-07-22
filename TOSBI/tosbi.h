/* 
   TOSBI - Tiny Operative System Bioloid IUCTC
*/

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/signal.h>

#ifndef TOSBI
#define TOSBI

#define REGS 33 //32 registros de proposito general + SREG
#define STACK_TASK_SIZE 256 //Se permiten 4 procesos 
#define GLOBAL_STACK 200 //destinamos 200 bytes de los 4096 a pila

#define MAX_TASKS 4
#define RUN 1
#define SLEEP 0

#define enable_multitask TCCR0 = 7; TCNT0=134; TIMSK = 0x01; sei();
#define disable_multitask TIMSK = 0x00; TCCR0 = 0; TCNT0=134;

uint8_t stacks_tasks[MAX_TASKS];

struct task
{
   char *name;
   int STATE;
   uint16_t task_SP_init;
   uint16_t task_SP_end;
   uint8_t num_stack;
   uint8_t priority;
   uint8_t actual_priority;
   int (*exe)();
};

struct scheduler
{
   struct task tasks[MAX_TASKS];
   int count_tasks;
   int actual_task;
   int global_SP_init;
}SHD;

void findnext2run();
int new_task(char *nm, int (*funcion)(), int ST, uint8_t priority);
void init_TOSBI(int (*main)(), uint8_t priority);
void run_tasks();
int set_priority(char *nm, uint8_t priority);

#endif
