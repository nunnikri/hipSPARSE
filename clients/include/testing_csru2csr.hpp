/* ************************************************************************
 * Copyright (c) 2021 Advanced Micro Devices, Inc.
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
#ifndef TESTING_CSRU2CSR_HPP
#define TESTING_CSRU2CSR_HPP

#include "hipsparse.hpp"
#include "hipsparse_test_unique_ptr.hpp"
#include "unit.hpp"
#include "utility.hpp"

#include <hipsparse.h>
#include <string>

using namespace hipsparse;
using namespace hipsparse_test;

void testing_csru2csr_bad_arg(void)
{
    int m         = 100;
    int n         = 100;
    int nnz       = 100;
    int safe_size = 100;

    std::unique_ptr<handle_struct> unique_ptr_handle(new handle_struct);
    hipsparseHandle_t              handle = unique_ptr_handle->handle;

    std::unique_ptr<descr_struct> unique_ptr_descr(new descr_struct);
    hipsparseMatDescr_t           descr = unique_ptr_descr->descr;

    std::unique_ptr<csru2csr_struct> unique_ptr_csru2csr(new csru2csr_struct);
    csru2csrInfo_t                   info = unique_ptr_csru2csr->info;

    size_t bufferSize;

    auto csr_row_ptr_managed
        = hipsparse_unique_ptr{device_malloc(sizeof(int) * safe_size), device_free};
    auto csr_col_ind_managed
        = hipsparse_unique_ptr{device_malloc(sizeof(int) * safe_size), device_free};
    auto csr_val_managed
        = hipsparse_unique_ptr{device_malloc(sizeof(float) * safe_size), device_free};
    auto buffer_managed
        = hipsparse_unique_ptr{device_malloc(sizeof(char) * safe_size), device_free};

    int*   csr_row_ptr = (int*)csr_row_ptr_managed.get();
    int*   csr_col_ind = (int*)csr_col_ind_managed.get();
    float* csr_val     = (float*)csr_val_managed.get();
    void*  buffer      = (void*)buffer_managed.get();

    if(!csr_row_ptr || !csr_col_ind || !csr_val || !buffer)
    {
        PRINT_IF_HIP_ERROR(hipErrorOutOfMemory);
        return;
    }

    // Testing csru2csr_bufferSizeExt for bad args
#ifndef __HIP_PLATFORM_NVIDIA__
    // cusparse seem to not have any error checking
    verify_hipsparse_status_invalid_handle(hipsparseXcsru2csr_bufferSizeExt(
        nullptr, m, n, nnz, csr_val, csr_row_ptr, csr_col_ind, info, &bufferSize));
    verify_hipsparse_status_invalid_pointer(
        hipsparseXcsru2csr_bufferSizeExt(
            handle, m, n, nnz, (float*)nullptr, csr_row_ptr, csr_col_ind, info, &bufferSize),
        "Error: csr_val is nullptr");
    verify_hipsparse_status_invalid_pointer(
        hipsparseXcsru2csr_bufferSizeExt(
            handle, m, n, nnz, csr_val, nullptr, csr_col_ind, info, &bufferSize),
        "Error: csr_row_ptr is nullptr");
    verify_hipsparse_status_invalid_pointer(
        hipsparseXcsru2csr_bufferSizeExt(
            handle, m, n, nnz, csr_val, csr_row_ptr, nullptr, info, &bufferSize),
        "Error: csr_col_ind is nullptr");
    verify_hipsparse_status_invalid_pointer(
        hipsparseXcsru2csr_bufferSizeExt(
            handle, m, n, nnz, csr_val, csr_row_ptr, csr_col_ind, nullptr, &bufferSize),
        "Error: info is nullptr");
    verify_hipsparse_status_invalid_pointer(
        hipsparseXcsru2csr_bufferSizeExt(
            handle, m, n, nnz, csr_val, csr_row_ptr, csr_col_ind, info, nullptr),
        "Error: bufferSize is nullptr");
#endif
    verify_hipsparse_status_invalid_size(
        hipsparseXcsru2csr_bufferSizeExt(
            handle, -1, n, nnz, csr_val, csr_row_ptr, csr_col_ind, info, &bufferSize),
        "Error: m is invalid");
    verify_hipsparse_status_invalid_size(
        hipsparseXcsru2csr_bufferSizeExt(
            handle, m, -1, nnz, csr_val, csr_row_ptr, csr_col_ind, info, &bufferSize),
        "Error: n is invalid");
    verify_hipsparse_status_invalid_size(
        hipsparseXcsru2csr_bufferSizeExt(
            handle, m, n, -1, csr_val, csr_row_ptr, csr_col_ind, info, &bufferSize),
        "Error: nnz is invalid");
#ifndef __HIP_PLATFORM_NVIDIA__
    // cusparse seem to not have any error checking for some parts
    verify_hipsparse_status_success(
        hipsparseXcsru2csr_bufferSizeExt(
            handle, 0, n, 0, (float*)nullptr, nullptr, nullptr, nullptr, &bufferSize),
        "Success");
    verify_hipsparse_status_success(
        hipsparseXcsru2csr_bufferSizeExt(
            handle, m, 0, 0, (float*)nullptr, nullptr, nullptr, nullptr, &bufferSize),
        "Success");
    verify_hipsparse_status_invalid_size(
        hipsparseXcsru2csr_bufferSizeExt(
            handle, 0, n, nnz, (float*)nullptr, nullptr, nullptr, nullptr, &bufferSize),
        "Error: nnz is invalid");
    verify_hipsparse_status_invalid_pointer(
        hipsparseXcsru2csr_bufferSizeExt(
            handle, 0, n, 0, (float*)nullptr, nullptr, nullptr, nullptr, nullptr),
        "Error: bufferSize is invalid");
#endif

    // Testing csru2csr for bad args
#ifndef __HIP_PLATFORM_NVIDIA__
    // cusparse seem to not have any error checking for some parts
    verify_hipsparse_status_invalid_handle(hipsparseXcsru2csr(
        nullptr, m, n, nnz, descr, csr_val, csr_row_ptr, csr_col_ind, info, buffer));
    verify_hipsparse_status_invalid_pointer(
        hipsparseXcsru2csr(
            handle, m, n, nnz, descr, csr_val, csr_row_ptr, csr_col_ind, nullptr, buffer),
        "Error: info is nullptr");
#endif
    verify_hipsparse_status_invalid_pointer(
        hipsparseXcsru2csr(
            handle, m, n, nnz, nullptr, csr_val, csr_row_ptr, csr_col_ind, info, buffer),
        "Error: descr is nullptr");
    verify_hipsparse_status_invalid_pointer(
        hipsparseXcsru2csr(
            handle, m, n, nnz, descr, (float*)nullptr, csr_row_ptr, csr_col_ind, info, buffer),
        "Error: csr_val is nullptr");
    verify_hipsparse_status_invalid_pointer(
        hipsparseXcsru2csr(handle, m, n, nnz, descr, csr_val, nullptr, csr_col_ind, info, buffer),
        "Error: csr_row_ptr is nullptr");
    verify_hipsparse_status_invalid_pointer(
        hipsparseXcsru2csr(handle, m, n, nnz, descr, csr_val, csr_row_ptr, nullptr, info, buffer),
        "Error: csr_col_ind is nullptr");
    verify_hipsparse_status_invalid_pointer(
        hipsparseXcsru2csr(
            handle, m, n, nnz, descr, csr_val, csr_row_ptr, csr_col_ind, info, nullptr),
        "Error: buffer is nullptr");
    verify_hipsparse_status_invalid_size(
        hipsparseXcsru2csr(
            handle, -1, n, nnz, descr, csr_val, csr_row_ptr, csr_col_ind, info, buffer),
        "Error: m is invalid");
    verify_hipsparse_status_invalid_size(
        hipsparseXcsru2csr(
            handle, m, -1, nnz, descr, csr_val, csr_row_ptr, csr_col_ind, info, buffer),
        "Error: n is invalid");
    verify_hipsparse_status_invalid_size(
        hipsparseXcsru2csr(
            handle, m, n, -1, descr, csr_val, csr_row_ptr, csr_col_ind, info, buffer),
        "Error: nnz is invalid");
#ifndef __HIP_PLATFORM_NVIDIA__
    // cusparse seem to not have any error checking for some parts
    verify_hipsparse_status_success(
        hipsparseXcsru2csr(
            handle, 0, n, 0, nullptr, (float*)nullptr, nullptr, nullptr, nullptr, &bufferSize),
        "Success");
    verify_hipsparse_status_success(
        hipsparseXcsru2csr(
            handle, m, 0, 0, nullptr, (float*)nullptr, nullptr, nullptr, nullptr, &bufferSize),
        "Success");
    verify_hipsparse_status_invalid_size(
        hipsparseXcsru2csr(
            handle, 0, n, nnz, nullptr, (float*)nullptr, nullptr, nullptr, nullptr, &bufferSize),
        "Error: nnz is invalid");
#endif

    // Testing csr2csru for bad args
#ifndef __HIP_PLATFORM_NVIDIA__
    // cusparse seem to not have any error checking for some parts
    verify_hipsparse_status_invalid_handle(hipsparseXcsr2csru(
        nullptr, m, n, nnz, descr, csr_val, csr_row_ptr, csr_col_ind, info, buffer));
    verify_hipsparse_status_invalid_pointer(
        hipsparseXcsr2csru(
            handle, m, n, nnz, descr, csr_val, csr_row_ptr, csr_col_ind, nullptr, buffer),
        "Error: info is nullptr");
#endif
    verify_hipsparse_status_invalid_pointer(
        hipsparseXcsr2csru(
            handle, m, n, nnz, nullptr, csr_val, csr_row_ptr, csr_col_ind, info, buffer),
        "Error: descr is nullptr");
    verify_hipsparse_status_invalid_pointer(
        hipsparseXcsr2csru(
            handle, m, n, nnz, descr, (float*)nullptr, csr_row_ptr, csr_col_ind, info, buffer),
        "Error: csr_val is nullptr");
    verify_hipsparse_status_invalid_pointer(
        hipsparseXcsr2csru(handle, m, n, nnz, descr, csr_val, nullptr, csr_col_ind, info, buffer),
        "Error: csr_row_ptr is nullptr");
    verify_hipsparse_status_invalid_pointer(
        hipsparseXcsr2csru(handle, m, n, nnz, descr, csr_val, csr_row_ptr, nullptr, info, buffer),
        "Error: csr_col_ind is nullptr");
    verify_hipsparse_status_invalid_pointer(
        hipsparseXcsr2csru(
            handle, m, n, nnz, descr, csr_val, csr_row_ptr, csr_col_ind, info, nullptr),
        "Error: buffer is nullptr");
    verify_hipsparse_status_invalid_size(
        hipsparseXcsr2csru(
            handle, -1, n, nnz, descr, csr_val, csr_row_ptr, csr_col_ind, info, buffer),
        "Error: m is invalid");
    verify_hipsparse_status_invalid_size(
        hipsparseXcsr2csru(
            handle, m, -1, nnz, descr, csr_val, csr_row_ptr, csr_col_ind, info, buffer),
        "Error: n is invalid");
    verify_hipsparse_status_invalid_size(
        hipsparseXcsr2csru(
            handle, m, n, -1, descr, csr_val, csr_row_ptr, csr_col_ind, info, buffer),
        "Error: nnz is invalid");
#ifndef __HIP_PLATFORM_NVIDIA__
    // cusparse seem to not have any error checking for some parts
    verify_hipsparse_status_success(
        hipsparseXcsr2csru(
            handle, 0, n, 0, nullptr, (float*)nullptr, nullptr, nullptr, nullptr, &bufferSize),
        "Success");
    verify_hipsparse_status_success(
        hipsparseXcsr2csru(
            handle, m, 0, 0, nullptr, (float*)nullptr, nullptr, nullptr, nullptr, &bufferSize),
        "Success");
    verify_hipsparse_status_invalid_size(
        hipsparseXcsr2csru(
            handle, 0, n, nnz, nullptr, (float*)nullptr, nullptr, nullptr, nullptr, &bufferSize),
        "Error: nnz is invalid");
#endif
}

template <typename T>
hipsparseStatus_t testing_csru2csr(void)
{
    // hipSPARSE handle
    std::unique_ptr<handle_struct> test_handle(new handle_struct);
    hipsparseHandle_t              handle = test_handle->handle;

    std::unique_ptr<descr_struct> test_descr(new descr_struct);
    hipsparseMatDescr_t           descr = test_descr->descr;

    std::unique_ptr<csru2csr_struct> test_info(new csru2csr_struct);
    csru2csrInfo_t                   info = test_info->info;

    // Sample test matrix
    srand(12345ULL);

    int m   = 51314;
    int n   = 12963;
    int nnz = 309274;

    // Sample random COO matrix
    std::vector<int> hcoo_row_ind;
    std::vector<int> hcsr_row_ptr(m + 1, 0);
    std::vector<int> hcsr_col_ind_gold;
    std::vector<T>   hcsr_val_gold;

    gen_matrix_coo(
        m, n, nnz, hcoo_row_ind, hcsr_col_ind_gold, hcsr_val_gold, HIPSPARSE_INDEX_BASE_ZERO);

    // Convert COO to CSR
    for(int i = 0; i < nnz; ++i)
    {
        ++hcsr_row_ptr[hcoo_row_ind[i] + 1];
    }

    hcsr_row_ptr[0] = 0;
    for(int i = 0; i < m; ++i)
    {
        hcsr_row_ptr[i + 1] += hcsr_row_ptr[i];
    }

    // Unsort CSR columns
    std::vector<int> hperm(nnz);
    std::vector<int> hcsr_col_ind_unsorted_gold(nnz);
    std::vector<T>   hcsr_val_unsorted_gold(nnz);

    hcsr_col_ind_unsorted_gold = hcsr_col_ind_gold;
    hcsr_val_unsorted_gold     = hcsr_val_gold;

    for(int i = 0; i < m; ++i)
    {
        int row_begin = hcsr_row_ptr[i];
        int row_end   = hcsr_row_ptr[i + 1];
        int row_nnz   = row_end - row_begin;

        for(int j = row_begin; j < row_end; ++j)
        {
            int rng = row_begin + rand() % row_nnz;

            int temp_col = hcsr_col_ind_unsorted_gold[j];
            T   temp_val = hcsr_val_unsorted_gold[j];

            hcsr_col_ind_unsorted_gold[j] = hcsr_col_ind_unsorted_gold[rng];
            hcsr_val_unsorted_gold[j]     = hcsr_val_unsorted_gold[rng];

            hcsr_col_ind_unsorted_gold[rng] = temp_col;
            hcsr_val_unsorted_gold[rng]     = temp_val;
        }
    }

    // Allocate memory on the device
    auto dcsr_row_ptr_managed
        = hipsparse_unique_ptr{device_malloc(sizeof(int) * (m + 1)), device_free};
    auto dcsr_col_ind_managed = hipsparse_unique_ptr{device_malloc(sizeof(int) * nnz), device_free};
    auto dcsr_val_managed     = hipsparse_unique_ptr{device_malloc(sizeof(T) * nnz), device_free};

    int* dcsr_row_ptr = (int*)dcsr_row_ptr_managed.get();
    int* dcsr_col_ind = (int*)dcsr_col_ind_managed.get();
    T*   dcsr_val     = (T*)dcsr_val_managed.get();

    if(!dcsr_row_ptr || !dcsr_col_ind || !dcsr_val)
    {
        verify_hipsparse_status_success(HIPSPARSE_STATUS_ALLOC_FAILED,
                                        "!dcsr_row_ptr || !dcsr_col_ind || !dcsr_val");
        return HIPSPARSE_STATUS_ALLOC_FAILED;
    }

    // Copy data from host to device
    CHECK_HIP_ERROR(
        hipMemcpy(dcsr_row_ptr, hcsr_row_ptr.data(), sizeof(int) * (m + 1), hipMemcpyHostToDevice));
    CHECK_HIP_ERROR(hipMemcpy(
        dcsr_col_ind, hcsr_col_ind_unsorted_gold.data(), sizeof(int) * nnz, hipMemcpyHostToDevice));
    CHECK_HIP_ERROR(
        hipMemcpy(dcsr_val, hcsr_val_unsorted_gold.data(), sizeof(T) * nnz, hipMemcpyHostToDevice));

    // Obtain buffer size
    size_t buffer_size;

    CHECK_HIPSPARSE_ERROR(hipsparseXcsru2csr_bufferSizeExt(
        handle, m, n, nnz, dcsr_val, dcsr_row_ptr, dcsr_col_ind, info, &buffer_size));

    // Allocate buffer on the device
    auto dbuffer_managed
        = hipsparse_unique_ptr{device_malloc(sizeof(char) * buffer_size), device_free};

    void* dbuffer = (void*)dbuffer_managed.get();

    if(!dbuffer)
    {
        verify_hipsparse_status_success(HIPSPARSE_STATUS_ALLOC_FAILED, "!dbuffer");
        return HIPSPARSE_STATUS_ALLOC_FAILED;
    }

    // Sort CSR columns
    CHECK_HIPSPARSE_ERROR(hipsparseXcsru2csr(
        handle, m, n, nnz, descr, dcsr_val, dcsr_row_ptr, dcsr_col_ind, info, dbuffer));

    // Copy output from device to host
    std::vector<int> hcsr_col_ind(nnz);
    std::vector<T>   hcsr_val(nnz);

    CHECK_HIP_ERROR(
        hipMemcpy(hcsr_col_ind.data(), dcsr_col_ind, sizeof(int) * nnz, hipMemcpyDeviceToHost));
    CHECK_HIP_ERROR(hipMemcpy(hcsr_val.data(), dcsr_val, sizeof(T) * nnz, hipMemcpyDeviceToHost));

    // Unsort CSR columns back to original state
    CHECK_HIPSPARSE_ERROR(hipsparseXcsr2csru(
        handle, m, n, nnz, descr, dcsr_val, dcsr_row_ptr, dcsr_col_ind, info, dbuffer));

    // Copy output from device to host
    std::vector<int> hcsr_col_ind_unsorted(nnz);
    std::vector<T>   hcsr_val_unsorted(nnz);

    CHECK_HIP_ERROR(hipMemcpy(
        hcsr_col_ind_unsorted.data(), dcsr_col_ind, sizeof(int) * nnz, hipMemcpyDeviceToHost));
    CHECK_HIP_ERROR(
        hipMemcpy(hcsr_val_unsorted.data(), dcsr_val, sizeof(T) * nnz, hipMemcpyDeviceToHost));

    // Unit check
    unit_check_general(1, nnz, 1, hcsr_col_ind.data(), hcsr_col_ind_gold.data());
    unit_check_general(1, nnz, 1, hcsr_val.data(), hcsr_val_gold.data());
    unit_check_general(1, nnz, 1, hcsr_col_ind_unsorted.data(), hcsr_col_ind_unsorted_gold.data());
    unit_check_general(1, nnz, 1, hcsr_val_unsorted.data(), hcsr_val_unsorted_gold.data());

    return HIPSPARSE_STATUS_SUCCESS;
}

#endif // TESTING_CSRU2CSR_HPP
