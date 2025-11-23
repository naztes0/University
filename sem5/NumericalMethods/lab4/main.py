import numpy as np

def func(x):
        return 2*x**7+3*x**6+4*x**5+3
    
def max_der(x):
    return 5040*x**2+2160*x+480
def nodes_Chebyshov(func, nodes_num,a,b):
    print(f"Choosing {nodes_num} nodes by Chebyshov polynomial")

    nodes=[]
    print(f"{'k':<3} {'xk':<10} {'f(xk)':<10}")
    for k in range(nodes_num):
        xk=(a+b)/2 + ((b-a)/2)*np.cos((2*k+1)*np.pi/(2*nodes_num))
        nodes.append(xk)
        print(f"{k:<3} {xk:<10.3f} {func(xk):<10.3f}")
    return nodes

def print_table(table, headers):
    print(" | ".join(f"{h:<14}" for h in headers))
    print("-" * (16 * len(headers)))

    for row in table:
        formatted_row = []
        for col_index, value in enumerate(row):
            if col_index == 0:
                formatted_row.append(f"{int(value):<14}")
            else:
                formatted_row.append(f"{value:<14.4f}")
        
        print(" | ".join(formatted_row))



def divided_differences(x, y):
    n = len(x)
    table = np.zeros((n, n + 2))

    table[:, 0] = np.arange(n)
    table[:, 1] = x
    table[:, 2] = y
    for j in range(1, n):
        for i in range(n - j):
            table[i][j + 2] = (table[i + 1][j + 1] - table[i][j + 1]) / (x[i + j] - x[i])

    f_x_arr = []
    for order in range(n):         
        name = "f("
        for i in range(order + 1):   
            name += f"x{i}"
            if i != order:
                name += ","
        name += ")"
        f_x_arr.append(name)
    headers = ["i", "x"] + f_x_arr

    print("\nDivided differences table:")
    print_table(table, headers)

    return table



def newton_interpolation(func, nodes_num, a,b):
    print("")
    nodes=nodes_Chebyshov(func,nodes_num, a,b)
    y=[func(xi) for xi in nodes]
    table= divided_differences(nodes,y)
    coef=table[0, 2:]
    
    return nodes, coef

def newton_polynomial_string(nodes, coef):
    n = len(coef)
    terms = [f"{coef[0]:.2f}"]
    for i in range(1, n):
        term = f"{coef[i]:.2f}"
        for j in range(i):
            term += f"*(x - {nodes[j]:.3f})"
        terms.append(term)
    poly_str = " +\n".join(terms)
    return poly_str

def newton_polynomial_eval(nodes, coef, x):
    result = coef[0]
    prod = 1.0
    for i in range(1, len(coef)):
        prod *= (x - nodes[i-1])
        result += coef[i] * prod
    return result


def factorial_func(n):
    res=1
    for i in range(1,n+1):
        res*=i
    return res

def interpolation_error(func, newton_polynomial_func,coef, max_der, a,b,nodes, test_x):
    n=len(nodes)-1
    max_point=max_der(b)
    leq=(max_point/factorial_func(n+1))*(((b-a)**(n+1))/2**(2*n+1))
    func_res=func(test_x)
    newton_poly_res=newton_polynomial_func(nodes, coef, test_x)
    print("\n====== Interpolation Check ======")
    print(f"Test point x = {test_x}")
    print(f"Function value f(x)       = {func_res:.6f}")
    print(f"Newton polynomial P(x)    = {newton_poly_res:.6f}")
    print(f"Absolute difference        = |f(x) - P(x)| = {abs(func_res - newton_poly_res):.6f}")
    print(f"Theoretical error bound    = {leq:.6f}")
    print("="*33)

    if abs(func_res - newton_poly_res) <= leq:
        print("Interpolation error is within the theoretical bound.")
    else:
        print("Interpolation error exceeds the theoretical bound.")

def find_root(func, left=-5, right=0, eps=1e-5):
    print("\n====== Searching for root of f(x)=0 ======")

    if func(left) * func(right) > 0:
        print("Searching for sign change on interval...")

        step = 0.5
        x1, x2 = left, left + step

        found = False
        while x2 <= right:
            if func(x1) * func(x2) <= 0:
                left, right = x1, x2
                found = True
                break
            x1 = x2
            x2 += step

        if not found:
            print("No sign change found — function may have no real roots.")
            return None

    print(f"Sign change detected on interval [{left}, {right}]")
    print("Applying bisection method...\n")

    iteration = 0
    while right - left > eps:
        mid = (left + right) / 2
        f_mid = func(mid)

        print(f"Iter {iteration:2d}: mid={mid:.7f}, f(mid)={f_mid:.7f}")

        if f_mid == 0:
            break
        if func(left) * f_mid < 0:
            right = mid
        else:
            left = mid

        iteration += 1

    root = (left + right) / 2
    print("\n====== Root Found ======")
    print(f"x ≈ {root:.10f}")
    print(f"f(x) ≈ {func(root):.10f}")
    print("========================")

    return root

#direct interpolation function 
def direct_interpolation(func, nodes_num, a, b, eps=1e-6):
    print("\n=========== Direct Interpolation Method ===========")
    di_nodes, di_coef = newton_interpolation(func, nodes_num, a, b)
    print("\nNewton interpolation polynomial P(x):")
    print(newton_polynomial_string(di_nodes, di_coef))
    
    def P(x):
        return newton_polynomial_eval(di_nodes, di_coef, x)

    print("\nSearching for root of P(x)=0 on interval:")
    print(f"[{a}, {b}]")

    fa = P(a)
    fb = P(b)

    if fa * fb > 0:
        print("\nERROR: P(a) and P(b) have the SAME sign.")
        print("Bisection cannot be applied.")
        print(f"P({a}) = {fa:.3f}, P({b}) = {fb:.3f}")
        return None

    print("\nApplying bisection to P(x)=0...\n")

    left, right = a, b
    iteration = 0

    while right - left > eps:
        mid = (left + right) / 2
        fmid = P(mid)

       
        print(f"iter {iteration:2d}: mid={mid:.7f}, P(mid)={fmid:.16f}")

        if fmid == 0 and fmid<1e-10:
            break

        if fa * fmid < 0:
            right = mid
            fb = fmid
        else:
            left = mid
            fa = fmid

        iteration += 1

    root = (left + right) / 2

    print("\n=========== Result ==========")
    print(f"Root of P(x)=0 : x ≈ {root:.10f}")
    print(f"P(x) ≈ {P(root):.10f}")
    print("=============================\n")

    return root

def inverse_interpolation(func, nodes_num, a, b):
    print("\n=========== Inverse Interpolation Method ===========")
    
    x_nodes = nodes_Chebyshov(func, nodes_num, a, b)
    y_nodes = [func(x) for x in x_nodes]

    print("\nBuilding polynomial x(y) (swapping x and y)...")

    table_inv = divided_differences(y_nodes, x_nodes)
    coef_inv = table_inv[0, 2:]

    root = newton_polynomial_eval(y_nodes, coef_inv, 0)

    print("\n=========== Result (Inverse) ==========")
    print(f"Approximation for f(x) = 0")
    print(f"Root x ≈ {root:.10f}")
    print(f"Check f(x_root) ≈ {func(root):.10f}")
    print("=======================================\n")

    return root


def main():
    
    nodes_num=5

    #Interval
    start=0
    end=4

    #Choose nodes by Chebyshov polynomial
    nodes, coef=newton_interpolation(func, nodes_num, start,end)
    newton_poly_str=newton_polynomial_string(nodes,coef)
    print(f"\nP{nodes_num-1}(x)=")
    print(newton_poly_str)

    test_x=2.4
    interpolation_error(func, newton_polynomial_eval,coef, max_der,start,end,nodes,test_x)

    root = find_root(func)
    #direct interpolation (di)
    direct_interpolation(func,nodes_num, -2, 0)
    #inverse interpolation(ii)
    inverse_interpolation(func, nodes_num, -2, 0)

    



if __name__ == "__main__":
    main()
