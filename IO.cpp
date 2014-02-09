/** These are the functions declared in IO.h. **/

#include "IO.h"

using namespace std;

void input_data(double& rate, double& expirate, double& div1, double& div2, double& lamda, double& yita, double& theta1, double& theta2, double& spot1, double& spot2, double& vol1, double& vol2, ifstream& infile)
{
	infile >> rate; // risk free interest rate
	infile >> expirate; // maturity days
	infile >> div1; // dividend of S&P 500
	infile >> div2; // dividend of Russell 2000
	infile >> lamda; // rate of reversion in Heston's mean reverting SDE
	infile >> yita; // volatility of Heston's mean reverting process
	infile >> theta1; // mean variance of S&P 500
	infile >> theta2; // mean variance of Russell 2000
	infile >> spot1; // initial price of S&P 500
	infile >> spot2; // initial price of Russell 2000
	infile >> vol1; // initial volatility of S&P 500
	infile >> vol2; // initial volatility of Russell 2000
}

void print_vector(ofstream& outfile, vector<double> v)
{
    vector<double>::iterator it = v.begin();
	while(it != v.end()) outfile << *it++ << ",";
	cout << endl;
	cout << endl;
}

void print_vector(ofstream& outfile, vector<double> u, vector<double> v)
{
	vector<double>::iterator it = v.begin();
    vector<double>::iterator ip = u.begin();
    while(it != v.end()) outfile << *ip++ << "," << *it++ << endl;
}

void print_vector(ofstream& outfile, vector<double> u, vector<double> v, vector<double> w)
{
	for(int i = 0; i < u.size(); i++)
	{
		outfile << u[i] << "," << v[i] << "," << w[i] << endl;
	}
}

void print_vector(ofstream& outfile, vector<double> u, vector<double> v, vector<vector<double> > x)
{ 
	int usize = u.size();
	int vsize = v.size();
	for(int i = 0; i < usize; i++)
	{ 
		for(int j = 0; j < vsize; j++)
		{ 
			outfile << u[i] << "," << v[j] << "," << x[i][j] << endl;
		}
	}
}



