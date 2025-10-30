import numpy as np
import time
import tracemalloc
from typing import Callable, Tuple, Dict
import matplotlib.pyplot as plt
from matplotlib.gridspec import GridSpec
import os


def count_operations_moore_penrose(A: np.ndarray, eps: float = 1e-6, delta: float = 100) -> int:
    m, n = A.shape
    operations = 0
    
    operations += m * m * n
    operations += m
    operations += m * m
    operations += m * m * m
    operations += n * m * m
    
    max_iterations = int(np.log2(delta / eps)) + 1
    
    for _ in range(max_iterations):
        operations += m * m * n
        operations += m
        operations += m * m
        operations += m * m * m
        operations += n * m * m
        operations += n * m
    
    return operations


def count_operations_greville(A: np.ndarray, eps: float = 1e-6) -> int:
    m, n = A.shape
    operations = 0
    
    operations += n
    operations += n
    
    for i in range(1, m):
        current_size = i + 1
        operations += n * current_size * current_size
        operations += n * current_size
        operations += n * current_size
        operations += current_size
        operations += n * current_size
        operations += current_size * n
        operations += n * n
        operations += n * (current_size + 1)
        operations += n
    
    return operations


def measure_time(func: Callable, *args, **kwargs) -> Tuple[any, float]:
    start_time = time.perf_counter()
    result = func(*args, **kwargs)
    end_time = time.perf_counter()
    execution_time = end_time - start_time
    return result, execution_time


def measure_memory(func: Callable, *args, **kwargs) -> Tuple[any, float]:
    tracemalloc.start()
    result = func(*args, **kwargs)
    current, peak = tracemalloc.get_traced_memory()
    tracemalloc.stop()
    memory_mb = peak / 1024 / 1024
    return result, memory_mb


def calculate_rmse(original: np.ndarray, reconstructed: np.ndarray) -> float:
    mse = np.mean((original - reconstructed) ** 2)
    rmse = np.sqrt(mse)
    return rmse


def normalize_image(img: np.ndarray) -> np.ndarray:
    normalized = (img - img.min()) / (img.max() - img.min())
    return (normalized * 255).astype('uint8')


def compare_and_visualize(X: np.ndarray, Y: np.ndarray, pim_mp: Callable, pim_greville: Callable, eps: float = 1e-6, delta: float = 1000):
    
    print("Computing Moore-Penrose method...")
    ops_mp = count_operations_moore_penrose(X, eps, delta)
    X_pi_mp, time_mp = measure_time(pim_mp, X, eps, delta)
    X_pi_mp_mem, memory_mp = measure_memory(pim_mp, X, eps, delta)
    
    A_mp = Y @ X_pi_mp
    Y_reconstructed_mp = A_mp @ X
    rmse_mp = calculate_rmse(Y, Y_reconstructed_mp)
    
    print("Computing Greville method...")
    ops_gr = count_operations_greville(X, eps)
    X_pi_gr, time_gr = measure_time(pim_greville, X, eps, delta)
    X_pi_gr_mem, memory_gr = measure_memory(pim_greville, X, eps, delta)
    
    A_gr = Y @ X_pi_gr
    Y_reconstructed_gr = A_gr @ X
    rmse_gr = calculate_rmse(Y, Y_reconstructed_gr)
    
    Y_original_norm = normalize_image(Y)
    Y_mp_norm = normalize_image(Y_reconstructed_mp)
    Y_gr_norm = normalize_image(Y_reconstructed_gr)
    
    fig = plt.figure(figsize=(18, 10))
    gs = GridSpec(2, 4, figure=fig, hspace=0.35, wspace=0.35)
    
    ax1 = fig.add_subplot(gs[0, 0:2])
    ax1.imshow(Y_original_norm, cmap='gray')
    ax1.set_title('Original Image Y', fontsize=13, fontweight='bold')
    ax1.axis('off')
    
    ax2 = fig.add_subplot(gs[0, 2])
    ax2.imshow(Y_mp_norm, cmap='gray')
    ax2.set_title(f'Moore-Penrose\nRMSE: {rmse_mp:.4f}', fontsize=11, fontweight='bold')
    ax2.axis('off')
    
    ax3 = fig.add_subplot(gs[0, 3])
    ax3.imshow(Y_gr_norm, cmap='gray')
    ax3.set_title(f'Greville\nRMSE: {rmse_gr:.4f}', fontsize=11, fontweight='bold')
    ax3.axis('off')
    
    methods = ['Moore-Penrose', 'Greville']
    colors = ['#3498db', '#e74c3c']
    
    ax4 = fig.add_subplot(gs[1, 0])
    operations = [ops_mp, ops_gr]
    bars1 = ax4.bar(methods, operations, color=colors, alpha=0.7, edgecolor='black', linewidth=1.5)
    ax4.set_ylabel('Number of Operations', fontsize=10, fontweight='bold')
    ax4.set_title('Operations Count', fontsize=11, fontweight='bold')
    ax4.grid(axis='y', alpha=0.3, linestyle='--')
    for bar in bars1:
        height = bar.get_height()
        ax4.text(bar.get_x() + bar.get_width()/2., height,
                f'{int(height):,}', ha='center', va='bottom', fontsize=8, fontweight='bold')
    
    ax5 = fig.add_subplot(gs[1, 1])
    times = [time_mp, time_gr]
    bars2 = ax5.bar(methods, times, color=colors, alpha=0.7, edgecolor='black', linewidth=1.5)
    ax5.set_ylabel('Time (seconds)', fontsize=10, fontweight='bold')
    ax5.set_title('Execution Time', fontsize=11, fontweight='bold')
    ax5.grid(axis='y', alpha=0.3, linestyle='--')
    for bar in bars2:
        height = bar.get_height()
        ax5.text(bar.get_x() + bar.get_width()/2., height,
                f'{height:.4f}', ha='center', va='bottom', fontsize=8, fontweight='bold')
    
    ax6 = fig.add_subplot(gs[1, 2])
    memory = [memory_mp, memory_gr]
    bars3 = ax6.bar(methods, memory, color=colors, alpha=0.7, edgecolor='black', linewidth=1.5)
    ax6.set_ylabel('Memory (MB)', fontsize=10, fontweight='bold')
    ax6.set_title('Memory Usage', fontsize=11, fontweight='bold')
    ax6.grid(axis='y', alpha=0.3, linestyle='--')
    for bar in bars3:
        height = bar.get_height()
        ax6.text(bar.get_x() + bar.get_width()/2., height,
                f'{height:.2f}', ha='center', va='bottom', fontsize=8, fontweight='bold')
    
    ax7 = fig.add_subplot(gs[1, 3])
    rmse = [rmse_mp, rmse_gr]
    bars4 = ax7.bar(methods, rmse, color=colors, alpha=0.7, edgecolor='black', linewidth=1.5)
    ax7.set_ylabel('RMSE', fontsize=10, fontweight='bold')
    ax7.set_title('Methods RMSE', fontsize=11, fontweight='bold')
    ax7.grid(axis='y', alpha=0.3, linestyle='--')
    for bar in bars4:
        height = bar.get_height()
        ax7.text(bar.get_x() + bar.get_width()/2., height,
                f'{height:.6f}', ha='center', va='bottom', fontsize=8, fontweight='bold')
    
    fig.suptitle('COMPARISON: MOORE-PENROSE vs GREVILLE METHOD', 
                 fontsize=15, fontweight='bold', y=0.98)
    save_path = os.path.join("results", "comparison_plot.png")
    fig.savefig(save_path, dpi=300, bbox_inches='tight')
    
    plt.show()