#include <iostream>
#include <vector>
#include <thread>
#include <functional>
#include <chrono>
#include <iomanip>
#include <string>

// Funkcja, której całkę liczymy: 4 / (1 + x^2)
double f(double x) {
    return 4.0 / (1.0 + x * x);
}

int main(int argc, char* argv[]) {
    long long num_steps = 100000000; // Domyślna wartość
    int num_threads = 1;             // Domyślna wartość

    // Obsługa argumentów wiersza poleceń (dla skryptu Pythona)
    if (argc >= 2) num_steps = std::stoll(argv[1]);
    if (argc >= 3) num_threads = std::stoi(argv[2]);

    double step = 1.0 / (double)num_steps;
    std::vector<std::thread> threads;
    std::vector<double> partial_sums(num_threads, 0.0);

    auto start_time = std::chrono::high_resolution_clock::now();

    // Wyrażenie lambda dla wątku 
    auto worker = [&](int thread_id) {
        double local_sum = 0.0;
        // Cykliczny podział pracy (loop splitting) lub blokowy. Tu zastosujemy blokowy.
        long long start_index = (num_steps / num_threads) * thread_id;
        long long end_index = (num_steps / num_threads) * (thread_id + 1);

        // Obsługa reszty dla ostatniego wątku
        if (thread_id == num_threads - 1) {
            end_index = num_steps;
        }

        for (long long i = start_index; i < end_index; ++i) {
            double x = (i + 0.5) * step; // Środek przedziału
            local_sum += f(x);
        }
        partial_sums[thread_id] = local_sum;
        };

    // Tworzenie wątków
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(worker, i);
    }

    // Dołączanie wątków (join)
    for (auto& t : threads) {
        t.join();
    }

    // Sumowanie wyników cząstkowych
    double total_sum = 0.0;
    for (double s : partial_sums) {
        total_sum += s;
    }
    double pi = total_sum * step;

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;

    // Format wyjścia: Czas, Wynik (Ważne dla skryptu Python)
    std::cout << elapsed.count() << " " << std::setprecision(15) << pi << std::endl;

    return 0;
}