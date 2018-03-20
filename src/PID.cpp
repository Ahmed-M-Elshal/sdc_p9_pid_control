#include "PID.h"
#include<iostream>
#include<math.h>
using namespace std;

/*
* TODO: Complete the PID class.
*/

PID::PID() {}

PID::~PID() {}

void PID::Init(double kp, double ki, double kd) {
  p_error = 0;
  d_error = 0;
  i_error = 0;
  Kp = kp;
  Ki = ki;
  Kd = kd;

  for(int i=0; i<3; i++) {
    p[i] = 0;
    dp[i] = 1;
  }

  iter = 0;
  ping_pong = 1;
  err = 0;
  best_err = 0;
  sumdp = 0;
}

void PID::UpdateError(double cte) {
  d_error = cte - p_error;
  p_error = cte;
  i_error += cte;
}

void PID::UpdateParams() {
  Kp = p[0];
  Ki = p[1];
  Kd = p[2];
  cout << "******** sumdp = " << sumdp << " params = " << p[0] << ", " << p[1] << ", " << p[2] << endl;
}

double PID::TotalError() {
  return p_error*p_error;
}
