#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <omp.h>

using namespace std;
using namespace chrono;

vector<vector<int>> readMatrix(const string& filename, int& size) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << "\n";
        exit(1);
    }

    file >> size;
    vector<vector<int>> matrix(size, vector<int>(size));

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            file >> matrix[i][j];
        }
    }

    file.close();
    return matrix;
}

void writeMatrix(const string& filename, const vector<vector<int>>& matrix, int size, 
                 long long microseconds, long long operations, int num_threads) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error creating file: " << filename << "\n";
        exit(1);
    }

    file << size << "\n";
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            file << matrix[i][j];
            if (j < size - 1) file << " ";
        }
        file << "\n";
    }

    file << "Время выполнения: " << microseconds << " microseconds\n";
    file << "Количество потоков: " << num_threads << "\n";
    file << "Объем задачи: " << operations << " operations\n";
    file.close();
}

vector<vector<int>> multiplyMatricesOpenMP(const vector<vector<int>>& A,
                                            const vector<vector<int>>& B,
                                            int size, int num_threads) {
    vector<vector<int>> C(size, vector<int>(size, 0));
    
    omp_set_num_threads(num_threads);
    
    #pragma omp parallel for
    for (int i = 0; i < size; i++) {
        for (int k = 0; k < size; k++) {
            int aik = A[i][k];
            for (int j = 0; j < size; j++) {
                C[i][j] += aik * B[k][j];
            }
        }
    }
    
    return C;
}

int main(int argc, char* argv[]) {
    string fileA = "matrix_a.txt";
    string fileB = "matrix_b.txt";
    string fileC = "result.txt";
    int num_threads = 1;
    
    if (argc >= 2) {
        num_threads = atoi(argv[1]);
    }
    
    int sizeA, sizeB;
    vector<vector<int>> A = readMatrix(fileA, sizeA);
    vector<vector<int>> B = readMatrix(fileB, sizeB);
    
    if (sizeA != sizeB) {
        cerr << "Error: matrix dimensions do not match" << "\n";
        return 1;
    }
    
    int size = sizeA;
    long long operations = 2LL * size * size * size;
    
    cout << "Size: " << size << "x" << size << "\n";
    cout << "Threads: " << num_threads << "\n";
    cout << "Operations: " << operations << "\n";
    
    auto start = high_resolution_clock::now();
    vector<vector<int>> C = multiplyMatricesOpenMP(A, B, size, num_threads);
    auto end = high_resolution_clock::now();
    
    auto duration = duration_cast<microseconds>(end - start);
    long long microseconds = duration.count();
    
    cout << "Time: " << microseconds << " mcs\n";
    
    writeMatrix(fileC, C, size, microseconds, operations, num_threads);
    
    return 0;
}