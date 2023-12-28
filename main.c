#include <stdio.h> 
#include <stdlib.h> 
#include <math.h> 
#include "simula.h" 
#define TIME 100000
struct coor{
  int x, y;
};

float prev_x_positions[TIME];
float prev_y_positions[TIME];
int positions_index = 0;
int charge_cycles = 0;

void to_home(){
  sensor_t rob = rmb_state();

  // Check if the battery is running low
  if (rmb_battery() < 300) {
    for (int i = positions_index - 1; i >= 0; i--) {
      float prev_x = prev_x_positions[i];
      float prev_y = prev_y_positions[i];

      // Check if the current position has been visited before
      for (int j = 0; j < i; j++) {
        if (prev_x_positions[j] == rob.x && prev_y_positions[j] == rob.y) {
          // If the current position has been visited before, continue from the last visited position
          i = j;
          prev_x = prev_x_positions[i];
          prev_y = prev_y_positions[i];
          break;
        }
      }

      float dist_x = prev_x - rob.x;
      float dist_y = prev_y - rob.y;

      // Calculate angle to previous position
      float angle_to_prev = atan2(dist_y, dist_x);

      // Calculate the difference between the current heading and the angle to the previous position
      float heading_difference = angle_to_prev - rob.head;

      // Turn to face the previous position
      rmb_turn(heading_difference);

      rmb_forward();

      if(rmb_ifr() > 0) {
        while(rmb_ifr() > 0) {
          rmb_clean();
        }
      }

      if (rmb_at_base()) {
        while(rmb_battery() < 1000) {
          rmb_load();
        }
        positions_index = 3;
        charge_cycles ++;
        break;
      }

      // Update the current state of the Roomba and recalculate the distance to the previous position
      rob = rmb_state();
    }
  }
}

void on_start(){
  struct coor base;
  sensor_t rob;
  float angle;
  rmb_awake(&base.x, &base.y);
  //random heading in a 180º arc
  rob = rmb_state();
  angle = rand()  / (float)RAND_MAX * M_PI - M_PI_2; 
  rmb_turn(rob.head + angle);
}

void cyclic_behav(struct coor coor, struct coor base){
  sensor_t rob = rmb_state();

  prev_x_positions[positions_index] = rob.x;
  prev_y_positions[positions_index] = rob.y;
  positions_index++;


  if (!rmb_bumper()) {
    rmb_forward();

      while(rmb_battery() < 300) {
          to_home();
      }

      if (rmb_ifr() > 0)
      {
        while(rmb_ifr() > 0) {
          rmb_clean();
        }

      if (charge_cycles <= 2) {
        rmb_turn(-rob.head);
        
        for(int k = 0; k < 4; k++) {
        for (int i = 0; i <= 4; i++) {
          for (int j = 0; j < k+1; j++){
            if(rmb_ifr() > 0) {
              while(rmb_ifr() > 0) {
                rmb_clean();
              }
            }
            rmb_forward();
          }
        rmb_turn(M_PI_2);
        }
      }
      }

      float angle = rand() / (float)RAND_MAX * 2 * M_PI;
      rmb_turn(angle);
      }

      if(rmb_at_base())
        while(rmb_battery() < 500)
          rmb_load();
  } 

  else {
    float angle = rand() / (float)RAND_MAX * 2 * M_PI;
    rmb_turn(angle);
  }
}

void on_stop(){
  visualize();
}


int main(){
  configure(on_start, cyclic_behav, on_stop, TIME);
  run();
  return 0;
}
