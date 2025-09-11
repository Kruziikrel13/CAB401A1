#include <chrono>
#include <cstdio>
#include <cstdlib>

int main() {
    const int N = 1024;

    int       A[N][N], B[N][N], C[N][N];

    srand(42);

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            A[i][j] = rand();
            B[i][j] = rand();
        }
    }

    printf("Multiplying matrices of size %dx%d\n", N, N);

    auto start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            int total = 0;
            for (int k = 0; k < N; k++) {
                total += A[i][k] * B[k][j];
            }
            C[i][j] = total;
        }
    }
    auto                          end_time = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> duration = end_time - start_time;

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d\n ", C[i][j]);
        }
    }

    printf("%f seconds\n", duration.count());
    return 0;
}
