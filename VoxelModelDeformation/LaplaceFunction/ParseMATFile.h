#pragma once
#include <vector>

using namespace std;
class ParseMATFile
{
public:
	ParseMATFile();
	~ParseMATFile();

	int* GetMATData_Int(const char* filename, const char* filed, long int& Len, long int& dim);
	double* GetMATData_Double(const char* filename, const char* filed, long int& Len, long int& dim);

	void SaveMTXtoMAT(const char* filename, const char* filed1, vector<long int> mtx_rows,
		const char* filed2, vector<long int> mtx_cols,
		const char* filed3, vector<float> mtx_dats,
		const char* filed4, long int * mtx_size,
		const char* filedx, float* phix,
		const char* filedy, float* phiy,
		const char* filedz, float* phiz);

};

