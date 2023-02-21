#include <stdio.h>

#include "ParseMATFile.h"
#include "ConstructeLaplaceMtx.h"


int** malloc_memory_2I(long int nx, long int ny);
float** malloc_memory_2F(long int nx, long int ny);
int** free_memory_2I(int** tab, long int nx, long int ny);
float** free_memory_2F(float** tab, long int nx, long int ny);

int main()
{
	printf("Hello world ...");
	ParseMATFile matio;
	ConstructeLaplaceMtx laplace;
	const char* filename = "F:\\UniformMF\\Human_models\\Leg_refinement\\Surf_idx_body_2mm.mat";
	const char* coo_mtx_filename = "F:\\UniformMF\\Human_models\\Leg_refinement\\coo_mtx_body_2mm.mat";
	long int nnz, dim;
	nnz = 0; dim = 0;
	int** nnz_coors_3D = NULL;
	int* surf_idx = NULL;
	float** close_points = NULL;
	float** deformed_points = NULL;
	float** surf_points = NULL;
	int size[3] = { 0 };

	vector<long int> mtx_rows;
	vector<long int> mtx_cols;
	vector<float> mtx_val;

	printf("Reading ...\n");
	int* val = matio.GetMATData_Int(filename, "size", nnz, dim);
	size[0] = (int)val[0];
	size[1] = (int)val[1];
	size[2] = (int)val[2];

	double *data = matio.GetMATData_Double(filename, "surf_idx", nnz, dim);
	surf_idx = new int[nnz];
	for (long int i = 0; i < nnz; i++)
		surf_idx[i] = (int)data[i];

	double* surf_pts = matio.GetMATData_Double(filename, "surf_points", nnz, dim);
	double* close_pts = matio.GetMATData_Double(filename, "close_points", nnz, dim);
	double* deformed_pts = matio.GetMATData_Double(filename, "deform_points", nnz, dim);
	double* coords = matio.GetMATData_Double(filename, "nnz_coors_3D", nnz, dim);

	surf_points = malloc_memory_2F(nnz, dim);
	close_points = malloc_memory_2F(nnz, dim);
	deformed_points = malloc_memory_2F(nnz, dim);
	nnz_coors_3D = malloc_memory_2I(nnz, dim);
	for (long int i = 0; i < dim; i++)
	{
		for (long int j = 0; j < nnz; j++)
		{
			long int idx = i * nnz + j;
			surf_points[j][i] = (float)surf_pts[idx];
			//printf("%d  ", (int)surf_points[j][i]);
			close_points[j][i] = (float)close_pts[idx];
			deformed_points[j][i] = (float)deformed_pts[idx];
			nnz_coors_3D[j][i] = (int)coords[idx];
			//printf("%d\n", (int)nnz_coors_3D[j][i]);
		}
	}

	printf("%d %d %d\n", (int)nnz_coors_3D[0][0], (int)nnz_coors_3D[0][1], (int)nnz_coors_3D[0][2]);
	printf("%d %d %d\n", (int)surf_points[0][0], (int)surf_points[0][1], (int)surf_points[0][2]);
	printf("%d %d %d\n", (int)close_points[0][0], (int)close_points[0][1], (int)close_points[0][2]);
	printf("Constructing COO mtx ...\n");
	laplace.SetInputDatas(nnz, surf_idx, close_points, deformed_points, nnz_coors_3D, size);
	laplace.Update();
	laplace.SaveMTX2MAT(coo_mtx_filename);
	laplace.GetOutputDatas(mtx_rows, mtx_cols, mtx_val);


	delete[]surf_idx;
	nnz_coors_3D = free_memory_2I(nnz_coors_3D, nnz, dim);
	surf_points = free_memory_2F(surf_points, nnz, dim);
	close_points = free_memory_2F(close_points, nnz, dim);
	deformed_points = free_memory_2F(deformed_points, nnz, dim);

	printf("Constructing COO mtx Finished\n");
	return 0;
}


int** malloc_memory_2I(long int nx, long int ny)
{
	int** tab = (int**)malloc(nx * sizeof(int*));
	for (long int i = 0; i < nx; i++)
	{
		tab[i] = (int*)malloc(ny * sizeof(int));
	}
	return tab;
}


float** malloc_memory_2F(long int nx, long int ny)
{
	float** tab = (float**)malloc(nx * sizeof(float*));
	for (long int i = 0; i < nx; i++)
	{
		tab[i] = (float*)malloc(ny * sizeof(float));
	}
	return tab;
}


int** free_memory_2I(int** tab, long int nx, long int ny)
{
	for (long int i = 0; i < nx; i++)
		free(tab[i]);
	free(tab);
	return NULL;
}

float** free_memory_2F(float** tab, long int nx, long int ny)
{
	for (long int i = 0; i < nx; i++)
		free(tab[i]);
	free(tab);
	return NULL;
}


