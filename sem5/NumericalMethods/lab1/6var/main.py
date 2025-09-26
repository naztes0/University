import numpy as np 

def f(x):
    """Function f(x) = x^3 + 3x^2 - x - 3"""
    return x**3 + 3*x**2 - x - 3

def df(x):
    return 3*x**2 + 6*x - 1

def d2f(x):
    return 6*x + 6
def has_root(func, a, b, num_points=1000) -> tuple:
    """
    Найпростіший спосіб: ділимо інтервал на частини і шукаємо першу зміну знаку
    Returns: (has_root: bool, left_bound: float, right_bound: float)
    """
    x_vals = np.linspace(a, b, num_points)
    
    for i in range(len(x_vals) - 1):
        f1 = func(x_vals[i])
        f2 = func(x_vals[i + 1])
        
        # Якщо знаки різні - є корінь
        if f1 * f2 < 0:
            return True, x_vals[i], x_vals[i + 1]
    
    # Повертаємо оригінальні границі, якщо корінь не знайдено
    return False, a, b
def extrema_of_derivative(df, a, b, num_points=1000):
    """
    Find m1 = min |f'(x)| and M1 = max |f'(x)| on [a, b].
    """
    x_vals = np.linspace(a, b, num_points)
    y_vals = np.abs([df(x) for x in x_vals])
    return np.min(y_vals), np.max(y_vals)

def find_aprior_estimate(x0_distance, eps, q0):
    """Calculate a priori estimate for number of iterations"""
    if q0 >= 1:
        return float('inf')  # Method won't converge
    return int(np.floor(np.log(x0_distance / eps) / np.log(1 / q0)) + 1)

def relaxation_method(f, df, a, b, eps, max_iterations=1000):

    has_root_result, a, b = has_root(f, a, b)
    
    if not has_root_result:
        print(f"No root found in interval [{a}, {b}]")
        return None, 0
    

    # If derivative changes sign, split interval
    if has_root(df, a, b):
        c = (a + b) / 2
        sub_intervals = [(a, c), (c, b)]
        
        for interval_start, interval_end in sub_intervals:
            if has_root(f, interval_start, interval_end):
                print(f"Splitting interval. Working with [{interval_start}, {interval_end}]")
                return relaxation_method(f, df, interval_start, interval_end, eps, max_iterations)
    
    x0 = (a + b) / 2
    
    min_df, max_df = extrema_of_derivative(df, a, b)
    
    # Check convergence condition
    if min_df == 0:
        print("Warning: Derivative has zero value in the interval")
        return None, 0
    
    tau = 2 / (max_df + min_df)
    if df(x0) > 0:
        tau = -tau
    
    q0 = (max_df - min_df) / (max_df + min_df)
    
    print(f"Convergence factor q0 = {q0}")
    print(f"Relaxation parameter tau = {tau}")
    
    if q0 >= 1:
        print("Warning: Method may not converge (q0 >= 1)")
        return None, 0
    
    x0_distance = max(b - x0, x0 - a)
    aprior_estimate = find_aprior_estimate(x0_distance, eps, q0)
    print(f"A priori estimate: {aprior_estimate}")
    
    # Iterative process
    iterations = 0
    x_current = x0
    
    print(f"\nStarting iterations with x0 = {x0}")
    print("-" * 60)
    
    while iterations < max_iterations:
         # Relaxation formula: x + tau*f(x)
        x_next = x_current + tau * f(x_current) 
        iterations += 1
        
        print(f"Iteration {iterations:2d}: x = {x_current:.6f}, f(x) = {f(x_current):8.6f}, x_next = {x_next:.6f}")
        
        # Check convergence
        if abs(x_current - x_next) <= eps:
            print(f"\nConverged after {iterations} iterations")
            return x_next, iterations
        
        x_current = x_next
    
    print(f"Maximum iterations ({max_iterations}) reached without convergence")
    return x_current, iterations

def newton_method(f, df, x0, eps, max_iterations=1000):
  
    print(f"\nNewton's method starting with x0 = {x0}")
    print("-" * 60)
    
    x_current = x0
    iterations = 0
    
    while iterations < max_iterations:
        f_val = f(x_current)
        df_val = df(x_current)
        
        if abs(df_val) < 1e-15:
            print("Error: Derivative is too close to zero")
            return None, iterations
        
        x_next = x_current - f_val / df_val
        iterations += 1
        
        print(f"Iteration {iterations:2d}: x = {x_current:.6f}, f(x) = {f_val:8.6f}, f'(x) = {df_val:8.6f}, x_next = {x_next:.6f}")
        
        if abs(x_current - x_next) <= eps:
            print(f"\nConverged after {iterations} iterations")
            return x_next, iterations
        
        x_current = x_next
    
    print(f"Maximum iterations ({max_iterations}) reached without convergence")
    return x_current, iterations

def main():
    eps = 1e-3
    
    print("=" * 80)
    print("NUMERICAL METHODS FOR SOLVING NONLINEAR EQUATIONS")
    print("=" * 80)
    print(f"Function: f(x) = x³ + 3x² - x - 3")
    print(f"Precision: ε = {eps}")
    print()
    
    # Relaxation method
    print("RELAXATION METHOD")
    print("=" * 40)
    x_relax, iter_relax = relaxation_method(f, df, -4, -1, eps)  
    
    if x_relax is not None:
        print(f"\nResult: x* = {x_relax:.6f}")
        print(f"Verification: f(x*) = {f(x_relax):.6f}")
        print(f"A posteriori estimate: {iter_relax}")
    
    print("\n" + "=" * 80)
    
    # # Newton's method
    # print("NEWTON'S METHOD")
    # print("=" * 40)
    # x_newton, iter_newton = newton_method(f, df, 1.25, eps)  
    
    # if x_newton is not None:
    #     print(f"\nResult: x* = {x_newton:.6f}")
    #     print(f"Verification: f(x*) = {f(x_newton):.6f}")
    #     print(f"Number of iterations: {iter_newton}")
    
    # print("\n" + "=" * 80)
    # print("COMPARISON")
    # print("=" * 80)
    # if x_relax is not None and x_newton is not None:
    #     print(f"Relaxation method: {iter_relax} iterations, x* = {x_relax:.6f}")
    #     print(f"Newton's method:   {iter_newton} iterations, x* = {x_newton:.6f}")
    #     print(f"Difference in roots: {abs(x_relax - x_newton):.6f}")

if __name__ == "__main__":
    main()