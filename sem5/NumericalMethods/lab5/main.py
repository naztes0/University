import numpy as np


def f(x: float) -> float:
    """Початкова функція: f(x) = 2x^7 + 3x^6 + 4x^5 + 3"""
    return 2 * x**7 + 3 * x**6 + 4 * x**5 + 3

def f_second(x: float) -> float:
    """Друга похідна: f''(x) = 84x^5 + 90x^4 + 80x^3"""
    return 84 * x**5 + 90 * x**4 + 80 * x**3

# Вузли інтерполяції
x_nodes = np.array([0.0, 2.0, 4.0])
y_nodes = np.array([f(x) for x in x_nodes])

def print_header(title: str):
    print("\n" + "=" * 70)
    print(title)
    print("=" * 70 + "\n")

def print_matrix(A, b, var_name="c"):
    """Красивий вивід системи A * c = b"""
    n = len(b)
    print("Система рівнянь у вигляді A * {} = rhs:".format(var_name))
    for i in range(n):
        row = "  "
        for j in range(n):
            row += f"{A[i, j: i+1, j][0]:10.3f}"
        row += "   |   {:10.3f}".format(b[i])
        print(row)
    print()


def build_piecewise_linear(x, y):
    print_header("Кусково-лінійна інтерполяція")

    n = len(x) - 1  # кількість відрізків
    h = np.diff(x)

    print("Вузли та значення функції:")
    for i in range(len(x)):
        print(f"  x_{i} = {x[i]:.3f},   f(x_{i}) = {y[i]:.3f}")
    print("\nКроки h_i = x_i - x_{i-1}:")
    for i in range(1, len(x)):
        print(f"  h_{i} = x_{i} - x_{i-1} = {x[i]} - {x[i-1]} = {h[i-1]:.3f}")

    slopes = []    # k (нахили)
    intercepts = []  # b (вільні члени в y = kx + b)

    print("\nБудуємо відрізки L_i(x) = k_i * x + b_i на кожному проміжку [x_{i-1}, x_i]:\n")

    print("Текстовий вивід кроків:")
    for i in range(1, n + 1):
        hi = h[i - 1]
        k = (y[i] - y[i - 1]) / hi
        b = y[i - 1] - k * x[i - 1]
        slopes.append(k)
        intercepts.append(b)

        print(f"[Проміжок {i}: [{x[i-1]}, {x[i]}]]")
        print(f"  k_{i} = (f(x_{i}) - f(x_{i-1})) / h_{i}")
        print(f"       = ({y[i]:.3f} - {y[i-1]:.3f}) / {hi:.3f} = {k:.3f}")
        print(f"  b_{i} = f(x_{i-1}) - k_{i} * x_{i-1}")
        print(f"       = {y[i-1]:.3f} - {k:.3f} * {x[i-1]:.3f} = {b:.3f}")
        print(f" L_{i}(x) = {k:.3f} * x + {b:.3f}\n")

    print("Табличний вивід коефіцієнтів кусково-лінійної інтерполяції:")
    print("  Проміжок     k_i (нахил)        b_i (вільний член)")
    print("---------------------------------------------------------")
    for i in range(1, n + 1):
        print(f" [{x[i-1]:.1f}, {x[i]:.1f}]   {slopes[i-1]:12.3f}   {intercepts[i-1]:15.3f}")
    print()

    return slopes, intercepts

def eval_piecewise_linear(x_val, x_nodes, slopes, intercepts):
    """Обчислення кусково-лінійної інтерполяції в точці x_val"""
    if x_val <= x_nodes[0]:
        i = 1
    elif x_val >= x_nodes[-1]:
        i = len(x_nodes) - 1
    else:
        i = np.searchsorted(x_nodes, x_val)
    k = slopes[i - 1]
    b = intercepts[i - 1]
    return k * x_val + b


def build_piecewise_quadratic(x, y):
    print_header("Кусково-квадратична інтерполяція")

    print("Вузли та значення функції:")
    for i in range(len(x)):
        print(f"  x_{i} = {x[i]:.3f},   f(x_{i}) = {y[i]:.3f}")

    print("\nМаємо три вузли, тому кусково-квадратичний поліном визначений на всьому [x0, x2].")

    h = x[1] - x[0]
    print(f"Крок h = x1 - x0 = {h:.3f}\n")

    print("Формула квадратичного інтерполянта L_1^2(x):")
    print(r"L(x) = f(x0)*(x-x1)*(x-x2)/(2*h^2)"
          r" - f(x1)*(x-x0)*(x-x2)/(h^2)"
          r" + f(x2)*(x-x0)*(x-x1)/(2*h^2)\n")

    x0, x1, x2 = x
    f0, f1, f2 = y

    print("Підставляємо значення у формулу:")

    print(f"  f(x0) = {f0}")
    print(f"  f(x1) = {f1}")
    print(f"  f(x2) = {f2}\n")

    print("L(x) = "
          f"{f0} * (x-{x1})*(x-{x2}) / (2*{h**2})\n"
          f"     - {f1} * (x-{x0})*(x-{x2}) / ({h**2})\n"
          f"     + {f2} * (x-{x0})*(x-{x1}) / (2*{h**2})\n")


    A1 = f0 / (2*h*h)
    A2 = -f1 / (h*h)
    A3 = f2 / (2*h*h)

    # (x-x1)(x-x2) = x^2 - (x1+x2)x + x1*x2
    poly1 = np.poly1d([1, -(x1+x2), x1*x2])

    # (x-x0)(x-x2)
    poly2 = np.poly1d([1, -(x0+x2), x0*x2])

    # (x-x0)(x-x1)
    poly3 = np.poly1d([1, -(x0+x1), x0*x1])

    P = A1*poly1 + A2*poly2 + A3*poly3

    A_coef = P[2]    # коеф x^2
    B_coef = P[1]    # коеф x
    C_coef = P[0]    # вільний член

    print("Розкриваємо L(x) у вигляді ax^2 + bx + c:\n")
    print(f"  a = {A_coef:.6f}")
    print(f"  b = {B_coef:.6f}")
    print(f"  c = {C_coef:.6f}\n")

    print(f"Отже, кусково-квадратичний інтерполянт має вигляд:")
    print(f"L(x) = {A_coef:.6f} * x^2 + {B_coef:.6f} * x + {C_coef:.6f}\n")

    return A_coef, B_coef, C_coef

def eval_quadratic(x_val, A, B, C):
    return A * x_val**2 + B * x_val + C


def build_natural_cubic_spline(x, y):
    print_header("Природній кубічний сплайн (s''(a) = 0, s''(b) = 0)")

    n = len(x) - 1
    h = np.diff(x)

    print("Вузли та значення функції:")
    for i in range(len(x)):
        print(f"  x_{i} = {x[i]:.3f},   f(x_{i}) = {y[i]:.3f}")

    print("\nКроки h_i = x_i - x_{i-1}:")
    for i in range(1, len(x)):
        print(f"  h_{i} = {h[i-1]:.3f}")

    print("\nФормула для внутрішніх коефіцієнтів c_i:")
    print(r"  h_i*c_{i-1} + 2(h_i + h_{i+1})c_i + h_{i+1}c_{i+1} = "
          r"6[(f_{i+1}-f_i)/h_{i+1} - (f_i - f_{i-1})/h_i]")

    print("\nОскільки вузлів 3, маємо лише один внутрішній вузол i = 1.\n")

    h1 = h[0]
    h2 = h[1]

    print("Підставляємо значення:")
    print(f"  h1 = x1 - x0 = {h1}")
    print(f"  h2 = x2 - x1 = {h2}\n")

    print("Ліва частина рівняння:")
    print("  h1*c0 + 2(h1+h2)*c1 + h2*c2")
    print(f"= {h1}*c0 + 2({h1}+{h2})*c1 + {h2}*c2")
    print(f"= {h1}*c0 + {2*(h1+h2)}*c1 + {h2}*c2\n")

    print("Права частина рівняння:")
    term1 = (y[2] - y[1]) / h2
    term2 = (y[1] - y[0]) / h1
    RHS = 6 * (term1 - term2)

    print("  6[(f2 - f1)/h2 - (f1 - f0)/h1]")
    print(f"= 6[({y[2]} - {y[1]})/{h2} - ({y[1]} - {y[0]})/{h1}]")
    print(f"= 6[{term1:.6f} - {term2:.6f}]")
    print(f"= 6*({term1 - term2:.6f})")
    print(f"= {RHS:.6f}\n")

    print("Отже, рівняння для c1 має вигляд:")
    print(f"  {h1}*c0 + {2*(h1+h2)}*c1 + {h2}*c2 = {RHS:.6f}")
    print("Умови природності:  c0 = 0,   c2 = 0\n")

    print(f"Після підстановки c0=0, c2=0:")
    print(f"  {2*(h1+h2)} * c1 = {RHS:.6f}")
    print(f"  c1 = {RHS:.6f} / {2*(h1+h2)} = {RHS/(2*(h1+h2)):.6f}\n")

    print("Матриця A та вектор правих частин (для контролю):\n")

    A = np.zeros((n+1, n+1))
    rhs_vec = np.zeros(n+1)

    A[0,0] = 1         # c0 = 0
    rhs_vec[0] = 0

    A[1,0] = h1
    A[1,1] = 2*(h1+h2)
    A[1,2] = h2
    rhs_vec[1] = RHS

    A[2,2] = 1         # c2 = 0
    rhs_vec[2] = 0

    for i in range(3):
        row = "  "
        for j in range(3):
            row += f"{A[i,j]:10.3f}"
        row += f"   |   {rhs_vec[i]:10.3f}"
        print(row)
    print()

    c = np.linalg.solve(A, rhs_vec)

    print("Розв'язок системи для c_i:")
    for i in range(n+1):
        print(f"  c_{i} = {c[i]:.6f}")
    print()

    a = []
    b = []
    d = []

    print("Обчислюємо коефіцієнти a_i, b_i, d_i:\n")
    for i in range(1, n+1):
        hi = h[i - 1]
        ai = y[i]
        di = (c[i] - c[i-1]) / hi
        bi = hi*c[i]/2 - hi*hi*di/6 + (y[i] - y[i-1]) / hi

        a.append(ai)
        b.append(bi)
        d.append(di)


        print(f"[Проміжок {i}: [{x[i-1]}, {x[i]}]]")
        print(f"  a_{i} = f(x_{i}) = {ai:.6f}")
        print(f"  d_{i} = (c_{i} - c_{i-1}) / h_{i} = ({c[i]:.6f} - {c[i-1]:.6f}) / {hi:.3f} = {di:.6f}")
        print(f"  b_{i} = h_{i}/2 * c_{i} - h_{i}^2/6 * d_{i} + (f_{i} - f_{i-1}) / h_{i}")
        print(f"       = {hi:.3f}/2*{c[i]:.6f} - {hi:.3f}^2/6*{di:.6f} + ({y[i]:.6f}-{y[i-1]:.6f})/{hi:.3f}")
        print(f"       = {bi:.6f}\n")

        print(f"[Проміжок {i}: [{x[i-1]}, {x[i]}]]")
        print(f"  a_{i} = {ai:.6f}")
        print(f"  b_{i} = {bi:.6f}")
        print(f"  c_{i} = {c[i]:.6f}")
        print(f"  d_{i} = {di:.6f}\n")

    print("Формули природного кубічного сплайна на кожному проміжку:")
    print("Формула:")
    print("  s_i(x) = a_i + b_i*(x - x_i) + c_i/2*(x - x_i)^2 + d_i/6*(x - x_i)^3\n")

    for i in range(1, n+1):
        print(f"Проміжок [{x[i-1]}, {x[i]}]:")
        print(f"s_{i}(x) = {a[i-1]:.6f} "
              f"+ {b[i-1]:.6f} * (x - {x[i]:.1f}) "
              f"+ {c[i]/2:.6f} * (x - {x[i]:.1f})^2 "
              f"+ {d[i-1]/6:.6f} * (x - {x[i]:.1f})^3\n")

    return np.array(a), np.array(b), c, np.array(d)


def eval_cubic_spline(x_val, x_nodes, a, b, c, d):
    """
    Обчислення кубічного сплайну в точці x_val.
    Представлення: на кожному [x_{i-1}, x_i]
      s_i(x) = a_i + b_i (x - x_i) + c_i/2 (x - x_i)^2 + d_i/6 (x - x_i)^3,
    де a_i, b_i, d_i зберігаються у масивах довжини n, а c_i у масиві довжини n+1.
    """
    n = len(x_nodes) - 1
    if x_val <= x_nodes[0]:
        i = 1
    elif x_val >= x_nodes[-1]:
        i = n
    else:
        i = np.searchsorted(x_nodes, x_val)
    # i – номер правого вузла, інтервал [x_{i-1}, x_i]
    dx = x_val - x_nodes[i]
    ai = a[i - 1]
    bi = b[i - 1]
    ci = c[i]
    di = d[i - 1]
    return ai + bi * dx + ci * dx**2 / 2.0 + di * dx**3 / 6.0


def build_clamped_second_derivative_spline(x, y):
    print_header("Кубічний сплайн з реальними значеннями f''(x) на краях")

    n = len(x) - 1
    h = np.diff(x)

    print("Вузли та значення функції:")
    for i in range(len(x)):
        print(f"  x_{i} = {x[i]:.3f},   f(x_{i}) = {y[i]:.3f}")

    print("\nКроки h_i = x_i - x_{i-1}:")
    for i in range(1, len(x)):
        print(f"  h_{i} = {h[i-1]:.3f}")

    f2_left = f_second(x[0])
    f2_right = f_second(x[-1])

    print("\nРеальні значення другої похідної в кінцях:")
    print(f"  f''(x_0) = {f2_left:.6f}")
    print(f"  f''(x_n) = {f2_right:.6f}")
    print("\nОтже крайові умови:")
    print("  c_0 = f''(x_0)")
    print("  c_n = f''(x_n)\n")

    print("Формула для внутрішнього коефіцієнта c_i:")
    print(r"  h_i*c_{i-1} + 2(h_i + h_{i+1})c_i + h_{i+1}c_{i+1} = "
          r"6[(f_{i+1}-f_i)/h_{i+1} - (f_i - f_{i-1})/h_i]\n")


    h1 = h[0]
    h2 = h[1]


    # Права частина
    term1 = (y[2] - y[1]) / h2
    term2 = (y[1] - y[0]) / h1
    RHS = 6 * (term1 - term2)

    print("Права частина:")
    print("  6[(f2 - f1)/h2 - (f1 - f0)/h1]")
    print(f"= 6[({y[2]} - {y[1]})/{h2} - ({y[1]} - {y[0]})/{h1}]")
    print(f"= 6[{term1:.6f} - {term2:.6f}]")
    print(f"= {RHS:.6f}\n")

    print("Ліва частина рівняння:")
    print("  h1*c0 + 2(h1+h2)*c1 + h2*c2")
    print(f"= {h1}*c0 + {2*(h1+h2)}*c1 + {h2}*c2\n")

    print("Підставляємо крайові значення другої похідної:")
    print(f"  c0 = f''(x0) = {f2_left:.6f}")
    print(f"  c2 = f''(x2) = {f2_right:.6f}\n")

    print("Отже рівняння стає:")
    print(f"  {h1}*{f2_left:.6f} + {2*(h1+h2)}*c1 + {h2}*{f2_right:.6f} = {RHS:.6f}\n")

    print("Виділимо c1:")
    LS_constant = h1*f2_left + h2*f2_right
    print(f"-> {2*(h1+h2)}*c1 = {RHS:.6f} - ({LS_constant:.6f})")
    print(f"-> {2*(h1+h2)}*c1 = {RHS - LS_constant:.6f}")
    c1_manual = (RHS - LS_constant) / (2*(h1+h2))
    print(f"-> c1 = {c1_manual:.6f}\n")

    print("Матриця A та вектор правих частин (для контролю):\n")

    A = np.zeros((n + 1, n + 1))
    rhs = np.zeros(n + 1)

    A[0, 0] = 1.0
    rhs[0] = f2_left

    A[1, 0] = h1
    A[1, 1] = 2*(h1+h2)
    A[1, 2] = h2
    rhs[1] = RHS

    A[2, 2] = 1.0
    rhs[2] = f2_right

    for r in range(n + 1):
        row = "  "
        for c_idx in range(n + 1):
            row += f"{A[r, c_idx]:10.3f}"
        row += f"   |   {rhs[r]:10.3f}"
        print(row)
    print()

    c = np.linalg.solve(A, rhs)

    print("Розв'язок системи для c_i:")
    for i in range(n + 1):
        print(f"  c_{i} = {c[i]:.6f}")
    print()

    a = []
    b = []
    d = []

    print("Обчислення коефіцієнтів a_i, b_i, d_i:\n")
    for i in range(1, n + 1):
        hi = h[i - 1]
        ai = y[i]
        di = (c[i] - c[i - 1]) / hi
        bi = hi*c[i]/2 - hi*hi*di/6 + (y[i] - y[i - 1]) / hi

        a.append(ai)
        b.append(bi)
        d.append(di)

        print(f"[Проміжок {i}: [{x[i-1]}, {x[i]}]]")
        print(f"  a_{i} = f(x_{i}) = {ai:.6f}")
        print(f"  d_{i} = (c_{i} - c_{i-1}) / h_{i} = ({c[i]:.6f} - {c[i-1]:.6f}) / {hi:.3f} = {di:.6f}")
        print(f"  b_{i} = h_{i}/2 * c_{i} - h_{i}^2/6 * d_{i} + (f_{i} - f_{i-1}) / h_{i}")
        print(f"       = {hi:.3f}/2*{c[i]:.6f} - {hi:.3f}^2/6*{di:.6f} + ({y[i]:.6f}-{y[i-1]:.6f})/{hi:.3f}")
        print(f"       = {bi:.6f}\n")

        print(f"[Проміжок {i}: [{x[i-1]}, {x[i]}]]")
        print(f"  a_{i} = {ai:.6f}")
        print(f"  b_{i} = {bi:.6f}")
        print(f"  c_{i} = {c[i]:.6f}")
        print(f"  d_{i} = {di:.6f}\n")


    print("Формули кубічного сплайна з крайовими значеннями f''(x):")
    print("  s_i(x) = a_i + b_i*(x - x_i) + c_i/2*(x - x_i)^2 + d_i/6*(x - x_i)^3\n")

    for i in range(1, n + 1):
        print(f"Проміжок [{x[i-1]}, {x[i]}]:")
        print(f"s_{i}(x) = {a[i-1]:.6f} "
              f"+ {b[i-1]:.6f}*(x - {x[i]:.1f}) "
              f"+ {(c[i]/2):.6f}*(x - {x[i]:.1f})^2 "
              f"+ {(d[i-1]/6):.6f}*(x - {x[i]:.1f})^3\n")

    return np.array(a), np.array(b), c, np.array(d)






if __name__ == "__main__":
    
    a_nat, b_nat, c_nat, d_nat = build_natural_cubic_spline(x_nodes, y_nodes)

    a_mod, b_mod, c_mod, d_mod = build_clamped_second_derivative_spline(x_nodes, y_nodes)

    lin_slopes, lin_intercepts = build_piecewise_linear(x_nodes, y_nodes)

    quad_A, quad_B, quad_C = build_piecewise_quadratic(x_nodes, y_nodes)

