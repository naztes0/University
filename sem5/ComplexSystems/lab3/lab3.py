import numpy as np
import time
import os 
import csv
from datetime import datetime
import matplotlib.pyplot as plt

def read_file(file_name):
    with open(file_name, 'r') as file:
        lines = file.readlines()
    data = []
    for line in lines:
        parts = line.strip().split()
        numbers = [float(x) for x in parts]
        data.append(numbers)
    matrix = np.array(data)
    return matrix.T

def ensure_logdir():
    if not os.path.exists("logs"):
        os.makedirs("logs")

def model_matr(p):
    #print(params)
    c1, c2, c3, c4 = p["c1"], p["c2"], p["c3"], p["c4"]
    m1, m2, m3 = p["m1"], p["m2"], p["m3"]
    
    matrix = [[0, 1, 0, 0, 0, 0],
              [-(c2+c1)/m1, 0, c2/m1, 0, 0, 0],
              [0, 0, 0, 1, 0, 0],
              [c2/m2, 0, -(c2+c3)/m2, 0, c3/m2, 0],
              [0, 0, 0, 0, 0, 1],
              [0, 0, c3/m3, 0, -(c4+c3)/m3, 0]]
    return np.array(matrix)


def finite_diff(y_func, betas, delta=1e-5):
    keys=np.array(list(betas.keys()))
    # print("keys: ", keys)
    n=len(y_func(betas))
    m=len(keys)
    deriv_matrix=np.zeros((n,m))
    for k in range(m):
        orig_value=betas[keys[k]]
        betas[keys[k]]= orig_value + delta
        y_plus=y_func(betas)
        betas[keys[k]]= orig_value - delta
        y_minus=y_func(betas)
        betas[keys[k]]= orig_value
        deriv_matrix[:,k]=(y_plus-y_minus)/(2*delta)
    return deriv_matrix



def get_U(A, partial_der,U, h):
    pd = np.array(partial_der)
    k1 = h * (np.dot(A, U) + pd)
    k2 = h * (np.dot(A, U + k1 / 2) + pd)
    k3 = h * (np.dot(A, U + k2 / 2) + pd)
    k4 = h * (np.dot(A, U + k3) + pd)
    return U + (k1 + 2 * k2 + 2 * k3 + k4) / 6

def get_y(A, y_aprox, h):
    k1 = h * np.dot(A, y_aprox)
    k2 = h * np.dot(A, y_aprox + k1 / 2)
    k3 = h * np.dot(A, y_aprox + k2 / 2)
    k4 = h * np.dot(A, y_aprox + k3)
    return y_aprox + (k1 + 2 * k2 + 2 * k3 + k4) / 6

def identify_parameters(y_matrix, params, betas, eps=1e-5,h=0.2):
    start_time=time.time()
    now= datetime.now()
    betas=betas.copy()
    all_params={**params, **betas}
    # print("betas:",betas)
    A_matr=model_matr(all_params)
    #print(A_matr)

    beta_results = np.array([b for b in betas.values()],dtype=float)
    # print("beta results", beta_results)

    iteration=0
    while True:
        all_params.update(betas)
        A_iter=model_matr(all_params)

        U = np.zeros((6, 3))
        accuracy = 0
        inv_integral = np.zeros((len(betas), len(betas)))
        integral_aprox= np.zeros(len(betas))
        y_approximation = y_matrix[0]

        for i in range (len(y_matrix)):
            y_vector = lambda b_values: model_matr({**all_params, **b_values}) @ y_approximation
            partial_der_matrix=finite_diff(y_vector, betas)
            inv_integral += U.T @ U
            integral_aprox += U.T@(y_matrix[i]-y_approximation)
            accuracy += (y_matrix[i]-y_approximation).T @ (y_matrix[i]-y_approximation)

            U = get_U(A_iter, partial_der_matrix,U, h)
            y_approximation = get_y(A_iter,y_approximation,h)
 
        inv_integral*=h
        integral_aprox*=h
        accuracy*=h


        delta_beta = np.linalg.inv(inv_integral) @ integral_aprox.flatten()
        beta_results += delta_beta

        
        for idx, key in enumerate(betas.keys()):
            betas[key] = beta_results[idx]
     
        
        print(f"Accuracy at {iteration} iteration: ",accuracy )
        if accuracy < eps:
            end_time=time.time()
            execution_time=end_time-start_time
            ensure_logdir()
            now_str=now.strftime("%Y-%m-%d_%H-%M-%S")
            summary= os.path.join("logs",f"summary_{now_str}.txt")
            with open(summary, "w") as f:
                f.write("Identification Results:\n")
                f.write("-"*25)
                f.write("\n\nIdentified parameters:\n")
                for param, value in betas.items():
                    f.write(f"{param}: {value:.6f}\n")
                f.write(f"\nQuality indicator: {accuracy:.6e}\n")
                f.write(f"Total ite rations: {iteration + 1}\n")
                f.write(f"Execution time: {execution_time:.2f} seconds\n")

            return betas, eps, iteration+1, execution_time
        iteration+=1      
def plot_model_results(measured_data, model_data, t_points, save_prefix):
    ensure_logdir()
    variables = ['x₁', 'dx₁/dt', 'x₂', 'dx₂/dt', 'x₃', 'dx₃/dt']
    fig, axes = plt.subplots(3, 2, figsize=(15, 12))
    axes = axes.flatten()
    
    for i, ax in enumerate(axes):
        ax.plot(t_points, measured_data[:, i], 'r.', label='Measured', markersize=4)
        ax.plot(t_points, model_data[:, i], 'b-', label='Model')
        ax.set_title(f'Variable {variables[i]}')
        ax.set_xlabel('Time')
        ax.set_ylabel('Value')
        ax.grid(True)
        ax.legend()
    
    plt.tight_layout()
    plot_path = os.path.join("logs", f"{save_prefix}.png")
    plt.savefig(plot_path)
    plt.close()
def get_model_solution(params, y0, t_points, h=0.2):
    A_matrix = model_matr(params)
    y_current = y0
    y_solution = [y0]
    
    for _ in range(1, len(t_points)):
        y_current = get_y(A_matrix, y_current, h)
        y_solution.append(y_current)
    
    return np.array(y_solution)


def main():
    data=read_file("y2.txt")
    # print(data)
    time_points=np.arange(0,0.2*len(data),0.2)
    # print(time_points)

    params={
        "c1":0.14,
        "c2":0.3,
        "c3":0.2,
        "c4":0.12
    }
    start_aprox={
        "m1":10,
        "m2":18,
        "m3":15
    }

    result, eps, iter, exec_time = identify_parameters(data,params,start_aprox)

    print()
    print("="*30)
    print("IDENTIFIED PARAMETERS")
    print("="*30)

    print("Initialized parametrs:")
    for key, value in start_aprox.items():
        print(f"{key}: {value}")
    print("\nIdentified parameters:")
    for key, value in result.items():
        print(f"{key}: {value}")
    
    print("\nPerformance metrics:")
    print(f"Quality indicator: {eps:.6e}")
    print(f"Total iterations: {iter}")
    print(f"Execution time: {exec_time:.2f} seconds")
    print("="*30)
    print()
    final_params = {**params, **result}
    model_data = get_model_solution(final_params, data[0], time_points)
    timestamp=datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
    plot_model_results(data, model_data, time_points, f"result_{timestamp}")
    
if __name__ == "__main__":
    main()
