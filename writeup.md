# Reflection of PID controls
Self-Driving Car Engineer Nanodegree Program

---

## Effects of P, I, D components 
Started with the following params:
p -> 1
i -> 1
d -> 1
Realized that was too large and the steering angle needs to be between [-1, 1]

I then changed them all to be 0.33. With that the car seemed to be flipping around right and left contantly. So clearly it was wrong. I looked back up on the lecture notes and start realizing that the integral part needs to be extremely small since it sums up the CTEs across all iterations. So then I set the params proportional to what we had in the lectures
p -> 0.15
i -> 0.05
d -> 0.80

This made a huge improvement but still wasnt good enough. I started implementing the twiddle algorithm. After running the twiddle for a few iterations I realized the i component needed to be a fraction of what I had and so i decreased it further 
p -> 0.25
i -> 0.0015
d -> 0.80

This made sense to start with and the car was able to go in a straight line till it hit the first curve. 
[TODO] Look at lectures and descibe what P, I, D dooo


## Tuning Hyperparameters 

This was done with a mix of manual tuning and twiddle. I started out with manual tuning and then implemented a twiddle function. It took me forever to get the twiddle function to work. I was not aware that sending and receiving JSON messages was required to get the updated error for each iteration. Once I figure that out I had a somewhat working version of the twiddle algorithm. This is a modified version of what is described in the lectures. 

The idea is to find a set of parameters that would minimize the CTE. I initialized the desired set of params dp to the default values to start tuning. 
            if(pid.iter == twiddle_start) 
              // Initialize for best error              
              pid.best_err = pid.TotalError();
              pid.dp[0] = pid.Kp;
              pid.dp[1] = pid.Ki;
              pid.dp[2] = pid.Kd;

After running for a bunch of iterations I noticed that the sumdp was settling once it reached a value of 19. So I set the tolerance to 19 and uodate the params once it reaches that value. This update the params to 
p ->
i ->
d ->

With and setting the start value of the params to [,,,] I was able to get the car to complete the lap (barely!!!). It starts out well and starts moving around a lot towards the end!
