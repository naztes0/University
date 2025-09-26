import numpy as np

def has_root(f, a, b):
    f_a = f(a)
    f_b = f(b)
    # Check if root exists in current interval
    if f_a * f_b < 0: 
        print(f"Root found in interval [{a:.2f}, {b:.2f}]")
        return True, a, b
    
    # Stop subdivision if interval is too small
    if abs(b - a) < 1e-2: 
        print(f"No root found in interval [{a:.3f}, {b:.3f}] (interval too small)")
        return False, None, None
    
    # Subdivide interval
    c = (a + b) / 2
    
    # Check right half first
    right_interval = has_root(f, c, b)
    if right_interval[0]: 
        return right_interval
    
    # Check left half
    left_interval = has_root(f, a, c)
    if left_interval[0]: 
        return left_interval
    
    return False, None, None

def method_of_simple_iteration(f, phi, dphi, a, b, eps)-> tuple[bool, float, float, int]:

    # Find interval containing root
    print("\nFinding interval containing root")
    print("-" * 40)
    hasroot, a, b = has_root(f, a, b)
    
    if not hasroot:
        print("No root found in the given interval range!")
        return False, None, None
    
    print(f"Working interval: [{a:.2f}, {b:.2f}]")
    

    x0 = (a + b) / 2
    delta = max(abs(x0 - a), abs(b - x0))
    print(f"\nx0 = {x0:.3f}")
    print(f"delta = max(|x0-a|, |b-x0|) = {delta:.3f}")
    
    # Check convergence conditions
    print(f"\nChecking convergence conditions")
    # First condition: max|phi'(x)| <= q < 1
    interval = np.linspace(a, b, 1000)
    q = 0
    for i in interval:
        if abs(dphi(i)) > q:
            q = abs(dphi(i))
    
    if q >= 1:
        print(f" First condition failed: q = {q:.6f} >= 1")
        print("Choose another phi(x) function!")
        return False, None, None
    
    print(f"\nFirst condition satisfied: q = max|phi'(x)| = {q:.3f} < 1")
    
    # Second condition: |phi(x0)) - x0| <= (1-q)*delta
    phi_x0 = phi(x0) 
    second_condition = abs(phi_x0 - x0) <= (1 - q) * delta
    
    print(f"\nSecond condition: |phi(x0)) - x0| <= (1-q)*delta")
    print(f" |{phi_x0:.2f} - {x0:.2f}| ≤ (1-{q:.3f})·{delta:.3f}")
    print(f"   {abs(phi_x0 - x0):.3f} ≤ {(1 - q) * delta:.3f}")
    
    if not second_condition:
        print("Second condition failed!")
        return False, None, None
    
    print("\nSecond condition satisfied!")
    
    
    method_eps = eps
    if( q < 0.5):
        method_eps = (1 - q) * eps / q
        print(f"As q < 0.5 => using enhanced precision:")
        print(f"eps_method = (1-q)*eps/q = (1-{q:.3f})·{eps}/{q:.3f} = {method_eps}")
    else:
        print(f"\nPrecision eps = {method_eps:}")
    

    #Prior estimate 
    prior_estimate=np.floor(np.log(abs(phi(x0)-x0)/((1-q)*eps))/np.log(1/q))+1
    


    # Iteration process
    print(f"\n Iteration process")
    print("-" * 40)
    print(f"{'Iter':<6} {'x_n':<12} {'x_n+1':<12} {'f(x_n+1)':<12} ")
    print("-" * 44)
    
    x_now = x0
    iterations = 0
    
    while True:
        x_next = phi(x_now)
        iterations += 1
        
        print(f"{iterations:<5} {x_now:<12.3f} {x_next:<12.3f} {f(x_next):<12.3f}")
        
        if (abs(x_next - x_now) <= method_eps):
            print("-" * 44)
            print(f"Final approximation: x* = {x_next:.5f}")
            return True, x_next,prior_estimate, iterations
        x_now = x_next


def modif_newtons_method(f, df, d2f, a, b, eps)-> tuple[bool, float, int]:
    # Find interval containing root
    print("\nFinding interval containing root")
    print("-" * 40)
    hasroot, a, b = has_root(f, a, b)
    if not hasroot:
        print("No root found in the given interval range!")
        return False, None, None
    
    print(f"Working interval: [{a:.2f}, {b:.2f}]")
    
    # Check sufficient conditions
    print(f"\nChecking convergence conditions")
    print("-" * 40)
    
    # Sufficient conditions:
    # 1. f'(x), f''(x) - constant sign on the interval [a;b]
    # 2. f'(x) != 0 on the [a;b]
    x_interval = np.linspace(a, b, 1000)
    dfx_values = np.array([df(x) for x in x_interval])
    dfx_values[np.abs(dfx_values) < 1e-10] = 0

    d2fx_values = np.array([d2f(x) for x in x_interval])
    d2fx_values[np.abs(d2fx_values) < 1e-10] = 0

    # Check if some of df(x) == 0 
    if np.any(dfx_values == 0):
        print("f'(x) = 0 at some point - first condition not satisfied!")
        return False, None, None

    # f'(x), f''(x) has to be constant sign on the interval [a;b]
    if not (np.all(dfx_values >= 0) or np.all(dfx_values <= 0)):
        print(f"f'(x) doesn't have constant sign on [{a:.2f},{b:.2f}]")
        return False, None, None

    if not (np.all(d2fx_values >= 0) or np.all(d2fx_values <= 0)):
        print(f"f''(x) doesn't have constant sign on [{a:.2f},{b:.2f}]")
        return False, None, None
    
  
    
    # Choose initial approximation
    x0 = (a + b) / 2
    if df(x0) * d2f(x0) <= 0:
        print("f'(x0)*f''(x0) <= 0 - choose different initial approximation!")
        return False, None, None
    
    print(f" Condition satisfied: f'(x0)*f''(x0) = {df(x0)*d2f(x0):.6f} > 0")
    print(f"\nx0 = {x0:.6f}")
    print(f"f'(x0) = {df(x0):.6f} (constant for all iterations)")
    
    # Iteration process
    print(f"\nIteration process")
    print("-" * 60)
    print(f"{'Iter':<6} {'x_n':<9} {'f(x_n)':<9} {'x_n+1':<9} {'f(x_n+1)':<9}")
    print("-" * 60)
    
    x_now = x0
    iterations = 0
    df_x0 = df(x0)
    x_prev = 0
    
    while True:
        f_x_now = f(x_now)
        x_next = x_now - (f_x_now / df_x0)
        iterations += 1
        
        print(f"{iterations:<5} {x_now:<9.4f} {f_x_now:<9.4f} {x_next:<9.4f} {f(x_next):<9.4f}")
        
        if np.abs(x_now - x_prev)<= eps: 
            print("-" * 60)
            print(f"Final approximation: x* = {x_next:.5f}")
            return True, x_next, iterations
            
        x_prev = x_now
        x_now = x_next

def main():

    eps = 1e-3
    #Interval for MNM
    a1=2.5
    b1=7

    #Interval for SIM
    a2=1
    b2=6
    # Modified Newton's Method
    print("MODIFIED NEWTON'S METHOD")
    print("=" * 60)
    def f1(x): return x**3 - 7*(x**2) + 7*x + 15
    def df1(x): return 3*x**2 - 14*x + 7
    def d2f1(x): return 6*x - 14
    
    print(f"Equation: f(x) = x**3 - 7*x**2 + 7*x + 15")
    print(f"epsilon = {eps}")
    print("=" * 60)
    
    success, x_res, iterations = modif_newtons_method(f1, df1, d2f1, a1, b1, eps)
    if success:
        print(f"\n" + "=" * 60)
        print("FINAL RESULTS")
        print("=" * 60)
        print(f"Root found: x* = {x_res:.8f}")
        print(f"Verification: f(x*) = {f1(x_res):.8f}")
        print(f"Iterations: {iterations}")

    

    # Simple Iteration Method
    print("\n\nSIMPLE ITERATION METHOD")
    print("=" * 60)
    def f2(x): 
        return x**3 - 5*x**2 - 4*x + 20
    
    print(f"Equation: f(x) = x**3 - 5*x**2 - 4*x + 20")
    print(f"epsilon = {eps}")
    print("=" * 60)

    def phi(x): 
        return np.cbrt(5*x**2 + 4*x - 20)
    def dphi(x):
        return (10*x + 4) / (3 * np.cbrt((5*x**2 + 4*x - 20)**2))
    

    # Run the method
    success, x_result,prior_estimate, iterations = method_of_simple_iteration(f2, phi, dphi, a2, b2, eps)
    
    if success:
        print(f"\n" + "=" * 60)
        print("FINAL RESULTS")
        print("=" * 60)
        print(f"Root found: x* = {x_result:.8f}")
        print(f"Prior estimate: {prior_estimate}")
        print(f"Posteriori estimate: {iterations}")
    else:
        print(f"\nMethod failed to converge!")

if __name__ == "__main__":
    main()