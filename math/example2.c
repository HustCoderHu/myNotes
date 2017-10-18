#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "cublas_v2.h"
#include "cuda_runtime.h"

#define ROWS 2048
#define COLS 1024

#define CUBLAS_ERROR_CHECK(sdata) if(CUBLAS_STATUS_SUCCESS!=sdata)\
                   {printf("ERROR at:%s:%d\n",__FILE__,__LINE__);exit(-1);}

void InitMemValue(float *x, int n, float down, float up)
{
        int i;
        srand(5778);
        for (i = 0; i < n; i++)
        {
                x[i] = (rand()*1.0f/RAND_MAX)*(up-down)+down;
        }
        return ;
}

void mySgemv(cublasOperation_t trans, \
        float *pfA, int siM, int siN, float *pfX, float *pfY, float alpha, float beta)
{
        int i, j;
        float fSum;
        if (CUBLAS_OP_N == trans)
        {
                for (i = 0; i < siM; i++)
                {
                        fSum = 0.0f;
                        for (j = 0; j < siN; j++)
                        {
                                fSum += pfA[i*siN+j]*pfX[j];  // 37
                        }
                        fSum *= alpha;
                        pfY[i] = pfY[i]*beta + fSum;
                }
        }
        else
        {
                for (i = 0; i < siN; i++)
                {
                        fSum = 0.0f;
                        for (j = 0; j < siM; j++)
                        {
                                fSum += pfA[j*siN+i]*pfX[j];  // 50
                        }
                        fSum *= alpha;
                        pfY[i] = pfY[i]*beta + fSum;
                }
        }
}

float computeError(int n, const float *x, const float *y)
{
        int i;
        float err, fSumError=0.0f;
        for (i = 0; i < n; ++i)
        {
                err = fabsf((x[i]-y[i])/x[i]);
                if (err > 1e-5)
                {
                        printf("%d error : %f\n", i, err);
                }
                fSumError += err;
        }
        return fSumError;
}


int main(int argc, char **argv)
{

        // 1.1 cublas helper variables
        cublasStatus_t cubStatus;
        cublasHandle_t cubHandle;

        // 1.2 vector, matrix, scalar
        int   siM  = ROWS, siN   = COLS;
        float *pfA = 0   , *d_pfA = 0   , *  pfY = 0;
        float *pfX = 0   , *d_pfX = 0   , *d_pfY = 0, *h_pfY = 0;
        cublasOperation_t cubTrans = CUBLAS_OP_T;
        float fAlpha = 0.2f;
        float fBeta  = 0.3f;
        int   siIncx=1; // need to be configured when use.

        // 1.3 cublas init
        cubStatus = cublasCreate(&cubHandle);CUBLAS_ERROR_CHECK(cubStatus)

        // 1.4 memory allocation
        pfA = (float*)malloc(siM*siN*sizeof(float));
        cudaMalloc((void**)&d_pfA, siM*siN*sizeof(float));
        if (CUBLAS_OP_N == cubTrans)
        {
                pfX = (float*)malloc(siN*sizeof(float));
                pfY = (float*)malloc(siM*sizeof(float));
                h_pfY = (float*)malloc(siM*sizeof(float));
                cudaMalloc((void**)&d_pfX, siN*sizeof(float));
                cudaMalloc((void**)&d_pfY, siM*sizeof(float));
        }
        else
        {
                pfX = (float*)malloc(siM*sizeof(float));
                pfY = (float*)malloc(siN*sizeof(float));
                h_pfY = (float*)malloc(siN*sizeof(float));
                cudaMalloc((void**)&d_pfX, siM*sizeof(float));
                cudaMalloc((void**)&d_pfY, siN*sizeof(float));
        }

        // 1.5 initialize CPU memory
        InitMemValue(pfA, siM*siN, 0.0f, 1.0f);
        if (CUBLAS_OP_N == cubTrans)
        {
                InitMemValue(pfX, siN, 0.0f, 1.0f);
                InitMemValue(pfY, siM, 0.0f, 1.0f);
        }
        else
        {
                InitMemValue(pfX, siM, 0.0f, 1.0f);
                InitMemValue(pfY, siN, 0.0f, 1.0f);
        }

        // 1.6 Initialize GPU memory

        // 129
        cubStatus = cublasSetMatrix(siN, siM, sizeof(float), pfA, siN, d_pfA, siN);CUBLAS_ERROR_CHECK(cubStatus)
        if (CUBLAS_OP_N == cubTrans)
        {
                cubStatus = cublasSetVector(siN, sizeof(float), pfX, 1, d_pfX, 1);CUBLAS_ERROR_CHECK(cubStatus)
                cubStatus = cublasSetVector(siM, sizeof(float), pfY, 1, d_pfY, 1);CUBLAS_ERROR_CHECK(cubStatus)
        }
        else
        {
                cubStatus = cublasSetVector(siM, sizeof(float), pfX, 1, d_pfX, 1);CUBLAS_ERROR_CHECK(cubStatus)
                cubStatus = cublasSetVector(siN, sizeof(float), pfY, 1, d_pfY, 1);CUBLAS_ERROR_CHECK(cubStatus)
        }

        // 1.7 Invoke CUBLAS
        //dfTimer_device = getTimeInMicroSecs();
        // 143
        cubStatus = cublasSgemv(cubHandle, cubTrans==CUBLAS_OP_N?CUBLAS_OP_T:CUBLAS_OP_N, siN, siM, &fAlpha, d_pfA, siN, d_pfX, 1, &fBeta, d_pfY, 1);CUBLAS_ERROR_CHECK(cubStatus)
        //dfTimer_device = getTimeInMicroSecs() - dfTimer_device;
        // 1.8 Get result
        if (CUBLAS_OP_N == cubTrans)
        {
                cubStatus = cublasGetVector(siM, sizeof(float), d_pfY, 1, h_pfY, 1);
        }
        else
        {
                cubStatus = cublasGetVector(siN, sizeof(float), d_pfY, 1, h_pfY, 1);
        }


        // 1.9 invoke cpu version
        mySgemv(cubTrans, pfA, siM, siN, pfX, pfY, fAlpha, fBeta);
        printf("Sum of error is %f\n", computeError(CUBLAS_OP_N==cubTrans?siM:siN, pfY, h_pfY));

        // last: release
        cudaFree(d_pfA);
        cudaFree(d_pfX);
        cudaFree(d_pfY);
        free(pfA);
        free(pfX);
        free(pfY);
        free(h_pfY);
        cubStatus = cublasDestroy(cubHandle);CUBLAS_ERROR_CHECK(cubStatus)
        return 0;
}
