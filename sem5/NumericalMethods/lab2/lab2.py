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

def invA_fullcalc(A, P_all, M_all, n):
    print()
    print("=" * 50)
    print("FULL CALCULATION OF INVERSE MATRIX")
    print("=" * 50)
    print()

    formula = "E"
    invA = np.eye(n) 
    A_work = A.copy() 

    print("\nFORWARD ELIMINATION")
    
    for i in range(n):
        print("#" * 30)
        print(f"Step {i+1}:\n")
        print(f"P{i+1}:")
        print_matrix(P_all[i])
        print(f"M{i+1}:")
        print_matrix(M_all[i])
        
        formula = f"M{i+1} * P{i+1} * " + formula
        print(f"invA = {formula}\n")
    
        invA = M_all[i] @ P_all[i] @ invA
        A_work = M_all[i] @ P_all[i] @ A_work
        
        print("Result of applying M and P on invA:")
        print_matrix(invA)

    print("\nBACK SUBSTITUTION PHASE")
    print("=" * 50)
    print("Upper triangular matrix after forward elimination:")
    print_matrix(A_work)
    print("\nMatrix invA after forward elimination (before back substitution):")
    print_matrix(invA)
    print("=" * 50)
    
    for col in range(n):
        print(f"\nProcessing column {col}:")
        for i in range(n-1, -1, -1):
            sum_ax = 0
            for j in range(i+1, n):
                sum_ax += A_work[i, j] * invA[j, col]
            invA[i, col] = (invA[i, col] - sum_ax) / A_work[i, i]
            print(f"invA[{i},{col}] = ({invA[i, col] * A_work[i, i] + sum_ax:.3f} - {sum_ax:.3f}) / {A_work[i,i]:.3f} = {invA[i, col]:.3f}")
        print("-" * 30)

    print("=" * 50)

def gaussian_method(A, b):
    A = np.array(A, dtype=float)
    b = np.array(b, dtype=float)
    n = len(b)
    A_original=A.copy()
    E=np.eye(n)
    invA=np.eye(n) #inverted A
    detA=1
    detAmult=[]
    p=0 #amount of row swapping
    formulaA="A"
    formulab="b"

    #arrays for detailed printig out of calculation inversed A
    M_all = []  
    P_all = []
    
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
        P=np.eye(n)
        M=np.eye(n) 
        print()
        print(f"Working on column {i}")

        # Choose main element in col
        max_row_index = i
        max_value = abs(A[i, i])
        for row in range(i+1, n):
            if abs(A[row, i]) > max_value:
                max_value = abs(A[row, i])
                max_row_index = row

        formulaA=f"P{i+1}"+formulaA
        formulab=f"P{i+1}"+formulab
        if max_row_index!=i:
            print_expandedmatrix(A,b)
            p+=1
            temp_row=P[i].copy()
            P[i]=P[max_row_index]
            P[max_row_index]=temp_row
            A=P@A
            b=P@b
            invA=P@invA
            print()
            print("-"*30)
            print(f"{formulaA} = {formulab}")
            print("-"*30)
            print(f"\nMatrix P{i+1}")
            print_matrix(P)
            print_expandedmatrix(A,b)
        P_all.append(P.copy())

        # Normalizing diagonal element of M matrix
        M[i, i] = 1.0 / A[i, i]

        # Then nulling elems under the dia
        for j in range (i+1,n):
            M[j,i] = -A[j,i]/A[i,i]
            if abs(M[j, i]) < 1e-10:
                M[j, i] = 0.0

        print(f"\nMatrix M{i+1}:")
        print_matrix(M)
        detA*=A[i,i]
        detAmult.append(A[i,i])

        #Multiplicate A,b with M_i+1 matrix
        A=M@A
        b=M@b
        invA=M@invA
         
        print("Matrix after elimination step:\n")
        formulaA=f"M{i+1}"+formulaA
        formulab=f"M{i+1}"+formulab
        print("-"*30)
        print(f"{formulaA} = {formulab}")
        print("-"*30)
        print_expandedmatrix(A,b)
        print("-"*30)
        M_all.append(M.copy())
    #Result of det A
    detA*=(-1)**p


    # print("Triangle A matrix:\n")
    # print_matrix(A)
    # print_vector(b)
    # print("")
    # print_matrix(invA)
     # Back substitution
    x = np.zeros(n) 
    for i in range(n-1, -1, -1): 
        sum_ax = 0 
        for j in range(i+1, n): 
            sum_ax += A[i, j] * x[j] 
        x[i] = b[i] - sum_ax
        print(f"Back substitution: x[{i}] = {b[i]:.3f} - {sum_ax:.3f} = {x[i]:.3f}")

    for col in range(n): 
        for i in range(n-1, -1, -1):  
            sum_ax = 0
            for j in range(i+1, n):
                sum_ax += A[i, j] * invA[j, col]  
            invA[i, col] = invA[i, col] - sum_ax

    print("-----------------------------")
    print("Solution vector x:")
    print_vector(x)

    print("-----------------------------")
    print("Calculating det A:")
    det_str = [f"{val:.3f}" for val in detAmult]
    print(f"det A: (-1)^{p} * " + " * ".join(det_str))

    print(f"Det A:{detA}")
    print (f"Calculating det A with NumPy: {np.round(np.linalg.det(A_original))}")
    invA_fullcalc(A_original, P_all, M_all, n)
    print("Inverted A:\n")
    print_matrix(invA)
    if np.allclose(A_original @ invA, E, atol=1e-10): print("A*A^-1=E. The inveted A matrix is correct")
    else: print("The inveted A matrix is NOT correct")
    return x,detA,invA

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
    if np.any(A !=A.T):
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
    print(f"Condition number: {cond_A:.3f}\n")
    return x,detA


def seidels_method(A,b, eps):
    A = np.array(A, dtype=float)
    b = np.array(b, dtype=float)
    n = len(A)

    print ()
    print ("="*50)
    print("SEIDELS METHOD")
    print ("="*50)
    print ()
    print("Initial matrix A:")
    print_matrix(A)
    print("Vector b:")
    print_vector(b)

    #Check sufficient conditions 
    ## for all i:1,n |a_ii|>=sum_j=1(j!=i)_n(|a_ij|)
    print("-"*40)
    print("Check the first condition:")
    for i in range(n):
        row_sum=0
        for j in range(n):
            if(i!=j):
                row_sum+=np.abs(A[i][j])
        if(np.abs(A[i][i])<row_sum): 
            print(f"Row{i}: {np.abs(A[i][i])}<{row_sum}")
            print("First sufficient condition is not satisfied!")
            return 
        print(f"Row {i}: {np.abs(A[i][i])}>={row_sum}")   
    print("First condition is satisfied!") 

    ##A=A^T>0
    if np.any(A !=A.T):
        print("Second sufficient condition is not satisfied!")
        return
    print("Second condition is satisfied!")
    print("-"*40)
    #initial approximation
    x_prev = np.zeros(n)
    x_new = np.zeros(n)
    iteration = 0

    print("\nIterations:")
    while True:
        iteration += 1
        for i in range(n):
            s1 = sum(A[i][j] * x_new[j] for j in range(i))       # new vector
            s2 = sum(A[i][j] * x_prev[j] for j in range(i + 1, n)) # prev vector
            x_new[i] = (b[i] - s1 - s2) / A[i][i]

        # Condition of break
        if np.linalg.norm(x_new - x_prev, ord=np.inf) < eps:
            break

        x_prev = x_new.copy()
        print(f"Iter {iteration}: {np.round(x_new, 5)}")

    print("\nFinal solution vector x:")
    print_vector(x_new)
    print(f"Number of iterations: {iteration}")
    return x_new


def main():
    #Gaussian method
    A_matrix_1 = [[7, 2, 3, 0],
                [0, 3, 2, 6],
                [2, 5, 1, 0],
                [0, 1, 4, 2]]
    b_vector_1 = [20, 36, 15, 22]
    resx1,detA1, invA=gaussian_method(A_matrix_1, b_vector_1)

    #Square root method
    A_matrix_2 = [[1,2,0],
                [2,2,3],
                [0,3,2]]
    b_vector_2 = [5,15,12]
    res_x2,detA2=square_root_method(A_matrix_2,b_vector_2)


    #Seidel's method
    A_matrix_3=[[4,0,1,0],
                [0,3,0,2],
                [1,0,5,1],
                [0,2,1,4]]
    b_vector_3=[12,19,27,30]

    epsilon=1e-4
    res_x3=seidels_method(A_matrix_3,b_vector_3,epsilon)


if __name__ == "__main__":
    main()
