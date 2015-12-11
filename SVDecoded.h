#pragma once
#include <vector>
#include <string>
using namespace std;

class SVDecoded {
public:
	vector<double> Ua;
	vector<double> Ub;
	vector<double> Uc;
	vector<double> Un;

	vector<double> Ia;
	vector<double> Ib;
	vector<double> Ic;
	vector<double> In;

	vector<double> QUa;
	vector<double> QUb;
	vector<double> QUc;
	vector<double> QUn;

	vector<double> QIa;
	vector<double> QIb;
	vector<double> QIc;
	vector<double> QIn;

	vector<int> counter;
	vector<string> svID;
};
