/* ************************************************************************
 * Copyright 2018 Advanced Micro Devices, Inc.
 * ************************************************************************ */

#pragma once
#ifndef TESTING_DOTI_HPP
#define TESTING_DOTI_HPP

#include "hipsparse_test_unique_ptr.hpp"
#include "hipsparse.hpp"
#include "utility.hpp"
#include "unit.hpp"

#include <hipsparse.h>

using namespace hipsparse;
using namespace hipsparse_test;

template <typename T>
void testing_doti_bad_arg(void)
{
    int nnz       = 100;
    int safe_size = 100;

    hipsparseIndexBase_t idx_base = HIPSPARSE_INDEX_BASE_ZERO;
    hipsparseStatus_t status;

    std::unique_ptr<handle_struct> unique_ptr_handle(new handle_struct);
    hipsparseHandle_t handle = unique_ptr_handle->handle;

    auto dx_val_managed = hipsparse_unique_ptr{device_malloc(sizeof(T) * safe_size), device_free};
    auto dx_ind_managed = hipsparse_unique_ptr{device_malloc(sizeof(int) * safe_size), device_free};
    auto dy_managed     = hipsparse_unique_ptr{device_malloc(sizeof(T) * safe_size), device_free};

    T* dx_val   = (T*)dx_val_managed.get();
    int* dx_ind = (int*)dx_ind_managed.get();
    T* dy       = (T*)dy_managed.get();

    if(!dx_ind || !dx_val || !dy)
    {
        PRINT_IF_HIP_ERROR(hipErrorOutOfMemory);
        return;
    }

    T result;

    // testing for (nullptr == dx_val)
    {
        T* dx_val_null = nullptr;

        status = hipsparseXdoti(handle, nnz, dx_val_null, dx_ind, dy, &result, idx_base);
        verify_hipsparse_status_invalid_pointer(status, "Error: x_val is nullptr");
    }

    // testing for (nullptr == dx_ind)
    {
        int* dx_ind_null = nullptr;

        status = hipsparseXdoti(handle, nnz, dx_val, dx_ind_null, dy, &result, idx_base);
        verify_hipsparse_status_invalid_pointer(status, "Error: x_ind is nullptr");
    }

    // testing for (nullptr == dy)
    {
        T* dy_null = nullptr;

        status = hipsparseXdoti(handle, nnz, dx_val, dx_ind, dy_null, &result, idx_base);
        verify_hipsparse_status_invalid_pointer(status, "Error: y is nullptr");
    }

    // testing for (nullptr == result)
    {
        T* result_null = nullptr;

        status = hipsparseXdoti(handle, nnz, dx_val, dx_ind, dy, result_null, idx_base);
        verify_hipsparse_status_invalid_pointer(status, "Error: result is nullptr");
    }

    // testing for(nullptr == handle)
    {
        hipsparseHandle_t handle_null = nullptr;

        status = hipsparseXdoti(handle_null, nnz, dx_val, dx_ind, dy, &result, idx_base);
        verify_hipsparse_status_invalid_handle(status);
    }
}

template <typename T>
hipsparseStatus_t testing_doti(Arguments argus)
{
    int N                         = argus.N;
    int nnz                       = argus.nnz;
    int safe_size                 = 100;
    hipsparseIndexBase_t idx_base = argus.idx_base;
    hipsparseStatus_t status;

    std::unique_ptr<handle_struct> test_handle(new handle_struct);
    hipsparseHandle_t handle = test_handle->handle;

    // Argument sanity check before allocating invalid memory
    if(nnz <= 0)
    {
        auto dx_ind_managed =
            hipsparse_unique_ptr{device_malloc(sizeof(int) * safe_size), device_free};
        auto dx_val_managed =
            hipsparse_unique_ptr{device_malloc(sizeof(T) * safe_size), device_free};
        auto dy_managed = hipsparse_unique_ptr{device_malloc(sizeof(T) * safe_size), device_free};

        int* dx_ind = (int*)dx_ind_managed.get();
        T* dx_val   = (T*)dx_val_managed.get();
        T* dy       = (T*)dy_managed.get();

        if(!dx_ind || !dx_val || !dy)
        {
            verify_hipsparse_status_success(HIPSPARSE_STATUS_ALLOC_FAILED,
                                            "!dx_ind || !dx_val || !dy");
            return HIPSPARSE_STATUS_ALLOC_FAILED;
        }

        T result;

        CHECK_HIPSPARSE_ERROR(hipsparseSetPointerMode(handle, HIPSPARSE_POINTER_MODE_HOST));
        status = hipsparseXdoti(handle, nnz, dx_val, dx_ind, dy, &result, idx_base);

        if(nnz < 0)
        {
            verify_hipsparse_status_invalid_size(status, "Error: nnz < 0");
        }
        else
        {
            verify_hipsparse_status_success(status, "nnz == 0");
        }

        return HIPSPARSE_STATUS_SUCCESS;
    }

    // Host structures
    std::vector<int> hx_ind(nnz);
    std::vector<T> hx_val(nnz);
    std::vector<T> hy(N);

    T hresult_1;
    T hresult_2;
    T hresult_gold;

    // Initial Data on CPU
    srand(12345ULL);
    hipsparseInitIndex(hx_ind.data(), nnz, 1, N);
    hipsparseInit<T>(hx_val, 1, nnz);
    hipsparseInit<T>(hy, 1, N);

    // allocate memory on device
    auto dx_ind_managed    = hipsparse_unique_ptr{device_malloc(sizeof(int) * nnz), device_free};
    auto dx_val_managed    = hipsparse_unique_ptr{device_malloc(sizeof(T) * nnz), device_free};
    auto dy_managed        = hipsparse_unique_ptr{device_malloc(sizeof(T) * N), device_free};
    auto dresult_2_managed = hipsparse_unique_ptr{device_malloc(sizeof(T)), device_free};

    int* dx_ind  = (int*)dx_ind_managed.get();
    T* dx_val    = (T*)dx_val_managed.get();
    T* dy        = (T*)dy_managed.get();
    T* dresult_2 = (T*)dresult_2_managed.get();

    if(!dx_ind || !dx_val || !dy || !dresult_2)
    {
        verify_hipsparse_status_success(HIPSPARSE_STATUS_ALLOC_FAILED,
                                        "!dx_ind || !dx_val || !dy || !dresult_2");
        return HIPSPARSE_STATUS_ALLOC_FAILED;
    }

    // copy data from CPU to device
    CHECK_HIP_ERROR(hipMemcpy(dx_ind, hx_ind.data(), sizeof(int) * nnz, hipMemcpyHostToDevice));
    CHECK_HIP_ERROR(hipMemcpy(dx_val, hx_val.data(), sizeof(T) * nnz, hipMemcpyHostToDevice));
    CHECK_HIP_ERROR(hipMemcpy(dy, hy.data(), sizeof(T) * N, hipMemcpyHostToDevice));

    if(argus.unit_check)
    {
        // ROCSPARSE pointer mode host
        CHECK_HIPSPARSE_ERROR(hipsparseSetPointerMode(handle, HIPSPARSE_POINTER_MODE_HOST));
        CHECK_HIPSPARSE_ERROR(
            hipsparseXdoti(handle, nnz, dx_val, dx_ind, dy, &hresult_1, idx_base));

        // ROCSPARSE pointer mode device
        CHECK_HIPSPARSE_ERROR(hipsparseSetPointerMode(handle, HIPSPARSE_POINTER_MODE_DEVICE));
        CHECK_HIPSPARSE_ERROR(hipsparseXdoti(handle, nnz, dx_val, dx_ind, dy, dresult_2, idx_base));

        // copy output from device to CPU^
        CHECK_HIP_ERROR(hipMemcpy(&hresult_2, dresult_2, sizeof(T), hipMemcpyDeviceToHost));

        // CPU
        double cpu_time_used = get_time_us();

        hresult_gold = static_cast<T>(0);
        for(int i = 0; i < nnz; ++i)
        {
            hresult_gold += hy[hx_ind[i] - idx_base] * hx_val[i];
        }

        cpu_time_used = get_time_us() - cpu_time_used;

        // enable unit check, notice unit check is not invasive, but norm check is,
        // unit check and norm check can not be interchanged their order
        unit_check_general(1, 1, 1, &hresult_gold, &hresult_1);
        unit_check_general(1, 1, 1, &hresult_gold, &hresult_2);
    }

    if(argus.timing)
    {
        int number_cold_calls = 2;
        int number_hot_calls  = argus.iters;
        CHECK_HIPSPARSE_ERROR(hipsparseSetPointerMode(handle, HIPSPARSE_POINTER_MODE_HOST));

        for(int iter = 0; iter < number_cold_calls; iter++)
        {
            hipsparseXdoti(handle, nnz, dx_val, dx_ind, dy, &hresult_1, idx_base);
        }

        double gpu_time_used = get_time_us(); // in microseconds

        for(int iter = 0; iter < number_hot_calls; iter++)
        {
            hipsparseXdoti(handle, nnz, dx_val, dx_ind, dy, &hresult_1, idx_base);
        }

        gpu_time_used     = (get_time_us() - gpu_time_used) / number_hot_calls;
        double gpu_gflops = (2.0 * nnz) / 1e9 / gpu_time_used * 1e6 * 1;
        double bandwidth  = (sizeof(int) * nnz + sizeof(T) * nnz * 2.0) / gpu_time_used / 1e3;

        printf("nnz\t\tGFlops\tGB/s\tusec\n");
        printf("%9d\t%0.2lf\t%0.2lf\t%0.2lf\n", nnz, gpu_gflops, bandwidth, gpu_time_used);
    }
    return HIPSPARSE_STATUS_SUCCESS;
}

#endif // TESTING_DOTI_HPP