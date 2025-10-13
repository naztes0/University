import numpy as np

def print_matrix(matrix):
    for i in range(len(matrix)):
        for j in range(len(matrix[i])):
            print(f"{matrix[i][j]:8.3f}", end=" ")
        print()
    print()

def print_vector(vector):
    for i in range(len(vector)):
        print (f"{vector[i]:8.3f}", end=" ")
    print()

def print_expandedmatrix(matrix, vector):
    for i in range(len(vector)):
        for j in range(len(vector)):
            print(f"{matrix[i][j]:8.3f}", end=" ")
        print(f"| {vector[i]:8.3f}", end=" ")    
        print()

def print_expanded_with_identity(A, E):
    n = len(A)
    for i in range(n):
        for j in range(n):
            print(f"{A[i][j]:8.3f}", end=" ")
        print("|", end=" ")
        for j in range(n):
            print(f"{E[i][j]:8.3f}", end=" ")
        print()
    print()

def gaussian_inverse(A):
    A = np.array(A, dtype=float)
    n = len(A)
    E = np.eye(n)

    print("\n==============================")
    print("Finding inverse matrix A^-1:")
    print("==============================")
    print("Initial A and E:")
    print_expanded_with_identity(A, E)

    for i in range(n):
        # Find the main element manually
        max_row = i
        max_value = abs(A[i, i])
        for row in range(i + 1, n):
            if abs(A[row, i]) > max_value:
                max_value = abs(A[row, i])
                max_row = row

        # Swap rows if needed
        if max_row != i:
            A[[i, max_row]] = A[[max_row, i]]
            E[[i, max_row]] = E[[max_row, i]]
            print(f"Swapped rows {i} and {max_row}")

        # Normalize current row
        pivot = A[i, i]
        for col in range(n):
            A[i, col] /= pivot
            E[i, col] /= pivot

        # Eliminate other rows
        for j in range(n):
            if j != i:
                factor = A[j, i]
                for col in range(n):
                    A[j, col] -= factor * A[i, col]
                    E[j, col] -= factor * E[i, col]

        print(f"\nAfter step {i + 1}:")
        print_expanded_with_identity(A, E)
    
    print("==============================")
    print("Inverse matrix A^-1:")
    print_matrix(E)
    
    return E

def gaussian_method(A, b):
    A = np.array(A, dtype=float)
    b = np.array(b, dtype=float)
    n = len(b)
    detA=1
    detAmult=[]
    p=0 #amount of row swapping

    print ()
    print ("="*50)
    print("GAUSSIAN METHOD")
    print ("="*50)
    print ()
    print("-"*50)
    print("Initial matrix A and vector b")
    print_expandedmatrix(A,b)
    print("-"*50)

    # Forward elimination
    for i in range(n):
        print()
        print(f"Working on column {i}")

        # Choose main element in col
        max_row_index = i
        max_value = abs(A[i, i])
        for row in range(i+1, n):
            if abs(A[row, i]) > max_value:
                max_value = abs(A[row, i])
                max_row_index = row

        # Swaping rows, if its necessary
        if max_row_index != i:
            print(f"Swap row {i} with row {max_row_index} because {max_value} is largest in column")
            p+=1
            temp_row = A[i].copy()
            A[i] = A[max_row_index]
            A[max_row_index] = temp_row

            temp_b = b[i]
            b[i] = b[max_row_index]
            b[max_row_index] = temp_b

            print_expandedmatrix(A,b)
        # Nulling elements below main one
        for j in range(i+1, n):
            factor = A[j, i] / A[i, i]
            print(f"Eliminate element A[{j},{i}] using factor = {factor:.3f}")
            for k in range(i, n):
                A[j, k] = A[j, k] - factor * A[i, k]
            b[j] = b[j] - factor * b[i]

        #Calculating det A
        detA*=A[i,i]
        detAmult.append(A[i,i])


        print("Matrix after elimination step:")
        print_expandedmatrix(A,b)
        print("-----------------------------")
    #Result of det A
    detA*=(-1)**p

    # Back substitution
    x = np.zeros(n)
    for i in range(n-1, -1, -1):
        sum_ax = 0
        for j in range(i+1, n):
            sum_ax += A[i, j] * x[j]
        x[i] = (b[i] - sum_ax) / A[i, i]
        print(f"Back substitution: x[{i}] = ({b[i]:.3f} - {sum_ax:.3f}) / {A[i,i]:.3f} = {x[i]:.3f}")

    print("-----------------------------")
    print("Solution vector x:")
    print_vector(x)

    print("-----------------------------")
    print("Calculating det A:")
    det_str = [f"{val:.3f}" for val in detAmult]
    print(f"det A: (-1)^{p} * " + " * ".join(det_str))

    print(f"Det A:{detA}")
    print (f"Calculating det A with NumPy: {np.round(np.linalg.det(A))}")
    return x,detA

def square_root_method(A, b):
    A = np.array(A, dtype=float)
    b = np.array(b, dtype=float)
    n = len(A)
    detA=1
    # Initialize S and D
    S = np.zeros((n, n))
    D = np.zeros(n)
    print ()
    print ("="*50)
    print("SQUARE ROOT METHOD")
    print ("="*50)
    print ()
    print("Initial matrix A:")
    print_matrix(A)
    print("Vector b:")
    print_vector(b)


    #Check sufficient condition
    # A=A^T
    if(np.all(A!=np.transpose(A))):
        print("Sufficient condition is not satisfied!")
        return
    print("\nThe sufficient condition is satisified: A=A^T \n")
    
    # Compute S and D
    for i in range(n):
        sum_squares = 0
        for p in range(i):
            sum_squares += (S[p, i] ** 2) * D[p]
        
        D[i] = np.sign(A[i, i] - sum_squares)
        S[i, i] = np.sqrt(abs(A[i, i] - sum_squares))
        
        for j in range(i+1, n):
            sum_products = 0
            for p in range(i):
                sum_products += S[p, i] * D[p] * S[p, j]
            S[i, j] = (A[i, j] - sum_products) / (S[i, i] * D[i])
    
    print("\nMatrix S:")
    print_matrix(S)
    print("Vector D:")
    print_vector(D)

    # Forward substitution: S^T*D*y = b
    y = np.zeros(n)
    for i in range(n):
        sum_sdy = 0
        for j in range(i):
            sum_sdy += S[j, i] * D[j] * y[j] 
        y[i] = (b[i] - sum_sdy) / (S[i, i] * D[i])

    # Backward substitution: S*x = y
    x = np.zeros(n)
    for i in range(n-1, -1, -1):
        sum_sx = 0
        for j in range(i+1, n):
            sum_sx += S[i, j] * x[j]
        x[i] = (y[i] - sum_sx) / S[i, i]

    print("\nSolution vector x:")
    print_vector(x)
    
    for k in range(n):
        detA*=D[k]*S[k,k]**2
    
    print("\nCalculating det A:")
    det_str = [f"{D[k]:.3f}*{S[k,k]:.3f}^2" for k in range(n)]
    print(f"det A: "+" * ".join(det_str))

    print(f"\nDet A:{np.round(detA)}")
    print (f"Calculating det A with NumPy: {np.round(np.linalg.det(A))}\n")

    norm_A = np.linalg.norm(A, ord=np.inf)
    norm_A_inv = np.linalg.norm(np.linalg.inv(A), ord=np.inf)
    cond_A = norm_A * norm_A_inv
    print(f"Condition number: {cond_A:.3f}")
    return x,detA

def main():
    #Gaussian method
    A_matrix_1 = [[7, 2, 3, 0],
                [0, 3, 2, 6],
                [2, 5, 1, 0],
                [0, 1, 4, 2]]
    b_vector_1 = [20, 36, 15, 22]

    #Square root method
    A_matrix_2 = [[1,2,0],
                [2,2,3],
                [0,3,2]]
    b_vector_2 = [5,15,12]

    resx,detA=gaussian_method(A_matrix_1, b_vector_1)
    inverseA=gaussian_inverse(A_matrix_1)

    square_root_method(A_matrix_2,b_vector_2)


main()
