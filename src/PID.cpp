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

double PID::SteeringAngle() {
  return -Kp*p_error - Kd*d_error - Ki*i_error;
}

void PID::Twiddle() {
  const int twiddle_start = 2;
  double tolerance = 20;
  iter++;
  if(iter >= twiddle_start) {
    if(iter == twiddle_start) {
      // Initialize for best error
      best_err = TotalError();
      dp[0] = Kp;
      dp[1] = Ki;
      dp[2] = Kd;
    } else {
      if(ping_pong) {
        ping_pong = 1;
        for(int i=0; i < 3; i++) {
          p[i] += dp[i];
        }
  
        err += TotalError()/iter;
        cout << "Err = " << err << " best_err = " << best_err << endl;
        if(err < best_err) {
          best_err = err;
          for(int i=0; i < 3; i++) {
            dp[i] *= 1.1;
          }
        } else {
          for(int i=0; i < 3; i++) {
            p[i] -= 2 * dp[i];
          }
          ping_pong = 0;
        }
      } else {
        ping_pong = 1;
        err += TotalError()/iter;
        cout << "Err = " << err << " best_err = " << best_err << endl;
        if(err < best_err) {
          best_err = err;
          for(int i=0; i < 3; i++) {
            dp[i] *= 1.1;
          }
        } else {
          for(int i=0; i < 3; i++) {
            p[i] += dp[i];
            dp[i] *= 0.9;
          }
        }
      }
      for(int i=0; i < 3; i++) {
        sumdp += dp[i];
      }
      // Update Kp, Ki, Kd with new values
      if(sumdp > tolerance) {
        UpdateParams();
        sumdp = 0;
      } else {
        cout <<"sumdp = " << sumdp << endl;
      }
    }
  }
}
