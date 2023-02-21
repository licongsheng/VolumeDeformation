#include "ParseMATFile.h"
#include "stdlib.h"
#include "matio.h"
#include "stdio.h"
#include <iostream>
#include <cassert>


ParseMATFile::ParseMATFile()
{

}

ParseMATFile::~ParseMATFile()
{

}


int* ParseMATFile::GetMATData_Int(const char* filename, const char* filed, long int& Len, long int& dim)
{
	mat_t* matfp = Mat_Open(filename, MAT_ACC_RDONLY);
    if (NULL == matfp) {
        fprintf(stderr, "Error opening MAT file %s\n", filename);
        return NULL;
    }

    matvar_t* matVar = NULL;
    // read data 'x'
    matVar = Mat_VarRead(matfp, filed);
    if (matVar) {
        assert(matVar->data_type == MAT_T_INT32);
        size_t xSize = matVar->nbytes / matVar->data_size;
        int* xData = static_cast<int*>(matVar->data);
        //for (int i = 0; i < xSize; ++i) {
        //    std::cout << xData[i] << " ";
       // }
        //std::cout << std::endl;
        for (int i = 0; i < matVar->rank; ++i) {
            std::cout << matVar->dims[i] << " " << std::endl;
        }
        Len = matVar->dims[0];
        if (matVar->rank > 1)
            dim = matVar->dims[1];
        std::cout << std::endl;
        return xData;
    }
    return NULL;
}

double * ParseMATFile::GetMATData_Double(const char* filename, const char* filed, long int& Len, long int &dim)
{

    mat_t* matfp = Mat_Open(filename, MAT_ACC_RDONLY);
    if (NULL == matfp) {
        fprintf(stderr, "Error opening MAT file %s\n", filename);
        return NULL;
    }

    matvar_t* matVar = NULL;
    // read data 'x'
    matVar = Mat_VarRead(matfp, filed);
    if (matVar) {
        assert(matVar->data_type == MAT_T_DOUBLE);
        size_t xSize = matVar->nbytes / matVar->data_size;
        double* xData = static_cast<double*>(matVar->data);
        //for (int i = 0; i < xSize; ++i) {
        //    std::cout << xData[i] << " ";
       // }
        //std::cout << std::endl;
        for (int i = 0; i < matVar->rank; ++i) {
            std::cout << matVar->dims[i] << " "<<std::endl;
        }
        Len = matVar->dims[0];
        if (matVar->rank>1)
            dim = matVar->dims[1];
        return xData;
    }
    return NULL;
}

void ParseMATFile::SaveMTXtoMAT(const char* filename, const char* filed1, vector<long int> mtx_rows,
    const char* filed2, vector<long int> mtx_cols,
    const char* filed3, vector<float> mtx_dats,
    const char* filed4, long int* mtx_size,
    const char* filedx, float* phix,
    const char* filedy, float* phiy,
    const char* filedz, float* phiz)
{
    mat_t* mat;
    matvar_t* matvar;
    size_t dims[2] = {mtx_size[0],1};
    long int* cols;
    long int* rows;
    double* dats;
    printf("Writing %s ...\n", filename);
    mat = Mat_CreateVer(filename, NULL, MAT_FT_DEFAULT);
    if (mat)
    {
        matvar = Mat_VarCreate(filedx, MAT_C_SINGLE, MAT_T_SINGLE, 2, dims, phix, 0);
        Mat_VarWrite(mat, matvar, MAT_COMPRESSION_ZLIB);
        Mat_VarFree(matvar);

        matvar = Mat_VarCreate(filedy, MAT_C_SINGLE, MAT_T_SINGLE, 2, dims, phiy, 0);
        Mat_VarWrite(mat, matvar, MAT_COMPRESSION_ZLIB);
        Mat_VarFree(matvar);

        matvar = Mat_VarCreate(filedz, MAT_C_SINGLE, MAT_T_SINGLE, 2, dims, phiz, 0);
        Mat_VarWrite(mat, matvar, MAT_COMPRESSION_ZLIB);
        Mat_VarFree(matvar);

        cols = new long int[mtx_rows.size()];
        rows = new long int[mtx_rows.size()];
        dats = new double[mtx_rows.size()];
        for (long int i = 0; i < mtx_rows.size(); i++)
        {
            rows[i] = mtx_rows[i];
            cols[i] = mtx_cols[i];
            dats[i] = mtx_dats[i];
        }
        dims[0] = mtx_rows.size();
        dims[1] = 1;

        matvar = Mat_VarCreate(filed1, MAT_C_INT32, MAT_T_INT32, 2, dims, rows, 0);
        Mat_VarWrite(mat, matvar, MAT_COMPRESSION_ZLIB);
        Mat_VarFree(matvar);

        matvar = Mat_VarCreate(filed2, MAT_C_INT32, MAT_T_INT32, 2, dims, cols, 0);
        Mat_VarWrite(mat, matvar, MAT_COMPRESSION_ZLIB);
        Mat_VarFree(matvar);

        matvar = Mat_VarCreate(filed3, MAT_C_DOUBLE, MAT_T_DOUBLE, 2, dims, dats, 0);
        Mat_VarWrite(mat, matvar, MAT_COMPRESSION_ZLIB);
        Mat_VarFree(matvar);


        dims[0] = 2;
        dims[1] = 1;
        matvar = Mat_VarCreate(filed4, MAT_C_INT32, MAT_T_INT32, 2, dims, mtx_size, 0);
        Mat_VarWrite(mat, matvar, MAT_COMPRESSION_NONE);
        Mat_VarFree(matvar);


        Mat_Close(mat);
        delete[]rows;
        delete[]cols;
        delete[]dats;
    }
    else
    {
        printf("Writing %s failed...\n", filename);
    }

}