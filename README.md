# TOSBI

### Problema
En el caso de que salga el error de "Fallo de Pila" significa que la pila de un proceso invadi� a otra pila de otro proceso

**Soluci�n:** Existen dos soluciones: Aumentar el tama�o de la pila para cada proceso. Para ello ir al directorio TOSBI/tosbi.h y aumentar el valor de STACK_TASK_SIZE. Otra soluci�n ser�a no realizar tantas llamadas a funciones anidadas para que no se desborde la pila
									
### Prioridad
La prioridad y los retardos de la pila var�an en funci�n del n�mero de tareas de la que se disponga.

```
  - 1 task
      priority 1
      retardo  15

  - 2 task
      priority 2
      retardo  5

  - 3 task
      priority 3,4
      retardo  0
```
