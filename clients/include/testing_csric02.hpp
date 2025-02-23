/* ************************************************************************
 * Copyright (c) 2020 Advanced Micro Devices, Inc.
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
#ifndef TESTING_CSRIC0_HPP
#define TESTING_CSRIC0_HPP

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
void testing_csric02_bad_arg(void)
{
#ifdef __HIP_PLATFORM_NVIDIA__
    // do not test for bad args
    return;
#endif
    int                    m         = 100;
    int                    nnz       = 100;
    int                    safe_size = 100;
    hipsparseSolvePolicy_t policy    = HIPSPARSE_SOLVE_POLICY_USE_LEVEL;
    hipsparseStatus_t      status;

    std::unique_ptr<handle_struct> unique_ptr_handle(new handle_struct);
    hipsparseHandle_t              handle = unique_ptr_handle->handle;

    std::unique_ptr<descr_struct> unique_ptr_descr(new descr_struct);
    hipsparseMatDescr_t           descr = unique_ptr_descr->descr;

    std::unique_ptr<csric02_struct> unique_ptr_csric02(new csric02_struct);
    csric02Info_t                   info = unique_ptr_csric02->info;

    auto dptr_managed = hipsparse_unique_ptr{device_malloc(sizeof(int) * safe_size), device_free};
    auto dcol_managed = hipsparse_unique_ptr{device_malloc(sizeof(int) * safe_size), device_free};
    auto dval_managed = hipsparse_unique_ptr{device_malloc(sizeof(T) * safe_size), device_free};
    auto dbuffer_managed
        = hipsparse_unique_ptr{device_malloc(sizeof(char) * safe_size), device_free};

    int*  dptr    = (int*)dptr_managed.get();
    int*  dcol    = (int*)dcol_managed.get();
    T*    dval    = (T*)dval_managed.get();
    void* dbuffer = (void*)dbuffer_managed.get();

    if(!dval || !dptr || !dcol || !dbuffer)
    {
        PRINT_IF_HIP_ERROR(hipErrorOutOfMemory);
        return;
    }

    // testing hipsparseXcsric02_bufferSize
    int size;

    // testing for(nullptr == dptr)
    {
        int* dptr_null = nullptr;

        status = hipsparseXcsric02_bufferSize(
            handle, m, nnz, descr, dval, dptr_null, dcol, info, &size);
        verify_hipsparse_status_invalid_pointer(status, "Error: dptr is nullptr");
    }
    // testing for(nullptr == dcol)
    {
        int* dcol_null = nullptr;

        status = hipsparseXcsric02_bufferSize(
            handle, m, nnz, descr, dval, dptr, dcol_null, info, &size);
        verify_hipsparse_status_invalid_pointer(status, "Error: dcol is nullptr");
    }
    // testing for(nullptr == dval)
    {
        T* dval_null = nullptr;

        status = hipsparseXcsric02_bufferSize(
            handle, m, nnz, descr, dval_null, dptr, dcol, info, &size);
        verify_hipsparse_status_invalid_pointer(status, "Error: dval is nullptr");
    }
    // testing for(nullptr == buffer_size)
    {
        int* size_null = nullptr;

        status = hipsparseXcsric02_bufferSize(
            handle, m, nnz, descr, dval, dptr, dcol, info, size_null);
        verify_hipsparse_status_invalid_pointer(status, "Error: size is nullptr");
    }
    // testing for(nullptr == descr)
    {
        hipsparseMatDescr_t descr_null = nullptr;

        status = hipsparseXcsric02_bufferSize(
            handle, m, nnz, descr_null, dval, dptr, dcol, info, &size);
        verify_hipsparse_status_invalid_pointer(status, "Error: descr is nullptr");
    }
    // testing for(nullptr == info)
    {
        csric02Info_t info_null = nullptr;

        status = hipsparseXcsric02_bufferSize(
            handle, m, nnz, descr, dval, dptr, dcol, info_null, &size);
        verify_hipsparse_status_invalid_pointer(status, "Error: info is nullptr");
    }
    // testing for(nullptr == handle)
    {
        hipsparseHandle_t handle_null = nullptr;

        status = hipsparseXcsric02_bufferSize(
            handle_null, m, nnz, descr, dval, dptr, dcol, info, &size);
        verify_hipsparse_status_invalid_handle(status);
    }

    // testing hipsparseXcsric02_analysis

    // testing for(nullptr == dptr)
    {
        int* dptr_null = nullptr;

        status = hipsparseXcsric02_analysis(
            handle, m, nnz, descr, dval, dptr_null, dcol, info, policy, dbuffer);
        verify_hipsparse_status_invalid_pointer(status, "Error: dptr is nullptr");
    }
    // testing for(nullptr == dcol)
    {
        int* dcol_null = nullptr;

        status = hipsparseXcsric02_analysis(
            handle, m, nnz, descr, dval, dptr, dcol_null, info, policy, dbuffer);
        verify_hipsparse_status_invalid_pointer(status, "Error: dcol is nullptr");
    }
    // testing for(nullptr == dval)
    {
        T* dval_null = nullptr;

        status = hipsparseXcsric02_analysis(
            handle, m, nnz, descr, dval_null, dptr, dcol, info, policy, dbuffer);
        verify_hipsparse_status_invalid_pointer(status, "Error: dval is nullptr");
    }
    // testing for(nullptr == dbuffer)
    {
        void* dbuffer_null = nullptr;

        status = hipsparseXcsric02_analysis(
            handle, m, nnz, descr, dval, dptr, dcol, info, policy, dbuffer_null);
        verify_hipsparse_status_invalid_pointer(status, "Error: dbuffer is nullptr");
    }
    // testing for(nullptr == descr)
    {
        hipsparseMatDescr_t descr_null = nullptr;

        status = hipsparseXcsric02_analysis(
            handle, m, nnz, descr_null, dval, dptr, dcol, info, policy, dbuffer);
        verify_hipsparse_status_invalid_pointer(status, "Error: descr is nullptr");
    }
    // testing for(nullptr == info)
    {
        csric02Info_t info_null = nullptr;

        status = hipsparseXcsric02_analysis(
            handle, m, nnz, descr, dval, dptr, dcol, info_null, policy, dbuffer);
        verify_hipsparse_status_invalid_pointer(status, "Error: info is nullptr");
    }
    // testing for(nullptr == handle)
    {
        hipsparseHandle_t handle_null = nullptr;

        status = hipsparseXcsric02_analysis(
            handle_null, m, nnz, descr, dval, dptr, dcol, info, policy, dbuffer);
        verify_hipsparse_status_invalid_handle(status);
    }

    // testing hipsparseXcsric02

    // testing for(nullptr == dptr)
    {
        int* dptr_null = nullptr;

        status = hipsparseXcsric02(
            handle, m, nnz, descr, dval, dptr_null, dcol, info, policy, dbuffer);
        verify_hipsparse_status_invalid_pointer(status, "Error: dptr is nullptr");
    }
    // testing for(nullptr == dcol)
    {
        int* dcol_null = nullptr;

        status = hipsparseXcsric02(
            handle, m, nnz, descr, dval, dptr, dcol_null, info, policy, dbuffer);
        verify_hipsparse_status_invalid_pointer(status, "Error: dcol is nullptr");
    }
    // testing for(nullptr == dval)
    {
        T* dval_null = nullptr;

        status = hipsparseXcsric02(
            handle, m, nnz, descr, dval_null, dptr, dcol, info, policy, dbuffer);
        verify_hipsparse_status_invalid_pointer(status, "Error: dval is nullptr");
    }
    // testing for(nullptr == dbuffer)
    {
        void* dbuffer_null = nullptr;

        status = hipsparseXcsric02(
            handle, m, nnz, descr, dval, dptr, dcol, info, policy, dbuffer_null);
        verify_hipsparse_status_invalid_pointer(status, "Error: dbuffer is nullptr");
    }
    // testing for(nullptr == descr)
    {
        hipsparseMatDescr_t descr_null = nullptr;

        status = hipsparseXcsric02(
            handle, m, nnz, descr_null, dval, dptr, dcol, info, policy, dbuffer);
        verify_hipsparse_status_invalid_pointer(status, "Error: descr is nullptr");
    }
    // testing for(nullptr == info)
    {
        csric02Info_t info_null = nullptr;

        status = hipsparseXcsric02(
            handle, m, nnz, descr, dval, dptr, dcol, info_null, policy, dbuffer);
        verify_hipsparse_status_invalid_pointer(status, "Error: info is nullptr");
    }
    // testing for(nullptr == handle)
    {
        hipsparseHandle_t handle_null = nullptr;

        status = hipsparseXcsric02(
            handle_null, m, nnz, descr, dval, dptr, dcol, info, policy, dbuffer);
        verify_hipsparse_status_invalid_handle(status);
    }

    // testing hipsparseXcsric02_zeroPivot
    int position;

    // testing for(nullptr == position)
    {
        int* position_null = nullptr;

        status = hipsparseXcsric02_zeroPivot(handle, info, position_null);
        verify_hipsparse_status_invalid_pointer(status, "Error: position is nullptr");
    }
    // testing for(nullptr == info)
    {
        csric02Info_t info_null = nullptr;

        status = hipsparseXcsric02_zeroPivot(handle, info_null, &position);
        verify_hipsparse_status_invalid_pointer(status, "Error: info is nullptr");
    }
    // testing for(nullptr == handle)
    {
        hipsparseHandle_t handle_null = nullptr;

        status = hipsparseXcsric02_zeroPivot(handle_null, info, &position);
        verify_hipsparse_status_invalid_handle(status);
    }
}

template <typename T>
hipsparseStatus_t testing_csric02(Arguments argus)
{
    int                    safe_size = 100;
    int                    m         = argus.M;
    hipsparseIndexBase_t   idx_base  = argus.idx_base;
    hipsparseSolvePolicy_t policy    = HIPSPARSE_SOLVE_POLICY_USE_LEVEL;
    std::string            binfile   = "";
    std::string            filename  = "";
    hipsparseStatus_t      status;
    int                    size;

    // When in testing mode, M == N == -99 indicates that we are testing with a real
    // matrix from cise.ufl.edu
    if(m == -99 && argus.timing == 0)
    {
        binfile = argus.filename;
        m       = safe_size;
    }

    if(argus.timing == 1)
    {
        filename = argus.filename;
    }

    std::unique_ptr<handle_struct> test_handle(new handle_struct);
    hipsparseHandle_t              handle = test_handle->handle;

    std::unique_ptr<descr_struct> test_descr(new descr_struct);
    hipsparseMatDescr_t           descr = test_descr->descr;

    std::unique_ptr<csric02_struct> unique_ptr_csric02(new csric02_struct);
    csric02Info_t                   info = unique_ptr_csric02->info;

    // Set matrix index base
    CHECK_HIPSPARSE_ERROR(hipsparseSetMatIndexBase(descr, idx_base));

    // Determine number of non-zero elements
    double scale = 0.02;
    if(m > 1000)
    {
        scale = 2.0 / m;
    }
    int nnz = m * scale * m;

    // Argument sanity check before allocating invalid memory
    if(m <= 0 || nnz <= 0)
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
        auto buffer_managed
            = hipsparse_unique_ptr{device_malloc(sizeof(char) * safe_size), device_free};

        int*  dptr   = (int*)dptr_managed.get();
        int*  dcol   = (int*)dcol_managed.get();
        T*    dval   = (T*)dval_managed.get();
        void* buffer = (void*)buffer_managed.get();

        if(!dval || !dptr || !dcol || !buffer)
        {
            verify_hipsparse_status_success(HIPSPARSE_STATUS_ALLOC_FAILED,
                                            "!dptr || !dcol || !dval || !buffer");
            return HIPSPARSE_STATUS_ALLOC_FAILED;
        }

        // Test hipsparseXcsric02_bufferSize
        status = hipsparseXcsric02_bufferSize(handle, m, nnz, descr, dval, dptr, dcol, info, &size);

        if(m < 0 || nnz < 0)
        {
            verify_hipsparse_status_invalid_size(status, "Error: m < 0 || nnz < 0");
        }
        else
        {
            verify_hipsparse_status_success(status, "m >= 0 && nnz >= 0");
        }

        // Test hipsparseXcsric02_analysis
        status = hipsparseXcsric02_analysis(
            handle, m, nnz, descr, dval, dptr, dcol, info, policy, buffer);

        if(m < 0 || nnz < 0)
        {
            verify_hipsparse_status_invalid_size(status, "Error: m < 0 || nnz < 0");
        }
        else
        {
            verify_hipsparse_status_success(status, "m >= 0 && nnz >= 0");
        }

        // Test hipsparseXcsric02
        status = hipsparseXcsric02(handle, m, nnz, descr, dval, dptr, dcol, info, policy, buffer);

        if(m < 0 || nnz < 0)
        {
            verify_hipsparse_status_invalid_size(status, "Error: m < 0 || nnz < 0");
        }
        else
        {
            verify_hipsparse_status_success(status, "m >= 0 && nnz >= 0");
        }

        // Test hipsparseXcsric02_zeroPivot
        int zero_pivot;
        CHECK_HIPSPARSE_ERROR(hipsparseXcsric02_zeroPivot(handle, info, &zero_pivot));

        // Zero pivot should be -1
        int res = -1;
        unit_check_general(1, 1, 1, &res, &zero_pivot);

        return HIPSPARSE_STATUS_SUCCESS;
    }

    // Host structures
    std::vector<int> hcsr_row_ptr;
    std::vector<int> hcsr_col_ind;
    std::vector<T>   hcsr_val;

    // Initial Data on CPU
    srand(12345ULL);
    if(binfile != "")
    {
        if(read_bin_matrix(
               binfile.c_str(), m, m, nnz, hcsr_row_ptr, hcsr_col_ind, hcsr_val, idx_base)
           != 0)
        {
            fprintf(stderr, "Cannot open [read] %s\n", binfile.c_str());
            return HIPSPARSE_STATUS_INTERNAL_ERROR;
        }
    }
    else if(argus.laplacian)
    {
        m   = gen_2d_laplacian(argus.laplacian, hcsr_row_ptr, hcsr_col_ind, hcsr_val, idx_base);
        nnz = hcsr_row_ptr[m];
    }
    else
    {
        std::vector<int> hcoo_row_ind;

        if(filename != "")
        {
            if(read_mtx_matrix(
                   filename.c_str(), m, m, nnz, hcoo_row_ind, hcsr_col_ind, hcsr_val, idx_base)
               != 0)
            {
                fprintf(stderr, "Cannot open [read] %s\n", filename.c_str());
                return HIPSPARSE_STATUS_INTERNAL_ERROR;
            }
        }
        else
        {
            gen_matrix_coo(m, m, nnz, hcoo_row_ind, hcsr_col_ind, hcsr_val, idx_base);
        }

        // Convert COO to CSR
        hcsr_row_ptr.resize(m + 1, 0);
        for(int i = 0; i < nnz; ++i)
        {
            ++hcsr_row_ptr[hcoo_row_ind[i] + 1 - idx_base];
        }

        hcsr_row_ptr[0] = idx_base;
        for(int i = 0; i < m; ++i)
        {
            hcsr_row_ptr[i + 1] += hcsr_row_ptr[i];
        }
    }

    // Allocate memory on device
    auto dptr_managed   = hipsparse_unique_ptr{device_malloc(sizeof(int) * (m + 1)), device_free};
    auto dcol_managed   = hipsparse_unique_ptr{device_malloc(sizeof(int) * nnz), device_free};
    auto dval_1_managed = hipsparse_unique_ptr{device_malloc(sizeof(T) * nnz), device_free};
    auto dval_2_managed = hipsparse_unique_ptr{device_malloc(sizeof(T) * nnz), device_free};
    auto d_analysis_pivot_2_managed = hipsparse_unique_ptr{device_malloc(sizeof(int)), device_free};
    auto d_solve_pivot_2_managed    = hipsparse_unique_ptr{device_malloc(sizeof(int)), device_free};

    int* dptr               = (int*)dptr_managed.get();
    int* dcol               = (int*)dcol_managed.get();
    T*   dval_1             = (T*)dval_1_managed.get();
    T*   dval_2             = (T*)dval_2_managed.get();
    int* d_analysis_pivot_2 = (int*)d_analysis_pivot_2_managed.get();
    int* d_solve_pivot_2    = (int*)d_solve_pivot_2_managed.get();

    if(!dval_1 || !dval_2 || !dptr || !dcol || !d_analysis_pivot_2 || !d_solve_pivot_2)
    {
        verify_hipsparse_status_success(
            HIPSPARSE_STATUS_ALLOC_FAILED,
            "!dval_1 || !dval_2 || !dptr || !dcol || !d_analysis_pivot_2 || !d_solve_pivot_2");
        return HIPSPARSE_STATUS_ALLOC_FAILED;
    }

    // copy data from CPU to device
    CHECK_HIP_ERROR(
        hipMemcpy(dptr, hcsr_row_ptr.data(), sizeof(int) * (m + 1), hipMemcpyHostToDevice));
    CHECK_HIP_ERROR(hipMemcpy(dcol, hcsr_col_ind.data(), sizeof(int) * nnz, hipMemcpyHostToDevice));
    CHECK_HIP_ERROR(hipMemcpy(dval_1, hcsr_val.data(), sizeof(T) * nnz, hipMemcpyHostToDevice));

    // Obtain csric02 buffer size
    CHECK_HIPSPARSE_ERROR(
        hipsparseXcsric02_bufferSize(handle, m, nnz, descr, dval_1, dptr, dcol, info, &size));

    // Allocate buffer on the device
    auto dbuffer_managed = hipsparse_unique_ptr{device_malloc(sizeof(char) * size), device_free};

    void* dbuffer = (void*)dbuffer_managed.get();

    if(!dbuffer)
    {
        verify_hipsparse_status_success(HIPSPARSE_STATUS_ALLOC_FAILED, "!dbuffer");
        return HIPSPARSE_STATUS_ALLOC_FAILED;
    }

    int h_analysis_pivot_gold;
    int h_analysis_pivot_1;
    int h_analysis_pivot_2;
    int h_solve_pivot_gold;
    int h_solve_pivot_1;
    int h_solve_pivot_2;

    if(argus.unit_check)
    {
        CHECK_HIP_ERROR(hipMemcpy(dval_2, hcsr_val.data(), sizeof(T) * nnz, hipMemcpyHostToDevice));

        hipsparseStatus_t status_analysis_1;
        hipsparseStatus_t status_analysis_2;
        hipsparseStatus_t status_solve_1;
        hipsparseStatus_t status_solve_2;

        // csric02 analysis - host mode
        CHECK_HIPSPARSE_ERROR(hipsparseSetPointerMode(handle, HIPSPARSE_POINTER_MODE_HOST));
        CHECK_HIPSPARSE_ERROR(hipsparseXcsric02_analysis(
            handle, m, nnz, descr, dval_1, dptr, dcol, info, policy, dbuffer));

        // Get pivot
        status_analysis_1 = hipsparseXcsric02_zeroPivot(handle, info, &h_analysis_pivot_1);
        if(h_analysis_pivot_1 != -1)
        {
            verify_hipsparse_status_zero_pivot(status_analysis_1,
                                               "expected HIPSPARSE_STATUS_ZERO_PIVOT");
        }

        // csric02 analysis - device mode
        CHECK_HIPSPARSE_ERROR(hipsparseSetPointerMode(handle, HIPSPARSE_POINTER_MODE_DEVICE));
        CHECK_HIPSPARSE_ERROR(hipsparseXcsric02_analysis(
            handle, m, nnz, descr, dval_2, dptr, dcol, info, policy, dbuffer));

        // Get pivot
        status_analysis_2 = hipsparseXcsric02_zeroPivot(handle, info, d_analysis_pivot_2);
        if(h_analysis_pivot_1 != -1)
        {
            verify_hipsparse_status_zero_pivot(status_analysis_2,
                                               "expected HIPSPARSE_STATUS_ZERO_PIVOT");
        }

        // csric02 solve - host mode
        CHECK_HIPSPARSE_ERROR(hipsparseSetPointerMode(handle, HIPSPARSE_POINTER_MODE_HOST));
        CHECK_HIPSPARSE_ERROR(
            hipsparseXcsric02(handle, m, nnz, descr, dval_1, dptr, dcol, info, policy, dbuffer));

        // Get pivot
        status_solve_1 = hipsparseXcsric02_zeroPivot(handle, info, &h_solve_pivot_1);
        if(h_solve_pivot_1 != -1)
        {
            verify_hipsparse_status_zero_pivot(status_solve_1,
                                               "expected HIPSPARSE_STATUS_ZERO_PIVOT");
        }

        // csric02 solve - device mode
        CHECK_HIPSPARSE_ERROR(hipsparseSetPointerMode(handle, HIPSPARSE_POINTER_MODE_DEVICE));
        CHECK_HIPSPARSE_ERROR(
            hipsparseXcsric02(handle, m, nnz, descr, dval_2, dptr, dcol, info, policy, dbuffer));

        // Get pivot
        status_solve_2 = hipsparseXcsric02_zeroPivot(handle, info, d_solve_pivot_2);
        if(h_solve_pivot_1 != -1)
        {
            verify_hipsparse_status_zero_pivot(status_solve_2,
                                               "expected HIPSPARSE_STATUS_ZERO_PIVOT");
        }

        // Copy output from device to CPU
        std::vector<T> result_1(nnz);
        std::vector<T> result_2(nnz);

        CHECK_HIP_ERROR(hipMemcpy(result_1.data(), dval_1, sizeof(T) * nnz, hipMemcpyDeviceToHost));
        CHECK_HIP_ERROR(hipMemcpy(result_2.data(), dval_2, sizeof(T) * nnz, hipMemcpyDeviceToHost));
        CHECK_HIP_ERROR(
            hipMemcpy(&h_analysis_pivot_2, d_analysis_pivot_2, sizeof(int), hipMemcpyDeviceToHost));
        CHECK_HIP_ERROR(
            hipMemcpy(&h_solve_pivot_2, d_solve_pivot_2, sizeof(int), hipMemcpyDeviceToHost));

        // Host csric02
        csric0(m,
               hcsr_row_ptr.data(),
               hcsr_col_ind.data(),
               hcsr_val.data(),
               idx_base,
               h_analysis_pivot_gold,
               h_solve_pivot_gold);

#ifndef __HIP_PLATFORM_NVIDIA__
        // Do not check pivots in cusparse
        unit_check_general(1, 1, 1, &h_analysis_pivot_gold, &h_analysis_pivot_1);
        unit_check_general(1, 1, 1, &h_analysis_pivot_gold, &h_analysis_pivot_2);
        unit_check_general(1, 1, 1, &h_solve_pivot_gold, &h_solve_pivot_1);
        unit_check_general(1, 1, 1, &h_solve_pivot_gold, &h_solve_pivot_2);
#endif

        if(h_analysis_pivot_gold == -1 && h_solve_pivot_gold == -1)
        {
            unit_check_near(1, nnz, 1, hcsr_val.data(), result_1.data());
            unit_check_near(1, nnz, 1, hcsr_val.data(), result_2.data());
        }
    }

    return HIPSPARSE_STATUS_SUCCESS;
}

#endif // TESTING_CSRIC0_HPP
