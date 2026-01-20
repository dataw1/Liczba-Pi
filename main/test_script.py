## @package test_script
#  @brief Skrypt automatyzujacy testy wydajnosciowe.
#  @details Skrypt kompiluje program C++, uruchamia go dla roznych parametrow
#  (liczba krokow i liczba watkow), zbiera czasy wykonania i generuje
#  wykres wydajnosci przy uzyciu biblioteki Matplotlib.
#
#  @author Twoje Imie
#  @version 1.0

import subprocess
import matplotlib.pyplot as plt
import os

# -*- coding: utf-8 -*-

## Lista liczby krokow do przetestowania (100M, 1B, 3B).
step_counts = [100000000, 1000000000, 3000000000] 

## Maksymalna liczba watkow do testowania.
max_threads = 50 

## Okreslenie nazwy pliku wykonywalnego w zaleznosci od systemu operacyjnego.
cpp_executable = "./main" if os.name != 'nt' else "main.exe"

## @brief Kompilacja kodu zrodlowego C++.
#  @details Uzywa g++ z optymalizacja (-O3) i obsluga watkow (pthread).
print("Kompilacja programu C++...")
compile_cmd = ["g++", "-O3", "-pthread", "main.cpp", "-o", "main"]
subprocess.run(compile_cmd, check=True)

## Slownik do przechowywania wynikow. Struktura: {kroki: {'x': [watki], 'y': [czasy]}}.
results = {} 

print("Rozpoczynanie testow...")

# Glowna petla testowa
for steps in step_counts:
    results[steps] = {'x': [], 'y': []}
    print(f"Testowanie dla {steps} krokow...")
    
    for threads in range(1, max_threads + 1):
        # Uruchomienie programu C++ z parametrami: kroki, watki
        process = subprocess.run([cpp_executable, str(steps), str(threads)], 
                                 capture_output=True, text=True)
        
        # Odczyt standardowego wyjscia z programu C++
        output = process.stdout.strip().split()
        if output:
            time_taken = float(output[0])
            results[steps]['x'].append(threads)
            results[steps]['y'].append(time_taken)
            print(f"  Watki: {threads}, Czas: {time_taken:.4f}s")

## @brief Generowanie i zapisywanie wykresu wydajnosci.
#  @details Uzywa Matplotlib do narysowania zaleznosci czasu od liczby watkow.
plt.figure(figsize=(10, 6))
for steps, data in results.items():
    plt.plot(data['x'], data['y'], marker='o', label=f'Kroki: {steps}')

plt.title('Zaleznosc czasu obliczen od liczby watkow')
plt.xlabel('Liczba watkow')
plt.ylabel('Czas wykonania [s]')
plt.grid(True)
plt.legend()
plt.savefig("wykres_wydajnosci.png")
plt.show()
print("Wykres zapisano jako wykres_wydajnosci.png")