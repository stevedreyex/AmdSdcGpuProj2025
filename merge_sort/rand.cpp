#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Usage: ./<program> <number of n>" << endl;
        return 1;
    }
    int size = atoi(argv[1]); // 亂數數量
    vector<int> randomNumbers(size);

    // 初始化隨機數生成器
    srand(static_cast<unsigned int>(time(nullptr)));

    // 生成亂數
    for (int i = 0; i < size; ++i) {
        randomNumbers[i] = rand();
    }

    for (int i = 0; i < size; ++i) {
        cout << randomNumbers[i] << " ";
    }
    cout << endl;

    return 0;
}
