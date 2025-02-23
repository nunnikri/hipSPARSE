/* ************************************************************************
 * Copyright (c) 2018-2019 Advanced Micro Devices, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * ************************************************************************ */

#pragma once
#ifndef TESTING_CSRMV_HPP
#define TESTING_CSRMV_HPP

#include "hipsparse.hpp"
#include "hipsparse_test_unique_ptr.hpp"
#include "unit.hpp"
#include "utility.hpp"

#include <cmath>
#include <hipsparse.h>
#include <string>

using namespace hipsparse;
using namespace hipsparse_test;

template <typename T>
void testing_csrmv_bad_arg(void)
{
#ifdef __HIP_PLATFORM_NVIDIA__
    // do not test for bad args
    return;
#endif
    int                  n         = 100;
    int                  m         = 100;
    int                  nnz       = 100;
    int                  safe_size = 100;
    T                    alpha     = 0.6;
    T                    beta      = 0.2;
    hipsparseOperation_t transA    = HIPSPARSE_OPERATION_NON_TRANSPOSE;
    hipsparseStatus_t    status;

    std::unique_ptr<handle_struct> unique_ptr_handle(new handle_struct);
    hipsparseHandle_t              handle = unique_ptr_handle->handle;

    std::unique_ptr<descr_struct> unique_ptr_descr(new descr_struct);
    hipsparseMatDescr_t           descr = unique_ptr_descr->descr;

    auto dptr_managed = hipsparse_unique_ptr{device_malloc(sizeof(int) * safe_size), device_free};
    auto dcol_managed = hipsparse_unique_ptr{device_malloc(sizeof(int) * safe_size), device_free};
    auto dval_managed = hipsparse_unique_ptr{device_malloc(sizeof(T) * safe_size), device_free};
    auto dx_managed   = hipsparse_unique_ptr{device_malloc(sizeof(T) * safe_size), device_free};
    auto dy_managed   = hipsparse_unique_ptr{device_malloc(sizeof(T) * safe_size), device_free};

    int* dptr = (int*)dptr_managed.get();
    int* dcol = (int*)dcol_managed.get();
    T*   dval = (T*)dval_managed.get();
    T*   dx   = (T*)dx_managed.get();
    T*   dy   = (T*)dy_managed.get();

    if(!dval || !dptr || !dcol || !dx || !dy)
    {
        PRINT_IF_HIP_ERROR(hipErrorOutOfMemory);
        return;
    }

    // testing for(nullptr == dptr)
    {
        int* dptr_null = nullptr;

        status = hipsparseXcsrmv(
            handle, transA, m, n, nnz, &alpha, descr, dval, dptr_null, dcol, dx, &beta, dy);
        verify_hipsparse_status_invalid_pointer(status, "Error: dptr is nullptr");
    }
    // testing for(nullptr == dcol)
    {
        int* dcol_null = nullptr;

        status = hipsparseXcsrmv(
            handle, transA, m, n, nnz, &alpha, descr, dval, dptr, dcol_null, dx, &beta, dy);
        verify_hipsparse_status_invalid_pointer(status, "Error: dcol is nullptr");
    }
    // testing for(nullptr == dval)
    {
        T* dval_null = nullptr;

        status = hipsparseXcsrmv(
            handle, transA, m, n, nnz, &alpha, descr, dval_null, dptr, dcol, dx, &beta, dy);
        verify_hipsparse_status_invalid_pointer(status, "Error: dval is nullptr");
    }
    // testing for(nullptr == dx)
    {
        T* dx_null = nullptr;

        status = hipsparseXcsrmv(
            handle, transA, m, n, nnz, &alpha, descr, dval, dptr, dcol, dx_null, &beta, dy);
        verify_hipsparse_status_invalid_pointer(status, "Error: dx is nullptr");
    }
    // testing for(nullptr == dy)
    {
        T* dy_null = nullptr;

        status = hipsparseXcsrmv(
            handle, transA, m, n, nnz, &alpha, descr, dval, dptr, dcol, dx, &beta, dy_null);
        verify_hipsparse_status_invalid_pointer(status, "Error: dy is nullptr");
    }
    // testing for(nullptr == d_alpha)
    {
        T* d_alpha_null = nullptr;

        status = hipsparseXcsrmv(
            handle, transA, m, n, nnz, d_alpha_null, descr, dval, dptr, dcol, dx, &beta, dy);
        verify_hipsparse_status_invalid_pointer(status, "Error: alpha is nullptr");
    }
    // testing for(nullptr == d_beta)
    {
        T* d_beta_null = nullptr;

        status = hipsparseXcsrmv(
            handle, transA, m, n, nnz, &alpha, descr, dval, dptr, dcol, dx, d_beta_null, dy);
        verify_hipsparse_status_invalid_pointer(status, "Error: beta is nullptr");
    }
    // testing for(nullptr == descr)
    {
        hipsparseMatDescr_t descr_null = nullptr;

        status = hipsparseXcsrmv(
            handle, transA, m, n, nnz, &alpha, descr_null, dval, dptr, dcol, dx, &beta, dy);
        verify_hipsparse_status_invalid_pointer(status, "Error: descr is nullptr");
    }
    // testing for(nullptr == handle)
    {
        hipsparseHandle_t handle_null = nullptr;

        status = hipsparseXcsrmv(
            handle_null, transA, m, n, nnz, &alpha, descr, dval, dptr, dcol, dx, &beta, dy);
        verify_hipsparse_status_invalid_handle(status);
    }
}

template <typename T>
hipsparseStatus_t testing_csrmv(Arguments argus)
{
    int                  safe_size = 100;
    int                  nrow      = argus.M;
    int                  ncol      = argus.N;
    T                    h_alpha   = make_DataType<T>(argus.alpha);
    T                    h_beta    = make_DataType<T>(argus.beta);
    hipsparseOperation_t transA    = argus.transA;
    hipsparseIndexBase_t idx_base  = argus.idx_base;
    std::string          binfile   = "";
    std::string          filename  = "";
    hipsparseStatus_t    status;

    // When in testing mode, M == N == -99 indicates that we are testing with a real
    // matrix from cise.ufl.edu
    if(nrow == -99 && ncol == -99 && argus.timing == 0)
    {
        binfile = argus.filename;
        nrow = ncol = safe_size;
    }

    if(argus.timing == 1)
    {
        filename = argus.filename;
    }

    std::unique_ptr<handle_struct> test_handle(new handle_struct);
    hipsparseHandle_t              handle = test_handle->handle;

    std::unique_ptr<descr_struct> test_descr(new descr_struct);
    hipsparseMatDescr_t           descr = test_descr->descr;

    // Set matrix index base
    CHECK_HIPSPARSE_ERROR(hipsparseSetMatIndexBase(descr, idx_base));

    // Determine number of non-zero elements
    double scale = 0.02;
    if(nrow > 1000 || ncol > 1000)
    {
        scale = 2.0 / std::max(nrow, ncol);
    }
    int nnz = nrow * scale * ncol;

    // Argument sanity check before allocating invalid memory
    if(nrow <= 0 || ncol <= 0 || nnz <= 0)
    {
#ifdef __HIP_PLATFORM_NVIDIA__
        // Do not test args in cusparse
        return HIPSPARSE_STATUS_SUCCESS;
#endif
        auto dptr_managed
            = hipsparse_unique_ptr{device_malloc(sizeof(int) * safe_size), device_free};
        auto dcol_managed
            = hipsparse_unique_ptr{device_malloc(sizeof(int) * safe_size), device_free};
        auto dval_managed = hipsparse_unique_ptr{device_malloc(sizeof(T) * safe_size), device_free};
        auto dx_managed   = hipsparse_unique_ptr{device_malloc(sizeof(T) * safe_size), device_free};
        auto dy_managed   = hipsparse_unique_ptr{device_malloc(sizeof(T) * safe_size), device_free};

        int* dptr = (int*)dptr_managed.get();
        int* dcol = (int*)dcol_managed.get();
        T*   dval = (T*)dval_managed.get();
        T*   dx   = (T*)dx_managed.get();
        T*   dy   = (T*)dy_managed.get();

        if(!dval || !dptr || !dcol || !dx || !dy)
        {
            verify_hipsparse_status_success(HIPSPARSE_STATUS_ALLOC_FAILED,
                                            "!dptr || !dcol || !dval || !dx || !dy");
            return HIPSPARSE_STATUS_ALLOC_FAILED;
        }

        CHECK_HIPSPARSE_ERROR(hipsparseSetPointerMode(handle, HIPSPARSE_POINTER_MODE_HOST));
        status = hipsparseXcsrmv(
            handle, transA, nrow, ncol, nnz, &h_alpha, descr, dval, dptr, dcol, dx, &h_beta, dy);

        if(nrow < 0 || ncol < 0 || nnz < 0)
        {
            verify_hipsparse_status_invalid_size(status, "Error: nrow < 0 || ncol < 0 || nnz < 0");
        }
        else
        {
            verify_hipsparse_status_success(status, "nrow >= 0 && ncol >= 0 && nnz >= 0");
        }

        return HIPSPARSE_STATUS_SUCCESS;
    }

    // Host structures
    std::vector<int> hcsr_row_ptr;
    std::vector<int> hcoo_row_ind;
    std::vector<int> hcol_ind;
    std::vector<T>   hval;

    // Initial Data on CPU
    srand(12345ULL);
    if(binfile != "")
    {
        if(read_bin_matrix(binfile.c_str(), nrow, ncol, nnz, hcsr_row_ptr, hcol_ind, hval, idx_base)
           != 0)
        {
            fprintf(stderr, "Cannot open [read] %s\ncol", binfile.c_str());
            return HIPSPARSE_STATUS_INTERNAL_ERROR;
        }
    }
    else if(argus.laplacian)
    {
        nrow = ncol = gen_2d_laplacian(argus.laplacian, hcsr_row_ptr, hcol_ind, hval, idx_base);
        nnz         = hcsr_row_ptr[nrow];
    }
    else
    {
        if(filename != "")
        {
            if(read_mtx_matrix(
                   filename.c_str(), nrow, ncol, nnz, hcoo_row_ind, hcol_ind, hval, idx_base)
               != 0)
            {
                fprintf(stderr, "Cannot open [read] %s\n", filename.c_str());
                return HIPSPARSE_STATUS_INTERNAL_ERROR;
            }
        }
        else
        {
            gen_matrix_coo(nrow, ncol, nnz, hcoo_row_ind, hcol_ind, hval, idx_base);
        }

        // Convert COO to CSR
        if(!argus.laplacian)
        {
            hcsr_row_ptr.resize(nrow + 1, 0);
            for(int i = 0; i < nnz; ++i)
            {
                ++hcsr_row_ptr[hcoo_row_ind[i] + 1 - idx_base];
            }

            hcsr_row_ptr[0] = idx_base;
            for(int i = 0; i < nrow; ++i)
            {
                hcsr_row_ptr[i + 1] += hcsr_row_ptr[i];
            }
        }
    }

    int m = (transA == HIPSPARSE_OPERATION_NON_TRANSPOSE) ? nrow : ncol;
    int n = (transA == HIPSPARSE_OPERATION_NON_TRANSPOSE) ? ncol : nrow;

    std::vector<T> hx(n);
    std::vector<T> hy_1(m);
    std::vector<T> hy_2(m);
    std::vector<T> hy_gold(m);

    hipsparseInit<T>(hx, 1, n);
    hipsparseInit<T>(hy_1, 1, m);

    // copy vector is easy in STL; hy_gold = hx: save a copy in hy_gold which will be output of CPU
    hy_2    = hy_1;
    hy_gold = hy_1;

    // allocate memory on device
    auto dptr_managed = hipsparse_unique_ptr{device_malloc(sizeof(int) * (nrow + 1)), device_free};
    auto dcol_managed = hipsparse_unique_ptr{device_malloc(sizeof(int) * nnz), device_free};
    auto dval_managed = hipsparse_unique_ptr{device_malloc(sizeof(T) * nnz), device_free};
    auto dx_managed   = hipsparse_unique_ptr{device_malloc(sizeof(T) * n), device_free};
    auto dy_1_managed = hipsparse_unique_ptr{device_malloc(sizeof(T) * m), device_free};
    auto dy_2_managed = hipsparse_unique_ptr{device_malloc(sizeof(T) * m), device_free};
    auto d_alpha_managed = hipsparse_unique_ptr{device_malloc(sizeof(T)), device_free};
    auto d_beta_managed  = hipsparse_unique_ptr{device_malloc(sizeof(T)), device_free};

    int* dptr    = (int*)dptr_managed.get();
    int* dcol    = (int*)dcol_managed.get();
    T*   dval    = (T*)dval_managed.get();
    T*   dx      = (T*)dx_managed.get();
    T*   dy_1    = (T*)dy_1_managed.get();
    T*   dy_2    = (T*)dy_2_managed.get();
    T*   d_alpha = (T*)d_alpha_managed.get();
    T*   d_beta  = (T*)d_beta_managed.get();

    if(!dval || !dptr || !dcol || !dx || !dy_1 || !dy_2 || !d_alpha || !d_beta)
    {
        verify_hipsparse_status_success(HIPSPARSE_STATUS_ALLOC_FAILED,
                                        "!dval || !dptr || !dcol || !dx || "
                                        "!dy_1 || !dy_2 || !d_alpha || !d_beta");
        return HIPSPARSE_STATUS_ALLOC_FAILED;
    }

    // copy data from CPU to device
    CHECK_HIP_ERROR(
        hipMemcpy(dptr, hcsr_row_ptr.data(), sizeof(int) * (nrow + 1), hipMemcpyHostToDevice));
    CHECK_HIP_ERROR(hipMemcpy(dcol, hcol_ind.data(), sizeof(int) * nnz, hipMemcpyHostToDevice));
    CHECK_HIP_ERROR(hipMemcpy(dval, hval.data(), sizeof(T) * nnz, hipMemcpyHostToDevice));
    CHECK_HIP_ERROR(hipMemcpy(dx, hx.data(), sizeof(T) * n, hipMemcpyHostToDevice));
    CHECK_HIP_ERROR(hipMemcpy(dy_1, hy_1.data(), sizeof(T) * m, hipMemcpyHostToDevice));
    CHECK_HIP_ERROR(hipMemcpy(d_alpha, &h_alpha, sizeof(T), hipMemcpyHostToDevice));
    CHECK_HIP_ERROR(hipMemcpy(d_beta, &h_beta, sizeof(T), hipMemcpyHostToDevice));

    if(argus.unit_check)
    {
        CHECK_HIP_ERROR(hipMemcpy(dy_2, hy_2.data(), sizeof(T) * m, hipMemcpyHostToDevice));

        // ROCSPARSE pointer mode host
        CHECK_HIPSPARSE_ERROR(hipsparseSetPointerMode(handle, HIPSPARSE_POINTER_MODE_HOST));
        CHECK_HIPSPARSE_ERROR(hipsparseXcsrmv(
            handle, transA, nrow, ncol, nnz, &h_alpha, descr, dval, dptr, dcol, dx, &h_beta, dy_1));

        // ROCSPARSE pointer mode device
        CHECK_HIPSPARSE_ERROR(hipsparseSetPointerMode(handle, HIPSPARSE_POINTER_MODE_DEVICE));
        CHECK_HIPSPARSE_ERROR(hipsparseXcsrmv(
            handle, transA, nrow, ncol, nnz, d_alpha, descr, dval, dptr, dcol, dx, d_beta, dy_2));

        // copy output from device to CPU
        CHECK_HIP_ERROR(hipMemcpy(hy_1.data(), dy_1, sizeof(T) * m, hipMemcpyDeviceToHost));
        CHECK_HIP_ERROR(hipMemcpy(hy_2.data(), dy_2, sizeof(T) * m, hipMemcpyDeviceToHost));

        // CPU - do the csrmv row reduction in the same order as the GPU
        if(transA == HIPSPARSE_OPERATION_NON_TRANSPOSE)
        {
            // Query for warpSize
            hipDeviceProp_t prop;
            hipGetDeviceProperties(&prop, 0);

            int WF_SIZE;
            int nnz_per_row = nnz / nrow;

            if(prop.warpSize == 32)
            {
                if(nnz_per_row < 4)
                    WF_SIZE = 2;
                else if(nnz_per_row < 8)
                    WF_SIZE = 4;
                else if(nnz_per_row < 16)
                    WF_SIZE = 8;
                else if(nnz_per_row < 32)
                    WF_SIZE = 16;
                else
                    WF_SIZE = 32;
            }
            else if(prop.warpSize == 64)
            {
                if(nnz_per_row < 4)
                    WF_SIZE = 2;
                else if(nnz_per_row < 8)
                    WF_SIZE = 4;
                else if(nnz_per_row < 16)
                    WF_SIZE = 8;
                else if(nnz_per_row < 32)
                    WF_SIZE = 16;
                else if(nnz_per_row < 64)
                    WF_SIZE = 32;
                else
                    WF_SIZE = 64;
            }
            else
            {
                return HIPSPARSE_STATUS_INTERNAL_ERROR;
            }

            for(int i = 0; i < nrow; ++i)
            {
                std::vector<T> sum(WF_SIZE, make_DataType<T>(0.0));

                for(int j = hcsr_row_ptr[i] - idx_base; j < hcsr_row_ptr[i + 1] - idx_base;
                    j += WF_SIZE)
                {
                    for(int k = 0; k < WF_SIZE; ++k)
                    {
                        if(j + k < hcsr_row_ptr[i + 1] - idx_base)
                        {
                            sum[k] = testing_fma(
                                h_alpha * hval[j + k], hx[hcol_ind[j + k] - idx_base], sum[k]);
                        }
                    }
                }

                for(int j = 1; j < WF_SIZE; j <<= 1)
                {
                    for(int k = 0; k < WF_SIZE - j; ++k)
                    {
                        sum[k] = sum[k] + sum[k + j];
                    }
                }

                if(h_beta == make_DataType<T>(0.0))
                {
                    hy_gold[i] = sum[0];
                }
                else
                {
                    hy_gold[i] = testing_fma(h_beta, hy_gold[i], sum[0]);
                }
            }
        }
        else
        {
            // Scale y with beta
            for(int i = 0; i < ncol; ++i)
            {
                hy_gold[i] = hy_gold[i] * h_beta;
            }

            // Transposed SpMV
            for(int i = 0; i < nrow; ++i)
            {
                int row_begin = hcsr_row_ptr[i] - idx_base;
                int row_end   = hcsr_row_ptr[i + 1] - idx_base;
                T   row_val   = h_alpha * hx[i];

                for(int j = row_begin; j < row_end; ++j)
                {
                    int col = hcol_ind[j] - idx_base;
                    T   val = (transA == HIPSPARSE_OPERATION_CONJUGATE_TRANSPOSE)
                                  ? testing_conj(hval[j])
                                  : hval[j];

                    hy_gold[col] = testing_fma(val, row_val, hy_gold[col]);
                }
            }
        }

        unit_check_near(1, m, 1, hy_gold.data(), hy_1.data());
        unit_check_near(1, m, 1, hy_gold.data(), hy_2.data());
    }

    return HIPSPARSE_STATUS_SUCCESS;
}

#endif // TESTING_CSRMV_HPP
