#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <time.h>
#include <math.h>
#include <vector>
#include <memory>
#include <iostream>
#include <algorithm>
#define BIG (1e7)
// #define DEBUG

using namespace std;

template<typename T>
__global__ void mergeVec_half(T* A, T* tmp, const int64_t vSize) {
    int64_t left = blockIdx.x * vSize;
    int64_t right = left + vSize - 1;
    int64_t mid = (left + right) / 2;

    int64_t i = left, j = mid + 1, k = left;
    while ((i <= mid) && (j <= right)) {
        if (A[i] <= A[j]) {
            tmp[k++] = A[i++];
        } else {
            tmp[k++] = A[j++];
        }
    }
    if (i > mid) {
        for (; j <= right; ++j, ++k) {
            tmp[k] = A[j];
        }
    } else {
        for (; i <= mid; ++i, ++k) {
            tmp[k] = A[i];
        }
    }
    for (k = left; k <= right; ++k) {
        A[k] = tmp[k];
    }
}

template<typename theIterator, typename T>
void mergeSort_power2n(theIterator begin, theIterator end, T args) {
    clock_t begT, endT;

    T* dataA, * dataTmp;
    int64_t vSize = end - begin;
    cudaMalloc((void**)&dataA, sizeof(*begin) * vSize);
    cudaMalloc((void**)&dataTmp, sizeof(*begin) * vSize);

#ifdef DEBUG
    int64_t n = 0;
    if (vSize >= 2) {
        for (int64_t i = 1; i < vSize; i <<= 1) {
            n += 1;
        }
    } else {
        return;
    }
    if (((int64_t)1 << n) > vSize) {
        cerr << "\033[31;1m error! vSize != 2 ** n \033[0m";
        exit(-1);
    }
#endif

    begT = clock();
    cudaMemcpy(dataA, &(*begin), sizeof(*begin) * vSize, cudaMemcpyHostToDevice);

    for (int64_t i = 2; i <= vSize; i <<= 1) {
        mergeVec_half<<<vSize / i, 1>>>(dataA, dataTmp, i);
        cudaError_t err = cudaGetLastError();
        if (err != cudaSuccess) {
            printf("CUDA Error: %s\n", cudaGetErrorString(err));
        }
    }

    cudaMemcpy(&(*begin), dataA, sizeof(*begin) * vSize, cudaMemcpyDeviceToHost);
    endT = clock();
    cout << "inside GPU operation, time = " << (endT - begT) / 
			(double)CLOCKS_PER_SEC << endl;

    cudaFree(dataA);
    cudaFree(dataTmp);
}

template<typename theIterator>
inline void mergeSort_power2n(theIterator begin, theIterator end) {
    mergeSort_power2n(begin, end, *begin);
}

template<typename theIterator, typename T>
void mergeVec(theIterator beg1, theIterator end1, theIterator beg2, theIterator end2, T args) {
    vector<T> tmp((end1 - beg1) + (end2 - beg2));
    theIterator i = beg1, j = beg2;
    auto k = tmp.begin();

    while (i != end1 && j != end2) {
        if (*i <= *j) {
            *k++ = *i++;
        } else {
            *k++ = *j++;
        }
    }
    while (i != end1) *k++ = *i++;
    while (j != end2) *k++ = *j++;

    k = tmp.begin();
    for (i = beg1; i != end1; ++i, ++k) {
        *i = *k;
    }
    for (j = beg2; j != end2; ++j, ++k) {
        *j = *k;
    }
}

template<typename theIterator>
inline void mergeVec(theIterator beg1, theIterator end1, theIterator beg2, theIterator end2) {
    mergeVec(beg1, end1, beg2, end2, *beg1);
}

template<typename vec>
void mergeSort_gpu(vec& A) {
    vector<bool> binA;
    int64_t vSize = A.size(), n = A.size();
    int64_t one = 1;
    while (n > 0) {
        binA.push_back(n & one);
        n >>= 1;
    }

    vector<int64_t> idxVec;
    idxVec.push_back(0);
    for (int64_t i = 0; i != binA.size(); ++i) {
        if (binA[i]) {
            idxVec.push_back(idxVec.back() + (one << i));
        }
    }

    for (int64_t i = 0; i != idxVec.size() - 1; ++i) {
        mergeSort_power2n(A.begin() + idxVec[i], A.begin() + idxVec[i + 1]);
    }

    for (int64_t i = 1; i != idxVec.size() - 1; ++i) {
        mergeVec(
            A.begin(), A.begin() + idxVec[i],
            A.begin() + idxVec[i], A.begin() + idxVec[i + 1]);
    }
}

template<typename theIterator, typename T>
void mergeSort_cpu(theIterator begin, theIterator end, T args) {
    if (end - begin <= 1) return;

    auto mid = begin + (end - begin) / 2;
    mergeSort_cpu(begin, mid, args);
    mergeSort_cpu(mid, end, args);

    vector<T> tmp(end - begin);
    auto i = begin, j = mid, k = tmp.begin();
    while (i != mid && j != end) {
        if (*i <= *j) {
            *k++ = *i++;
        } else {
            *k++ = *j++;
        }
    }
    while (i != mid) *k++ = *i++;
    while (j != end) *k++ = *j++;

    k = tmp.begin();
    for (i = begin; i != end; ++i, ++k) {
        *i = *k;
    }
}

template<typename theIterator>
inline void mergeSort_cpu(theIterator begin, theIterator end) {
    mergeSort_cpu(begin, end, *begin);
}

int main(int argc, char* argv[])
{
	if (argc < 2) {
        cout << "Usage: ./<program> <number of n>" << endl;
        return 1;
    }
    int n = atoi(argv[1]);

    vector<int> a(n), b(n), c(n);
    for (int i = 0; i < n; ++i) {
        cin >> a[i];
    }
    b = a;
    c = a;
    
    clock_t t1, t2;
	t1 = clock();
    mergeSort_gpu(a);
    t2 = clock();

	cout << "CUDA Time taken: " << (t2 - t1) / 
			(double)CLOCKS_PER_SEC << endl;

	t1 = clock();
    mergeSort_cpu(b.begin(), b.end());
    t2 = clock();

	cout << "CPU Time taken: " << (t2 - t1) / 
			(double)CLOCKS_PER_SEC << endl;

	t1 = clock();
    stable_sort(c.begin(), c.end());
    t2 = clock();

	cout << "STL Time taken: " << (t2 - t1) / 
			(double)CLOCKS_PER_SEC << endl;
}