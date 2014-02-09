#ifndef MCENGINE_H
#define MCENGINE_H

#include <iostream>
#include <vector>
#include <algorithm>
#include "OptionPay.h"
#include "Simulator.h"




/* The entities Payoff and Simulator are parameters.  At compile time the compiler will first check the 
* validity of the actual values used and then translate this into machine code.
* Note that to implement a basic Monte Carlo pricer all you need are 
*    a payoff function 
*    a way to simulate the final prices
*    the number of simulations to perform
*    and (perhaps) the discount factor
*
* That's all we have here.
* **********************************/


//for corralated Milstein
template <typename T, typename Simulator>
double MonteCarloPricer_MS(int N, T p, Simulator s, double discount, TNT::Array2D<double> Corr)
{
	if(N <= 0){ std::cerr << "\n\t illegal number of simulations "<< std::endl; exit(1);}
	double sum = 0.0;
	double sumeach[3] = {0};
	for(int i = 0; i< N; ++i)
	{ 
		double z = s(Corr); 
		for(int j = 0; j < p.size(); j++)
		{
			double tempp = p(j,z);
			sumeach[j] += tempp;
			sum += tempp;
		}
	} 
	cout << "The price of short put option is " << discount*(sumeach[0]/N) << endl;
	cout << "The price of long digital call option is " << discount*(sumeach[1]/N) << endl;
	cout << "The price of long call option is " << discount*(sumeach[2]/N) << endl;
	return discount*(sum/N);
}

//for Milstein
template <typename T, typename Simulator> // T is sum of payoff, Simulator is the simulator
double MonteCarloPricer_MS(int N, T p, std::vector<Simulator> s, double discount)
{
	if(N <= 0){ std::cerr << "\n\t illegal number of simulations "<< std::endl; exit(1);}
	double sum = 0.0;
	double sumeach[3] = {0};
	int s_size = s.size();
	double weight = 1.0 / double(s_size);
	//std::cout << weight << std::endl;
	for(int i = 0; i< N; ++i)
	{ 
		double z = 0;
		for(int j = 0; j < s_size; j++)
		{
			z += weight * s[j]();
		}
		for(int j = 0; j < p.size(); j++)
		{
			double tempp = p(j,z);
			sumeach[j] += tempp;
			sum += tempp;
		}
	} 
	cout << "The price of short put option is " << discount*(sumeach[0]/N) << endl;
	cout << "The price of long digital call option is " << discount*(sumeach[1]/N) << endl;
	cout << "The price of long call option is " << discount*(sumeach[2]/N) << endl;

	return discount*(sum/N);
}


#endif
