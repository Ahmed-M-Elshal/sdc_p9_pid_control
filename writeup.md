# Reflection of PID controls
Self-Driving Car Engineer Nanodegree Program

---

## Effects of P, I, D components 
Started with the following params: p,i,d [1, 1, 1]
Realized that was too large and the steering angle needs to be between [-1, 1]

I then changed them all to be 0.33. With that the car seemed to be flipping around right and left contantly. So clearly it was wrong. I looked back up on the lecture notes and start realizing that the integral part needs to be extremely small since it sums up the CTEs across all iterations. So then I set the params proportional to what we had in the lectures [0.15, 0.05, 0.8]

This made a huge improvement but still wasnt good enough. I started implementing the twiddle algorithm. After running the twiddle for a few iterations I realized the i component needed to be a fraction of what I had and so i decreased it further [0.25, 0.0015, 0.8]. This made sense to start with and the car was able to go in a straight line till it hit the first curve. 

The P controller keeps the car proportional to the refernce trajectory. A value around 0.2 gives it a low weight but enough so that the car doesnt overshoot too much. 

The D controller helps with the overshoot and brings the car in line with the reference trajectory. The weight of this parameter is the highest so that the car brings itself back on track with the refernce trajectory to compensate for the sharp turns.  

The I controller is the sum of all the CTEs and to compensate for the bias so this value should be miniscule

## Tuning Hyperparameters 

This was done with a mix of manual tuning and twiddle. I started out with manual tuning and then implemented a twiddle function. It took me forever to get the twiddle function to work. I was not aware that sending and receiving JSON messages was required to get the updated error for each iteration. Once I figure that out I had a somewhat working version of the twiddle algorithm. This is a modified version of what is described in the lectures. 

The idea is to find a set of parameters that would minimize the CTE. I initialized the desired set of params dp to the default values to start tuning. 
pid.dp[0] = pid.Kp;
pid.dp[1] = pid.Ki;
pid.dp[2] = pid.Kd;

After running for a bunch of iterations I noticed that the sumdp was settling once it reached a value of 19. So I set the tolerance to 19 with the initial params [0.2, 0.0015, 0.8]. The twiddle algorithm updates the params when it reaches the tolerance of 19. But with this the car was till swirling around quite a bit and would occassionally crash. 

So I reduced the i component of the controller and with start value of the params to [0.20,0.0012,0.8] I was able to get the car to complete the lap (barely!!!). It starts out well and starts moving around a lot towards the end! So I reduced the start value of the d component furhter [0.18, 0.0012, 0.8] and after around 50 iterations the twiddle updates the final params [0.164206, 0.00109471, 0.729806]. Seems like the car is more stable now! 

I realized the car was overshooting a bit so I changed the init params to [0.15, 0.0012, 0.8] and after running Twiddle is adjusted the params to [0.142228, 0.00113783, 0.758551]. I completed 2 laps with this!!!

Attached is a link to the video:
https://youtu.be/3jG3x5B3f2A
