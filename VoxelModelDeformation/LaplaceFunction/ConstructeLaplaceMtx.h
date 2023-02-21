#pragma once

#include <vector>

using namespace std;

class ConstructeLaplaceMtx
{
public:
	ConstructeLaplaceMtx();
	~ConstructeLaplaceMtx();
	void SetInputDatas(long int nnz, int* surf_idx, float** close_points, float** surf_points, int** nnz_coors_3D, int size[3]);

	void Update();
	void GetOutputDatas(vector<long int> &mtx_rows, vector<long int> &mtx_cols, vector<float>& mtx_val);
	void SaveMTX2MAT(const char* filename);

private:
	long int coor_in_array(int x, int y, int z, long int start, long int end);
	long int max(long int x1, long int x2);
	long int min(long int x1, long int x2);

private:
	long int nnz;
	int size[3];
	int* surf_idx;
	float** orig_pts;
	float** deformed_pts;
	int** coords_xyz;
	vector<long int> mtx_rows;
	vector<long int> mtx_cols;
	vector<float> mtx_val;
	float* phi_x;
	float* phi_y;
	float* phi_z;

};

