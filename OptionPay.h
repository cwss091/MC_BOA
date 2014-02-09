#ifndef _OPTIONPAY_H__
#define _OPTIONPAY_H__

/* Payoff of the option */

#include <iostream>
#include <vector>
//#include "BinomialTree.h"

class Payoff
{ 
	protected:
		double strike; //strike price
		bool call; // if call, be true
		//double S0; //
	public:
		//Payoff(double stike_, double call_):strike(strike_), call(call_){}
		//void setvalue(double _strike, bool _call, double _S0); // setvalue
		void setvalue(double _strike, bool _call);
		virtual double operator() (double spot) = 0;
		virtual Payoff* clone() = 0;
		~Payoff(){}
};

// Payoff of Vanilla Option
class VanillaPay: public Payoff
{ 
	public:
		double operator() (double spot);
		Payoff* clone();
};

// Payoff of digital option
class DigitalPay: public Payoff
{
	public:
		double operator() (double spot);
		Payoff* clone();
};

#endif

