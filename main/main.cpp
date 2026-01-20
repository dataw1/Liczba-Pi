/**
 * @file main.cpp
 * @brief Program obliczajacy przyblizona wartosc liczby PI metoda calkowania numerycznego.
 * @details Program wykorzystuje metode prostokatow do obliczenia calki z funkcji 4/(1+x^2).
 * Wykorzystuje wielowatkowosc (std::thread) do rownoleglego wykonywania obliczen.
 * Program jest przystosowany do uruchamiania przez skrypt automatyzujacy w Pythonie.
 *
 * @author Twoje Imie
 * @version 1.0
 */

#include <iostream>
#include <vector>
#include <thread>
#include <functional>
#include <chrono>
#include <iomanip>
#include <string>

 /**
  * @brief Funkcja matematyczna, ktorej calke liczymy.
  * @details Oblicza wartosc f(x) = 4 / (1 + x^2).
  *
  * @param x Wartosc wejsciowa.
  * @return double Wynik funkcji dla x.
  */
double f(double x) {
    return 4.0 / (1.0 + x * x);
}

/**
 * @brief Glowna funkcja programu.
 * @details Pobiera argumenty wiersza polecen, uruchamia watki liczace,
 * mierzy czas wykonania i wypisuje wynik na standardowe wyjscie.
 *
 * @param argc Liczba argumentow wiersza polecen.
 * @param argv Tablica argumentow wiersza polecen.
 * argv[1]: Liczba krokow calkowania (long long).
 * argv[2]: Liczba watkow (int).
 * @return int Kod wyjscia programu (0 oznacza sukces).
 */
int main(int argc, char* argv[]) {
    long long num_steps = 100000000; /**< Domyslna liczba krokow calkowania */
    int num_threads = 1;             /**< Domyslna liczba watkow */

    // Obsluga argumentow wiersza polecen (dla skryptu Pythona)
    if (argc >= 2) num_steps = std::stoll(argv[1]);
    if (argc >= 3) num_threads = std::stoi(argv[2]);

    double step = 1.0 / (double)num_steps;
    std::vector<std::thread> threads;
    std::vector<double> partial_sums(num_threads, 0.0);

    auto start_time = std::chrono::high_resolution_clock::now();

    /**
     * @brief Funkcja lambda wykonywana przez kazdy watek.
     * @details Oblicza sume czesciowa calki dla przydzielonego zakresu krokow.
     * Stosuje podzial blokowy pracy miedzy watkami.
     *
     * @param thread_id Unikalny identyfikator watku (od 0 do num_threads-1).
     */
    auto worker = [&](int thread_id) {
        double local_sum = 0.0;
        // Obliczenie indeksu poczatkowego i koncowego dla danego watku
        long long start_index = (num_steps / num_threads) * thread_id;
        long long end_index = (num_steps / num_threads) * (thread_id + 1);

        // Obsluga reszty dla ostatniego watku (musi doliczyc do konca)
        if (thread_id == num_threads - 1) {
            end_index = num_steps;
        }

        // Glowna petla obliczeniowa
        for (long long i = start_index; i < end_index; ++i) {
            double x = (i + 0.5) * step; // Srodek przedzialu
            local_sum += f(x);
        }
        partial_sums[thread_id] = local_sum;
        };

    // Tworzenie i uruchamianie watkow
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(worker, i);
    }

    // Oczekiwanie na zakonczenie wszystkich watkow (join)
    for (auto& t : threads) {
        t.join();
    }

    // Sumowanie wynikow czastkowych ze wszystkich watkow
    double total_sum = 0.0;
    for (double s : partial_sums) {
        total_sum += s;
    }
    double pi = total_sum * step;

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;

    // Format wyjscia: Czas Wynik
    // Ten format jest parsowany przez skrypt w Pythonie
    std::cout << elapsed.count() << " " << std::setprecision(15) << pi << std::endl;

    return 0;
}