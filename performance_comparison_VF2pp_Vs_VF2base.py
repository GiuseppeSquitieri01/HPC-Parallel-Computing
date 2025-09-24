#!/usr/bin/env python3
#
# License: GNU GPLv3
#
# File derived and heavily modified from:
# https://github.com/kpetridis24/vf2-pp (MIT License)
# Original copyright (c) 2022 kpetridis24
# See LICENSE file in the repository for details.
#
# Student:
# Squitieri Giuseppe, 0622702339, g.squitieri8@studenti.unisa.it
#
# Assignment: Progetto VF2++ - Implementazione Seriale e Parallela MPI
#
# Course Lecturer: Moscato Francesco, fmoscato@unisa.it
#
# Assignment Requirements:
# - Progettare, realizzare e analizzare una versione parallela di VF2-pp basata su MPI
# - Confrontare i risultati con l’algoritmo originale
# - Documentazione e codice secondo le istruzioni sulla piattaforma e-learning
#

# Assignment Description:
# Qui trova la descrizione di un algoritmo (VF2++) per il controllo di isomorfismo tra grafi.
#
# https://github.com/MiviaLab/vf2lib (algoritmo originale)
# https://github.com/pfllo/VF2 (versione semplificata dell'algoritmo originale)
# https://github.com/kpetridis24/vf2-pp (vf2-++ con benchmark: il benchmark va utilizzato per confrontare i risultati con quelli dell'algoritmo originale)
#
# Progetti, Realizzi e Analizzi una versione parallela di VF2-pp basata su MPI
import time
import os
import subprocess
import shutil
import csv
import matplotlib.pyplot as plt
import networkx as nx
import sys

def save_graph_custom(G, path, name):
    N = G.number_of_nodes()
    M = G.number_of_edges()
    os.makedirs(path, exist_ok=True)
    file_path = os.path.join(path, name)
    with open(file_path, "w") as f:
        f.write(f"{N} {M}\n")
        for node in sorted(G.nodes()):
            neighbors = sorted(list(G.neighbors(node)))
            if neighbors:
                neighbors_str = " ".join(map(str, neighbors))
                f.write(f"{node}\t{neighbors_str}\n")
            else:
                f.write(f"{node}\n")
    return file_path

def vf2pp_c_is_isomorphic(G1, G2, outdir, exec_path, tag):
    file1 = save_graph_custom(G1, outdir, f"{tag}_G1.txt")
    file2 = save_graph_custom(G2, outdir, f"{tag}_G2.txt")

    if not os.path.isabs(exec_path):
        exec_path = os.path.abspath(exec_path)

    if not os.path.isfile(exec_path):
        print(f" VF2++ executable not found: {exec_path}")
        try:
            os.remove(file1)
            os.remove(file2)
        except OSError:
            pass
        return False, 0.0

    try:
        result = subprocess.run([exec_path,
                                 os.path.basename(file1),
                                 os.path.basename(file2)],
                                cwd=outdir, capture_output=True, text=True)
        output = result.stdout.strip() if result.stdout else result.stderr.strip()

        if result.returncode != 0:
            print(f" VF2++ Error (returncode {result.returncode}): {result.stderr.strip()}")
            return False, 0.0

        algorithm_time_file = os.path.join(outdir, "algorithm_time.txt")
        dur = 0.0
        if os.path.exists(algorithm_time_file):
            try:
                with open(algorithm_time_file, 'r') as f:
                    lines = f.read().strip().split('\n')
                
                if len(lines) >= 2:
                    is_iso = bool(int(lines[0]))
                    dur = float(lines[1])
                else:
                    print(f" Warning: algorithm_time.txt has insufficient lines: {len(lines)}")
                    is_iso = False
                    dur = 0.0
                    
                os.remove(algorithm_time_file)
            except (IOError, ValueError) as e:
                print(f" Warning: Could not read algorithm time file: {e}")
                is_iso = False
                dur = 0.0
        else:
            print(f" Warning: algorithm_time.txt not found in {outdir}")
            is_iso = 'isomorph' in output.lower()
            dur = 0.0
        return is_iso, dur

    except subprocess.TimeoutExpired:
        print("VF2++ timed out")
        return False, 0.0
    except Exception as e:
        print(f"Errore nell'esecuzione: {e}")
        return False, 0.0
    finally:
        try:
            if os.path.exists(file1):
                os.remove(file1)
            if os.path.exists(file2):
                os.remove(file2)
        except OSError:
            pass

class VF2VF2ppComparison:
    def __init__(self, number_of_nodes, outdir="/tmp", exec_path=None):
        self.number_of_nodes = number_of_nodes
        self.outdir = outdir
        if exec_path is None:
            self.exec_path = "Compilati_Serial/VF2pp_O3"
        else:
            self.exec_path = exec_path
        self.vf2_times = []
        self.vf2pp_times = []
        self.results_data = []

    def reset(self):
        self.vf2pp_times.clear()
        self.vf2_times.clear()

    def cleanup_temp_files(self):
        try:
            if os.path.exists(self.outdir):
                for file in os.listdir(self.outdir):
                    if file.endswith('.txt') and any(tag in file for tag in ['random_', 'disconnected_', 'complete_']):
                        os.remove(os.path.join(self.outdir, file))
                print(f"Cleaned test graph files from {self.outdir}")
        except OSError as e:
            print(f"Warning: Could not clean temp files: {e}")

    def plot_times(self, graph_type="random", save_dir=None):
        plt.figure(figsize=(10, 6))
        plt.plot(self.number_of_nodes, self.vf2_times, label="NetworkX VF2",
                 linestyle="dashed", marker='o')
        plt.plot(self.number_of_nodes, self.vf2pp_times, label="VF2++", marker='s')
        plt.xlabel("Number of nodes")
        plt.ylabel("Time (seconds)")
        plt.title(f"Performance Comparison - {graph_type.title()} Graphs")
        plt.legend()
        plt.grid(True, alpha=0.3)
        if save_dir:
            filename = f"times_{graph_type}.png"
            filepath = os.path.join(save_dir, filename)
            plt.savefig(filepath, dpi=300, bbox_inches='tight')
            plt.close()
            print(f"Saved: {filename}")

    def plot_speedup(self, graph_type="random", save_dir=None):
        speedup = []
        for slow, fast in zip(self.vf2_times, self.vf2pp_times):
            if fast > 0:
                speedup.append(slow / fast)
            else:
                speedup.append(0.0)
        plt.figure(figsize=(10, 6))
        plt.plot(self.number_of_nodes, speedup, label="Speedup VF2++ vs NetworkX VF2",
                 marker='o', linewidth=2)
        plt.axhline(y=1, color='r', linestyle='--', alpha=0.7, label="No speedup (1x)")
        plt.xlabel("Number of nodes")
        plt.ylabel("Speedup (t_networkx / t_vf2pp)")
        plt.title(f"Speedup Analysis - {graph_type.title()} Graphs")
        plt.legend()
        plt.grid(True, alpha=0.3)
        if save_dir:
            filename = f"speedup_{graph_type}.png"
            filepath = os.path.join(save_dir, filename)
            plt.savefig(filepath, dpi=300, bbox_inches='tight')
            plt.close()
            print(f"Saved: {filename}")

    def save_results_to_csv(self, save_dir=None):
        if not save_dir:
            save_dir = self.outdir
        csv_filename = os.path.join(save_dir, "benchmark_results.csv")
        with open(csv_filename, 'w', newline='', encoding='utf-8') as csvfile:
            fieldnames = [
                'graph_type', 'nodes', 'edges', 'probability', 'vf2pp_time',
                'vf2_time', 'speedup', 'vf2pp_result', 'vf2_result', 'results_match'
            ]
            writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
            writer.writeheader()
            for result in self.results_data:
                writer.writerow(result)
        print(f"Results saved to: {csv_filename}")
        return csv_filename

    def compare_in_random_graphs(self, prob=0.5):
        print(f"Testing Random Graphs (probability={prob})")
        for V in self.number_of_nodes:
            print(f"  Nodes: {V}")
            G1 = nx.gnp_random_graph(V, p=prob, seed=39)
            G2 = nx.gnp_random_graph(V, p=prob, seed=39)
            vf2pp_result, dur1 = vf2pp_c_is_isomorphic(G1, G2, self.outdir, self.exec_path, f"random_{V}")
            t0 = time.time()
            nx_result = nx.is_isomorphic(G1, G2)
            dur2 = time.time() - t0
            self.vf2pp_times.append(dur1)
            self.vf2_times.append(dur2)
            speed = dur2 / dur1 if dur1 > 0 else 0
            self.results_data.append({
                'graph_type': 'Random',
                'nodes': V,
                'edges': G1.number_of_edges(),
                'probability': prob,
                'vf2pp_time': dur1,
                'vf2_time': dur2,
                'speedup': speed,
                'vf2pp_result': vf2pp_result,
                'vf2_result': nx_result,
                'results_match': vf2pp_result == nx_result
            })
            vf2pp_iso = " Isomorphic" if vf2pp_result else " Not Isomorphic"
            nx_iso = "Isomorphic" if nx_result else " Not Isomorphic"
            consistent = "OK" if vf2pp_result == nx_result else " MISMATCH"
            speedup = speed if speed != 0 else float('inf') if dur1 == 0 and dur2 > 0 else speed
            print(f"    VF2++: {dur1:.4f}s ({vf2pp_iso}) | NetworkX: {dur2:.4f}s ({nx_iso})")
            print(f"    Speedup: {speedup:.1f}x | Results: {consistent}")
        print()

    def compare_in_disconnected_graphs(self):
        print(" Testing Disconnected Graphs")
        for V in self.number_of_nodes:
            print(f"  Nodes: {V}")
            G1 = nx.Graph(); G2 = nx.Graph()
            G1.add_nodes_from(range(V)); G2.add_nodes_from(range(V))
            for n in G1.nodes():
                G1.nodes[n]["label"] = "blue"
                G2.nodes[n]["label"] = "blue"
            vf2pp_result, dur1 = vf2pp_c_is_isomorphic(G1, G2, self.outdir, self.exec_path, f"disconnected_{V}")
            t0 = time.time()
            nx_result = nx.is_isomorphic(G1, G2)
            dur2 = time.time() - t0
            self.vf2pp_times.append(dur1)
            self.vf2_times.append(dur2)
            speed = dur2 / dur1 if dur1 > 0 else 0
            self.results_data.append({
                'graph_type': 'Disconnected',
                'nodes': V,
                'edges': G1.number_of_edges(),
                'probability': 'N/A',
                'vf2pp_time': dur1,
                'vf2_time': dur2,
                'speedup': speed,
                'vf2pp_result': vf2pp_result,
                'vf2_result': nx_result,
                'results_match': vf2pp_result == nx_result
            })
            vf2pp_iso = " Isomorphic" if vf2pp_result else " Not Isomorphic"
            nx_iso = " Isomorphic" if nx_result else " Not Isomorphic"
            consistent = "OK" if vf2pp_result == nx_result else " MISMATCH"
            speedup = speed if speed != 0 else float('inf') if dur1 == 0 and dur2 > 0 else speed
            print(f"    VF2++: {dur1:.4f}s ({vf2pp_iso}) | NetworkX: {dur2:.4f}s ({nx_iso})")
            print(f"    Speedup: {speedup:.1f}x | Results: {consistent}")
        print()

    def compare_in_complete_graphs(self):
        print(" Testing Complete Graphs")
        for V in self.number_of_nodes:
            print(f"  Nodes: {V}")
            G1 = nx.complete_graph(V); G2 = nx.complete_graph(V)
            for n in G1.nodes():
                G1.nodes[n]["label"] = "blue"
                G2.nodes[n]["label"] = "blue"
            vf2pp_result, dur1 = vf2pp_c_is_isomorphic(G1, G2, self.outdir, self.exec_path, f"complete_{V}")
            t0 = time.time()
            nx_result = nx.is_isomorphic(G1, G2)
            dur2 = time.time() - t0
            self.vf2pp_times.append(dur1)
            self.vf2_times.append(dur2)
            speed = dur2 / dur1 if dur1 > 0 else 0
            self.results_data.append({
                'graph_type': 'Complete',
                'nodes': V,
                'edges': G1.number_of_edges(),
                'probability': 'N/A',
                'vf2pp_time': dur1,
                'vf2_time': dur2,
                'speedup': speed,
                'vf2pp_result': vf2pp_result,
                'vf2_result': nx_result,
                'results_match': vf2pp_result == nx_result
            })
            vf2pp_iso = " Isomorphic" if vf2pp_result else " Not Isomorphic"
            nx_iso = " Isomorphic" if nx_result else " Not Isomorphic"
            consistent = "OK" if vf2pp_result == nx_result else " MISMATCH"
            speedup = speed if speed != 0 else float('inf') if dur1 == 0 and dur2 > 0 else speed
            print(f"    VF2++: {dur1:.4f}s ({vf2pp_iso}) | NetworkX: {dur2:.4f}s ({nx_iso})")
            print(f"    Speedup: {speedup:.1f}x | Results: {consistent}")
        print()

if __name__ == "__main__":
    if len(sys.argv) >= 4:
        graph_min = int(sys.argv[1])
        graph_max = int(sys.argv[2])
        graph_step = int(sys.argv[3])
        node_range = range(graph_min, graph_max, graph_step)
        print(f" Using custom graph parameters: {graph_min} to {graph_max-1} step {graph_step}")
    else:
        node_range = range(0, 501, 100)
        print(f" Using default graph parameters: 0 to 500 step 100")
    base_serial = "Compilati_Serial"
    executables = [
        os.path.join(base_serial, "VF2pp_O0"),
        os.path.join(base_serial, "VF2pp_O1"),
        os.path.join(base_serial, "VF2pp_O2"),
        os.path.join(base_serial, "VF2pp_O3")
    ]

    folders = [
        "Test_Result/vf2pp_vs_vf2/diagrammi_vf2pp_vs_vf2_O0",
        "Test_Result/vf2pp_vs_vf2/diagrammi_vf2pp_vs_vf2_O1",
        "Test_Result/vf2pp_vs_vf2/diagrammi_vf2pp_vs_vf2_O2",
        "Test_Result/vf2pp_vs_vf2/diagrammi_vf2pp_vs_vf2_O3"
    ]

    debug_dir = "grafi_prove"

    print(" VF2++ Multi-Optimization Benchmark")
    print("=" * 50)

    for i in range(4):
        exec_path = executables[i]
        diagrams_dir = folders[i]
        print(f"\n [{i+1}/4] Testing with O{i} optimization")
        print(f" Executable: {exec_path}")
        print(f" Output dir: {diagrams_dir}")

        if os.path.exists(diagrams_dir):
            shutil.rmtree(diagrams_dir)
            print(f" Removed old directory")
        os.makedirs(diagrams_dir, exist_ok=True)
        print(f" Created directory: {diagrams_dir}")

        try:
            benchmark = VF2VF2ppComparison(node_range, outdir=debug_dir, exec_path=exec_path)

            benchmark.compare_in_random_graphs()
            benchmark.plot_times("random", diagrams_dir)
            benchmark.plot_speedup("random", diagrams_dir)
            benchmark.cleanup_temp_files()

            benchmark.reset()
            benchmark.compare_in_disconnected_graphs()
            benchmark.plot_times("disconnected", diagrams_dir)
            benchmark.plot_speedup("disconnected", diagrams_dir)
            benchmark.cleanup_temp_files()

            benchmark.reset()
            benchmark.compare_in_complete_graphs()
            benchmark.plot_times("complete", diagrams_dir)
            benchmark.plot_speedup("complete", diagrams_dir)
            benchmark.cleanup_temp_files()

            csv_file = benchmark.save_results_to_csv(diagrams_dir)

            print(f" Completed O{i} optimization!")
            print(f" Diagrams: {diagrams_dir}")
            print(f" CSV: {csv_file}")

        except Exception as e:
            print(f" Error with O{i}: {e}")
            continue
        finally:
            try:
                if os.path.exists(debug_dir):
                    for file in os.listdir(debug_dir):
                        if file.endswith(('.txt', '.tmp')):
                            os.remove(os.path.join(debug_dir, file))
                    print(f" Cleaned temporary files from {debug_dir}")
            except OSError:
                pass

    print("\n ALL BENCHMARKS COMPLETED!")
    print(" Check the diagrammi_O0, diagrammi_O1, diagrammi_O2, diagrammi_O3 folders")