import subprocess
import matplotlib.pyplot as plt
import os

# Konfiguracja testów [cite: 6]
step_counts = [100000000, 1000000000, 3000000000] # 100M, 1B, 3B
max_threads = 20 # Zmieñ na 50 zgodnie z[cite: 6], jeœli masz tyle rdzeni logicznych
cpp_executable = "./main" if os.name != 'nt' else "main.exe"

# Kompilacja C++ (wymaga g++)
print("Kompilacja programu C++...")
compile_cmd = ["g++", "-O3", "-pthread", "main.cpp", "-o", "main"]
subprocess.run(compile_cmd, check=True)

results = {} # S³ownik do przechowywania wyników: {steps: {threads: time}}

print("Rozpoczynanie testów...")
for steps in step_counts:
    results[steps] = {'x': [], 'y': []}
    print(f"Testowanie dla {steps} kroków...")
    
    for threads in range(1, max_threads + 1):
        # Uruchomienie programu C++ z parametrami
        process = subprocess.run([cpp_executable, str(steps), str(threads)], 
                                 capture_output=True, text=True)
        
        # Odczyt wyjœcia (zak³adamy format: "CZAS WYNIK")
        output = process.stdout.strip().split()
        if output:
            time_taken = float(output[0])
            results[steps]['x'].append(threads)
            results[steps]['y'].append(time_taken)
            print(f"  W¹tki: {threads}, Czas: {time_taken:.4f}s")

# Rysowanie wykresu [cite: 9]
plt.figure(figsize=(10, 6))
for steps, data in results.items():
    plt.plot(data['x'], data['y'], marker='o', label=f'Kroki: {steps}')

plt.title('Zale¿noœæ czasu obliczeñ od liczby w¹tków')
plt.xlabel('Liczba w¹tków')
plt.ylabel('Czas wykonania [s]')
plt.grid(True)
plt.legend()
plt.savefig("wykres_wydajnosci.png")
plt.show()
print("Wykres zapisano jako wykres_wydajnosci.png")