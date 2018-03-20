#include <uWS/uWS.h>
#include <iostream>
#include "json.hpp"
#include "PID.h"
#include <math.h>

// for convenience
using json = nlohmann::json;

// For converting back and forth between radians and degrees.
constexpr double pi() { return M_PI; }
double deg2rad(double x) { return x * pi() / 180; }
double rad2deg(double x) { return x * 180 / pi(); }

// Checks if the SocketIO event has JSON data.
// If there is data the JSON object in string format will be returned,
// else the empty string "" will be returned.
std::string hasData(std::string s) {
  auto found_null = s.find("null");
  auto b1 = s.find_first_of("[");
  auto b2 = s.find_last_of("]");
  if (found_null != std::string::npos) {
    return "";
  }
  else if (b1 != std::string::npos && b2 != std::string::npos) {
    return s.substr(b1, b2 - b1 + 1);
  }
  return "";
}

int main()
{
  uWS::Hub h;

  PID pid;
  pid.Init(0.20,0.0012,0.8);
  //pid.Init(0.25,0.0012,0.8);
	//pid.Init(0.208724, 0.00166979, 0.667917);
  //pid.Init(0, 0, 0);

  h.onMessage([&pid](uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length, uWS::OpCode opCode) {
    // "42" at the start of the message means there's a websocket message event.
    // The 4 signifies a websocket message
    // The 2 signifies a websocket event
    if (length && length > 2 && data[0] == '4' && data[1] == '2')
    {
      auto s = hasData(std::string(data).substr(0, length));
      if (s != "") {
        auto j = json::parse(s);
        std::string event = j[0].get<std::string>();
        if (event == "telemetry") {
          // j[1] is the data JSON object
          double cte = std::stod(j[1]["cte"].get<std::string>());
          double speed = std::stod(j[1]["speed"].get<std::string>());
          double angle = std::stod(j[1]["steering_angle"].get<std::string>());
          double steer_value;
					const int twiddle_start = 2;
					double tolerance = 21;
          /*
          * Calcuate steering value here, remember the steering value is
          * [-1, 1].
          * NOTE: Feel free to play around with the throttle and speed. Maybe use
          * another PID controller to control the speed!
          */

          pid.iter++;
					if(pid.iter >= twiddle_start) {
        		if(pid.iter == twiddle_start) {
							// Initialize for best error
        		  pid.best_err = pid.TotalError();
							pid.dp[0] = pid.Kp;
							pid.dp[1] = pid.Ki;
							pid.dp[2] = pid.Kd;
        		} else {
        		  if(pid.ping_pong) {
								pid.ping_pong = 1;
        		  	for(int i=0; i < 3; i++) {
        		  	  pid.p[i] += pid.dp[i];
        		  	}

        				pid.err += pid.TotalError()/pid.iter;
        		  	std::cout << "Err = " << pid.err << " best_err = " << pid.best_err << std::endl;
        				if(pid.err < pid.best_err) {
        				  pid.best_err = pid.err;
        				  for(int i=0; i < 3; i++) {
        				    pid.dp[i] *= 1.1;
        				  }
        				} else {
        				  for(int i=0; i < 3; i++) {
        				  	pid.p[i] -= 2 * pid.dp[i];
									}
									pid.ping_pong = 0;
								}
							} else {
								pid.ping_pong = 1;
        				pid.err += pid.TotalError()/pid.iter;
        		  	std::cout << "Err = " << pid.err << " best_err = " << pid.best_err << std::endl;
        				if(pid.err < pid.best_err) {
        				  pid.best_err = pid.err;
        				  for(int i=0; i < 3; i++) {
        				    pid.dp[i] *= 1.1;
        				  }
								} else {
        				  for(int i=0; i < 3; i++) {
        				    pid.p[i] += pid.dp[i];
        				    pid.dp[i] *= 0.9;
									}
								}
							}
        			for(int i=0; i < 3; i++) {
								pid.sumdp += pid.dp[i];
							}
							// Update Kp, Ki, Kd with new values
							if(pid.sumdp > tolerance) { 
								pid.UpdateParams();
								pid.sumdp = 0;
							} else {
								std::cout <<"sumdp = " << pid.sumdp << std::endl;
							}
						}
					}
						
          pid.UpdateError(cte);
          steer_value = -pid.Kp*pid.p_error - pid.Kd*pid.d_error - pid.Ki*pid.i_error;
          // DEBUG
          std::cout << "iter " << pid.iter << " CTE: " << cte << " Steering Value: " << steer_value << std::endl;

          json msgJson;
          msgJson["steering_angle"] = steer_value;
          msgJson["throttle"] = 0.3;
          auto msg = "42[\"steer\"," + msgJson.dump() + "]";
          //std::cout << msg << std::endl;
          ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
        }
      } else {
        // Manual driving
        std::string msg = "42[\"manual\",{}]";
        ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
      }
    }
  });

  // We don't need this since we're not using HTTP but if it's removed the program
  // doesn't compile :-(
  h.onHttpRequest([](uWS::HttpResponse *res, uWS::HttpRequest req, char *data, size_t, size_t) {
    const std::string s = "<h1>Hello world!</h1>";
    if (req.getUrl().valueLength == 1)
    {
      res->end(s.data(), s.length());
    }
    else
    {
      // i guess this should be done more gracefully?
      res->end(nullptr, 0);
    }
  });

  h.onConnection([&h](uWS::WebSocket<uWS::SERVER> ws, uWS::HttpRequest req) {
    std::cout << "Connected!!!" << std::endl;
  });

  h.onDisconnection([&h](uWS::WebSocket<uWS::SERVER> ws, int code, char *message, size_t length) {
    ws.close();
    std::cout << "Disconnected" << std::endl;
  });

  int port = 4567;
  if (h.listen(port))
  {
    std::cout << "Listening to port " << port << std::endl;
  }
  else
  {
    std::cerr << "Failed to listen to port" << std::endl;
    return -1;
  }
  h.run();
}
