#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>

#include <ctime>

#include "OptionPay.h"
#include "MCEngine.h"
#include "IO.h"

#include "./tnt/tnt.h"
#include "./jama125/jama_cholesky.h"

const double DofY = 365.0; // days of one year is 365

using namespace std;
using namespace JAMA;
using namespace TNT;

// Payoff of each option
class PayforSum
{
	Payoff* OptPay;
	public:
	PayforSum(Payoff& optpay)
	{
		OptPay = optpay.clone();
	}
	double operator() (double spot)
	{
		return (*OptPay)(spot);
	}
};

// sum of all payoff
class PayofSum
{
	vector<PayforSum> pos;
	vector<double> amt;// the amout of option
	public:
	void insert(Payoff& optpay, double amount)
	{
		PayforSum pfs(optpay);
		pos.push_back(pfs);
		amt.push_back(amount);
	}
	double operator() (int i, double spot)
	{
		return pos[i](spot) * amt[i];
	}
	double size()//the size of PayofSum
	{
		return amt.size();
	}
};

int main()
{ 
	ifstream InputIdx("./InputIndex.txt");
	if(!InputIdx)
	{
		cout << "No input index file, please set your input index file" << endl;
	}
	
	double rf_rate; // risk free interest rate
	double d_expirate; // maturity days
	double div_sp; // dividend of S&P 500
	double div_ru; // dividend of Russell 2000
	double lamda; // rate of reversion in Heston's mean reverting SDE
	double yita; // volatility of Heston's mean reverting process
	double theta_sp; // mean variance of S&P 500
	double theta_ru; // mean variance of Russell 2000
	double ini_sp; // initial price of S&P 500
	double ini_ru; // initial price of Russell 2000
	double ivol_sp; // initial volatility of S&P 500
	double ivol_ru; // initial volatility of Russell 2000

	input_data(rf_rate, d_expirate, div_sp, div_ru, lamda, yita, theta_sp, theta_ru, ini_sp, ini_ru, ivol_sp, ivol_ru, InputIdx);
	int maturity_day = int(d_expirate); // maturity day (day)
	//cout << sizeof(maturity_day) << endl;
	d_expirate /= DofY; // maturity day (year)
	
	cout << "Risk free rate is " << rf_rate << endl;
	cout << "Maturity days is "<< d_expirate << endl;
	cout << "Dividend of S&P 500 is " << div_sp << ", and dividend of Russell 2000 is " << div_ru << endl;
	cout << "rate of reversion in Heston's mean reverting SDE is " << lamda << endl;
	cout << "volatility of Heston's mean reverting process is " << yita << endl;
	cout << "Mean variance of S&P 500 is " << theta_sp << ", and mean variance of Russell 2000 is " << theta_ru << endl;
	cout << "Initial price of S&P 500 is " << ini_sp << ", and mean variance of Russell 2000 is " << ini_ru << endl;
	cout << "Initial volatility of S&P 500 is " << ivol_sp << ", and initial volatility of Russell 2000 is " << ivol_ru << endl;

	InputIdx.close();

	double strike = 0.5 * (ini_sp + ini_ru); // strike price for digital option

	// Payoff dividendly
	VanillaPay PutOpt;
	PutOpt.setvalue(strike*0.95, 0);
	VanillaPay CallOpt;
	CallOpt.setvalue(strike*1.2, 1);
	DigitalPay DigiOpt;
	DigiOpt.setvalue(strike, 1);

	Milstein Indp_sp(ini_sp, rf_rate - div_sp, ivol_sp, d_expirate / DofY, lamda, theta_sp, yita, maturity_day); //S&P 500 price process
	Milstein Indp_ru(ini_ru, rf_rate - div_ru, ivol_ru, d_expirate / DofY, lamda, theta_ru, yita, maturity_day); // Rusell price process

	// simulator the stochastic process
	vector<Milstein> Indp;
	Indp.push_back(Indp_sp);
	Indp.push_back(Indp_ru);

	// payoff sumof
	PayofSum SumofPay;
	SumofPay.insert(PutOpt, -10.0/strike);
	SumofPay.insert(CallOpt, 10.0/strike);
	SumofPay.insert(DigiOpt, 2.0);

	int N = 10000; // number of steps for monte carlo
	double dis = exp(-rf_rate * d_expirate); // discount rate
	double bank_acct = 10.0;

	clock_t start, finish;

	start = clock();
	double Indprice = MonteCarloPricer_MS(N, SumofPay, Indp, dis);
	finish = clock();
	cout << "The duration of MontoCarlo Engine is " << ((float)(finish - start)) / CLOCKS_PER_SEC << " seconds."<< endl;
	cout << "The price of replicating options is " << Indprice << endl;
	Indprice += dis * bank_acct;
	cout << "The price of the security is " << Indprice << endl;
	

	// Correlation Brownian Motion
	Array2D<double> Corr(4, 4); // Correlation Matrix
	Array2D<double> Corr_L; // Lower Triangle Matrix

	// Construct the correlation matrix
	Corr[0][0] = 1.0;
	Corr[0][1] = 0.7;
	Corr[0][2] = -0.7;
	Corr[0][3] = -0.8;
	Corr[1][2] = -0.6;
	Corr[1][3] = -0.7;
	Corr[2][3] = 0.8;

	for(int i = 1; i < 4; i++)
	{
		for(int j = 0; j <= i; j++)
		{
			if(i == j)
			{
				Corr[i][j] = 1.0;
			}
			else
			{
				Corr[i][j] = Corr[j][i];
			}
		}
	}
	// Cholesky Factor
	Cholesky<double> chol(Corr);
	if(chol.is_spd())
	{
		Corr_L = chol.getL();
	}

	vector<double> OptPay_Corr(3);
	Milstein Corr_Price(ini_sp, ini_ru, rf_rate - div_sp, rf_rate - div_ru, ivol_sp, ivol_ru, d_expirate / DofY, lamda, theta_sp, theta_ru, yita, maturity_day);
	start = clock();
	double Corrprice = MonteCarloPricer_MS(N, SumofPay, Corr_Price, dis, Corr_L);
	finish = clock();
	cout << "The duration of MontoCarlo Engine is " << ((float)(finish - start)) / CLOCKS_PER_SEC << " seconds."<< endl;
	cout << "The price of replicating option is " << Corrprice << endl;
	Corrprice += dis * bank_acct;
	cout << "The price of the security is " << Corrprice << endl;
	
	
	return 0;
}

