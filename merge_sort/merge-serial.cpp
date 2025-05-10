#include <iostream>
#include <vector>
#include <chrono> // 用於計時
using namespace std;

// 合併兩個子陣列的函式
void merge(vector<int>& arr, int left, int mid, int right) {
    int n1 = mid - left + 1; // 左子陣列大小
    int n2 = right - mid;    // 右子陣列大小

    // 建立暫時的子陣列
    vector<int> leftArray(n1), rightArray(n2);

    // 將資料拷貝到暫時的子陣列
    for (int i = 0; i < n1; ++i)
        leftArray[i] = arr[left + i];
    for (int j = 0; j < n2; ++j)
        rightArray[j] = arr[mid + 1 + j];

    // 合併兩個子陣列
    int i = 0, j = 0, k = left; // i 和 j 為子陣列指標，k 為合併陣列指標
    while (i < n1 && j < n2) {
        if (leftArray[i] <= rightArray[j]) {
            arr[k] = leftArray[i];
            ++i;
        } else {
            arr[k] = rightArray[j];
            ++j;
        }
        ++k;
    }

    // 拷貝剩餘元素（若有）
    while (i < n1) {
        arr[k] = leftArray[i];
        ++i;
        ++k;
    }
    while (j < n2) {
        arr[k] = rightArray[j];
        ++j;
        ++k;
    }
}

// Merge Sort 的遞迴函式
void mergeSort(vector<int>& arr, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2; // 避免溢位計算中間點

        // 遞迴排序左右子陣列
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);

        // 合併已排序的子陣列
        merge(arr, left, mid, right);
    }
}

// 主函式
int main(int argc, char* argv[]) {
    // int n = 100000;

    if (argc < 2) {
        cout << "Usage: ./<program> <number of n>" << endl;
        return 1;
    }
    int n = atoi(argv[1]);

    vector<int> arr(n);
    for (int i = 0; i < n; ++i) {
        cin >> arr[i];
    }

    // 計時開始
    clock_t t1, t2;

	t1 = clock();

    mergeSort(arr, 0, arr.size() - 1);

    // 計時結束
    t2 = clock();

	cout << "Serial Time taken: " << (t2 - t1) / 
			(double)CLOCKS_PER_SEC << endl;

    return 0;
}
