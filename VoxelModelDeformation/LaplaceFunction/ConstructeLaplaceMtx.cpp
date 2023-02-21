#include "ConstructeLaplaceMtx.h"
#include "ParseMATFile.h"


ConstructeLaplaceMtx::ConstructeLaplaceMtx()
{

}

ConstructeLaplaceMtx::~ConstructeLaplaceMtx()
{

}


void ConstructeLaplaceMtx::SetInputDatas(long int nnz, int* surf_idx, float** orig_points, float** deformed_points, int** nnz_coors_3D, int size[3])
{
	this->nnz = nnz;
	this->surf_idx = surf_idx;
	this->orig_pts = orig_points;
	this->deformed_pts = deformed_points;
	this->coords_xyz = nnz_coors_3D;
	this->size[0] = size[0];
	this->size[1] = size[1];
	this->size[2] = size[2];
}

void ConstructeLaplaceMtx::GetOutputDatas(vector<long int>& mtx_rows, vector<long int>& mtx_cols, vector<float>& mtx_val)
{
	mtx_rows = this->mtx_rows;
	mtx_cols = this->mtx_cols;
	mtx_val = this->mtx_val;
}

void ConstructeLaplaceMtx::Update()
{
	this->phi_x = new float[this->nnz];
	this->phi_y = new float[this->nnz];
	this->phi_z = new float[this->nnz];
	long int LM = this->size[0] * this->size[1];
	for (long int i = 0; i < this->nnz; i++)
	{
		if (i % 1000 == 0)
		{
			printf("Processing %ld / %ld, %4.2f %%\n", i, this->nnz, (float)i / (float)this->nnz*100.0);
		}
		if (this->surf_idx[i] == 1)
		{
			this->phi_x[i] = this->orig_pts[i][0] - this->deformed_pts[i][0];
			this->phi_y[i] = this->orig_pts[i][1] - this->deformed_pts[i][1];
			this->phi_z[i] = this->orig_pts[i][2] - this->deformed_pts[i][2];
		}
		int x0 = this->coords_xyz[i][0];
		int y0 = this->coords_xyz[i][1];
		int z0 = this->coords_xyz[i][2];

		//int related_p = 0;
		int related_p[6] = { 0 };
		long int cols[6] = { 0 };
		if (this->surf_idx[i] == 0)
		{
			long int idx_z = coor_in_array(x0, y0, z0 - 1, this->max(0, i - 1 * LM-10), i);
			if (idx_z >= 0)
			{
				cols[0] = idx_z;
				//this->mtx_cols.push_back(idx_z);
				//this->mtx_rows.push_back(i);
				//this->mtx_val.push_back(1);
				related_p[0] = 1;
			}
			long int idx_y = coor_in_array(x0, y0 - 1, z0, this->max(0, i - this->size[0]-10), i);
			if (idx_y >= 0)
			{
				cols[1] = idx_y;
				//this->mtx_cols.push_back(idx_y);
				//this->mtx_rows.push_back(i);
				//this->mtx_val.push_back(1);
				related_p[1] = 1;
			}
			long int idx_x = coor_in_array(x0 - 1, y0, z0, this->max(0, i - 10), i);
			if (idx_x >= 0)
			{
				cols[2] = idx_x;
				//this->mtx_cols.push_back(idx_x);
				//this->mtx_rows.push_back(i);
				//this->mtx_val.push_back(1);
				related_p[2] = 1;
			}
			idx_x = coor_in_array(x0 + 1, y0, z0, i, this->min(i + 10, nnz));
			if (idx_x >= 0)
			{
				cols[3] = idx_x;
				//this->mtx_cols.push_back(idx_x);
				//this->mtx_rows.push_back(i);
				//this->mtx_val.push_back(1);
				related_p[3] = 1;
			}
			idx_y = coor_in_array(x0, y0 + 1, z0, i, this->min(i + this->size[0]+10, nnz));
			if (idx_y >= 0)
			{
				cols[4] = idx_y;
				//this->mtx_cols.push_back(idx_y);
				//this->mtx_rows.push_back(i);
				//this->mtx_val.push_back(1);
				related_p[4] = 1;
			}
			idx_z = coor_in_array(x0, y0, z0 + 1, i, this->min(i + LM+10, nnz));
			if (idx_z >= 0)
			{
				cols[5] = idx_z;
				//this->mtx_cols.push_back(idx_z);
				//this->mtx_rows.push_back(i);
				//this->mtx_val.push_back(1);
				related_p[5] = 1;
			}

			int sum = related_p[0] + related_p[1] + related_p[2] + related_p[3] + related_p[4] + related_p[5];
			if (related_p[0] == 1)
			{
				this->mtx_cols.emplace_back(cols[0]);
				this->mtx_rows.emplace_back(i);
				this->mtx_val.emplace_back(1);
			}
			if (related_p[1] == 1)
			{
				this->mtx_cols.emplace_back(cols[1]);
				this->mtx_rows.emplace_back(i);
				this->mtx_val.emplace_back(1);
			}
			if (related_p[2] == 1)
			{
				this->mtx_cols.emplace_back(cols[2]);
				this->mtx_rows.emplace_back(i);
				this->mtx_val.emplace_back(1);
			}
			this->mtx_cols.emplace_back(i);
			this->mtx_rows.emplace_back(i);
			this->mtx_val.emplace_back(-sum);
			if (related_p[3] == 1)
			{
				this->mtx_cols.emplace_back(cols[3]);
				this->mtx_rows.emplace_back(i);
				this->mtx_val.emplace_back(1);
			}
			if (related_p[4] == 1)
			{
				this->mtx_cols.emplace_back(cols[4]);
				this->mtx_rows.emplace_back(i);
				this->mtx_val.emplace_back(1);
			}
			if (related_p[5] == 1)
			{
				this->mtx_cols.emplace_back(cols[5]);
				this->mtx_rows.emplace_back(i);
				this->mtx_val.emplace_back(1);
			}
		}
		else
		{
			this->mtx_cols.emplace_back(i);
			this->mtx_rows.emplace_back(i);
			this->mtx_val.emplace_back(1);
		}
	}

}

void ConstructeLaplaceMtx::SaveMTX2MAT(const char* filename)
{
	long int mtx_size[2] = { this->nnz, this->nnz };
	ParseMATFile matio;
	matio.SaveMTXtoMAT(filename, "coo_row", this->mtx_rows,
		"coo_col", this->mtx_cols,
		"coo_val", this->mtx_val,
		"mtx_size", (long int*)mtx_size,
		"translate_x", this->phi_x,
		"translate_y", this->phi_y,
		"translate_z", this->phi_z);
}


long int ConstructeLaplaceMtx::coor_in_array(int x, int y, int z, long int start, long int end)
{
	for (long int i = start; i < end; i++)
	{
		if (x == this->coords_xyz[i][0] && y == this->coords_xyz[i][1] && z == this->coords_xyz[i][2])
			return i;
	}
	return -1;
}

long int ConstructeLaplaceMtx::max(long int x1, long int x2)
{
	if (x1 < x2)
		return x2;
	else
		return x1;
}

long int ConstructeLaplaceMtx::min(long int x1, long int x2)
{
	if (x1 > x2)
		return x2;
	else
		return x1;
}
