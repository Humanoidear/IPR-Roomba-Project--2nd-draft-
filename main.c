#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "simula.h"

struct coor{
  int x, y;
};

int rand_angle(int angle, int cases){  // Genera un angulo random en 7 posiciones posibles 0-360
  angle = rand()%cases;                // ESTA IMPLEMENTACION FUERZA A TENER EL DOCK EN LA PARED IZQUIERDA
  switch (angle)
  {
  case 0:
    angle = 0;
    break;
  
  case 1:
    angle = 45;
    break;

  case 2:
    angle = 90;
    break;

  case 3:
    angle = 135;
    break;

  case 4:
    angle = 180;
    break;

  case 5:
    angle = 225;
    break;

  case 6: 
    angle = 270;
    break;
  
  case 7: 
    angle = 315;
    break;

  default:
    break;
  }
  return angle;
}


void on_start(){
  struct coor base;
  sensor_t rob;
  rmb_awake(&base.x, &base.y);
  int angle = rand_angle(&angle, 5); // Gira cabeza de manera aleatoria en el 1er y 4o cuadrante, step = pi/2
  rmb_turn(rob.head+angle);
  rmb_forward();
}


void cyclic_behav(){  // Gira cabeza, avanza, detecta obstaculos_gira cabeza if TRUE, detecta suciedad_limpieza if TRUE, carga if DOCK=TRUE
  int angle = rand_angle(&angle, 8); // Gira cabeza de manera aleatoria entre el 1er y 4o cuadrante, step = pi/2
  rmb_turn(angle);
  
  //da un paso en la dirección actual
  rmb_forward();

  //detección de obstáculos
  if(rmb_bumper()){
    //gira un ángulo aleatorio
    angle = rand_angle(&angle, 8);
    rmb_turn(angle);
  }

  //deteccion de suciedad
  while(rmb_ifr() > 0)
    rmb_clean();

  //carga si se encuentra en la base
  if(rmb_at_base())
    while(rmb_battery() < 250)
      rmb_load();
}


void on_stop(){
  visualize();
}


int main(){
  configure(on_start, cyclic_behav, on_stop, 2000);
  run();
  return 0;
}
