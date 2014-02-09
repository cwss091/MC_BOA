/* This Simulator cited from Prof. Demko's Simulator.h
 * I have changed the class Halton, HaltonGBM, EulerMarayama
 * This file generates random number and simulates stochastic process
 */

#ifndef __SIMULATOR_H_
#define __SIMULATOR_H_

#include <cmath>
#include <ctime>
#include <string>
#include "mt19937ar.h"
#include "BoxMuller.h"
#include "./tnt/tnt.h"
//#include "./jama125/jama_cholesky.h"

//using namespace JAMA;
//using namespace TNT;

class Simulator{
public:
  virtual double operator()(void)= 0; // argument gives step size
  virtual void Seed(unsigned long int) = 0;  // initializes RNG
  virtual void Seed() = 0;
  virtual void Skip(int) = 0; //  goes through several simulations;
};

class Mersenne:public Simulator{
  public:
    Mersenne(){Seed();}
    Mersenne(unsigned long s){Seed(s);}
    void Seed(unsigned long s){ init_genrand(s);}
    void Seed(){ init_genrand(0x13579bdf);}
    void Skip(int n){ for(int i =0; i< n; ++i) genrand_real3();}
    double operator()(){return genrand_real3();}
};

class Box_Muller:public Mersenne{
public:
    Box_Muller():Mersenne(){}
    Box_Muller(unsigned long s):Mersenne(s){}
    double operator()(){return genrand_std_normal();}
};

class Milstein:public Box_Muller
{
	double spot; // initial price
	double spot2; // the second initial price
	double drift; // drift
	double drift2;
	double sigma; // initial diffusion
	double sigma2;
	double dT; // delta t
	double lamda; 
	double theta;
	double theta2;
	double yita;
	int NumSteps;
	double det; // dt*drift
	double det2;
	// constant term for milstein methods
	double cons_yita;
	double cons_theta;
	double cons_theta2;
	double cons_lamda;
	public:
	Milstein(double sp, double d, double s, double dt, double lmd, double thet, double yta, int N):Box_Muller(), spot(sp), drift(d), sigma(s), dT(dt), lamda(lmd), theta(thet), yita(yta), NumSteps(N)
	{
		det = dT * drift;
		cons_yita = yita / 2.0 * sqrt(dT);
		cons_lamda = lamda * dT;
		cons_theta = lamda * theta * dT - yita * yita / 4.0 * dT;
	}

	Milstein(double sp, double ru, double d, double d2, double s, double s2, double dt, double lmd, double thet, double thet2, double yta, int N):Box_Muller(), spot(sp), spot2(ru), drift(d), drift2(d2), sigma(s), sigma2(s2), dT(dt), lamda(lmd), theta(thet), theta2(thet2), yita(yta), NumSteps(N)
	{
		det = dT * drift; // for S&P 500
		det2 = dT * drift2; // for Russel 2000
		cons_yita = yita / 2.0 * sqrt(dT);
		cons_lamda = lamda * dT;
		cons_theta = lamda * theta * dT - yita * yita / 4.0 * dT; // for S&P 500
		cons_theta2 = lamda * theta2 * dT - yita * yita / 4.0 * dT; // for Russel 2000
	}

	double operator ()()
	{
		double x = spot;
		double vol = sigma;
		for(int i = 0; i < NumSteps; i++)
		{
			double z1 = genrand_std_normal();
			x *= 1 + det + sqrt(vol*dT) * z1;
			double z2 = genrand_std_normal();
			double temp = sqrt(vol) + cons_yita * z2;
			vol = temp * temp - cons_lamda * vol + cons_theta;
		}

		return x;
	}
	
	double operator()(TNT::Array2D<double> C)
	{
		double x1 = spot; // S&P 500 initial spot price
		double x2 = spot2; // Russel 2000 initial spot price
		double vol1 = sigma; // S&P 500 initial variance
		double vol2 = sigma2; // Russel 2000 initial variance
		int dim = C.dim1();
		for(int i = 0; i < NumSteps; i++)
		{
			double *NormPhi = new double [dim];
			double *NormRand = new double [dim];

			for(int i = 0; i < dim; i++)
			{
				NormRand[i] = genrand_std_normal();
				NormPhi[i] = 0.0;
			}
			
			for(int i = 0; i < dim; i++)
			{
				for(int j = 0; j < dim; j++)
				{
					NormPhi[i] += C[i][j] * NormRand[j];
				}
			}
			x1 *= 1 + det +sqrt(vol1 * dT) * NormPhi[0];
			x2 *= 1 + det2 + sqrt(vol2 * dT) * NormPhi[1];
			double temp1 = sqrt(vol1) + cons_yita * NormPhi[2];
			vol1 = temp1 * temp1 - cons_lamda * vol1 + cons_theta;
			double temp2 = sqrt(vol2) + cons_yita * NormPhi[3];
			vol2 = temp2 * temp2 - cons_lamda * vol2 + cons_theta2;
			delete [] NormPhi;
			delete [] NormRand;
		}

		return (x1 + x2) / 2.0;
	}
};

#endif

