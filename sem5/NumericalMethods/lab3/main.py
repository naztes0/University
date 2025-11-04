import numpy as np


def power_method(A, eps=1e-4, max_iter=100):
    n = A.shape[0]
    x = np.ones(n)       
    lam_old = 0                 
    m=n-1

    print("====== POWER METHOD ======")
    print("Initial vector x0:", x)
    print("Eps:", eps)
    print("Maximum iterations:", max_iter)
    print("-"*30)
    for k in range(max_iter):
        print(f"\nIteration {k + 1}:")
        # Multiply A by the current vector
        x_new = A @ x
        print("A * x =", np.round(x_new, 3))

        # Estimate new eigenvalue
        lam_new = x_new[m] / x[m]
        print(f"λ_new = x_new[{m}] / x[{m}] = {lam_new:.3f}")

        #Normalize the resulting vector
        x_new_norm = np.linalg.norm(x_new)
        x = x_new / x_new_norm
        x_display = np.where(np.abs(x) < 1e-6, 0, np.round(x, 3))
        print(f"||x_new|| = {x_new_norm:.3f}")
        print(f"Normalized vector e{k + 1} =", x_display)

        #Check convergence
        if np.abs(lam_new - lam_old) <= eps:
            print("\n")
            print("-"*30)
            print(f"Converged after {k + 1} iterations.")
            print(f"Dominant eigenvalue λ ≈ {lam_new:.4f}")
            print("="*30)
            return lam_new
        
        lam_old = lam_new
       

    print("\n")
    print("-"*30)
    print("Method did not converge within the iteration limit.")
    print(f"Last estimated λ = {lam_new:.4f}")
    print("="*30)
    return lam_new


def min_eigenvalue(A):
    n = A.shape[0]
    #Check conditiond to use the power method 
    #A=A^T >0
    if not np.allclose(A, A.T):
        print("Matrix A isn't symmetrical. Method can't be applied.")
        return
    
    matrix_minor = np.zeros(n)
    print("Matrix minors:")
    for i in range(1, n + 1):
        minor = np.linalg.det(A[:i, :i])
        matrix_minor[i - 1] = round(minor)
        print (f" |A{i}|={matrix_minor[i-1]}") 
        if minor <= 0:
            print(f"Matrix is not positive definite (|A{i}| = {round(minor)})")
            return None

    print("\nConditions for power method is satisfied!")
    
    normA = np.max(np.sum(np.abs(A), axis=1)) 
    print("\nMax ||A||=",normA)
    print()

    # B = ||A||E - A
    B = normA * np.eye(len(A)) - A     
    print("B = ||A||E - A :\n", B)
    
    #find greatest egien value of B matrix by power method        
    lam_max_B = power_method(B) 
    print("\nmax_lam(B)=",lam_max_B)


    lam_min_A = normA - lam_max_B
    print("\nmin_lam(A)=||A||-max_lam_B")  
    print(f"\nmin_lam(A)={normA}-{lam_max_B}")
    print(f"\nmin_lam(A)={lam_min_A}")           
    return lam_min_A


def jacobi_method(A, eps=1e-8, max_iter=100):
    np.set_printoptions(precision=6, suppress=True)

    if not np.allclose(A, A.T):
        print("Matrix A isn't symmetrical. Jacobi method can't be applied.")
        return None, None

    n = A.shape[0]
    V = np.eye(n)  # for eigenvectors

    print("====== JACOBI ROTATION METHOD ======")
    print("Initial matrix A:")
    print(np.round(A, 6))
    print("Eps:", eps)
    print("Maximum iterations:", max_iter)
    print("-" * 40)

    for k in range(1, max_iter + 1):
        p, q = 0, 1
        max_val = 0.0
        for i in range(n):
            for j in range(i + 1, n):
                if abs(A[i, j]) > abs(max_val):
                    max_val = A[i, j]
                    p, q = i, j

        print(f"\nIteration {k}:")
        print(f"  Pivot indices (p, q) = ({p}, {q})")
        print(f"  a[p,q] = {A[p,q]:.6f}")

        # Перевірка зупинки
        off_norm = np.sqrt(np.sum(A**2) - np.sum(np.diag(A)**2))
        print(f"  Off-diagonal norm = {np.round(off_norm,6)}")
        if off_norm < eps:
            print("\nConvergence criterion satisfied (off-diagonal norm < eps).")
            break

        # Compute rotation angle φ_k (according to theory)
        a_pp = A[p, p]
        a_qq = A[q, q]
        a_pq = A[p, q]

        if a_pp == a_qq:
            phi_k = np.pi / 4
        else:
            phi_k = 0.5 * np.arctan((2 * a_pq) / (a_pp - a_qq))

        c = np.cos(phi_k)
        s = np.sin(phi_k)

        print(f"  φ_k = 0.5 * arctan(2 * a[{p},{q}] / (a[{p},{p}] - a[{q},{q}]))")
        print(f"  φ_k = {phi_k:.6f}")
        print(f"  cos(φ_k) = {c:.6f}")
        print(f"  sin(φ_k) = {s:.6f}")

        # Build full rotation matrix U_k
        U = np.eye(n)
        U[p, p] = c
        U[q, q] = c
        U[p, q] = -s
        U[q, p] = s

        print("\n  Rotation matrix U_k:")
        print(np.round(U, 6))

        # Apply transformation A_{k+1} = U^T * A * U
        A = U.T @ A @ U

        # Accumulate eigenvectors: V = V * U
        V = V @ U

        # Print updated matrix A
        print("\n  Matrix A after rotation:")
        print(np.round(A, 4))
        print("-" * 40)

    # After iterations, diagonal of A are approximated eigenvalues
    eigvals = np.diag(A).copy()
    eigvecs = V.copy()


    print("\n" + "-" * 40)
    print(f"Finished after {k} iterations.")
    print("Approximated eigenvalues:")
    print(np.round(eigvals, 6))
    print("\nCorresponding eigenvectors (columns):")
    print(np.round(eigvecs, 4))
    print("=" * 40)

    return eigvals, eigvecs




def modified_newton_system(eps=1e-6, max_iter=50):
    
    np.set_printoptions(precision=4, suppress=True)
    xk = np.array([0.0, 0.0], dtype=float)
    
    def F(x):
        x1, x2 = x
        return np.array([
            np.sin(x1 - 0.6) - x2 - 1.6,
            3*x1 - np.cos(x2) - 0.9
        ])
    
    def jacobian(x):
        x1, x2 = x
        return np.array([
            [np.cos(x1 - 0.6), -1],
            [3, np.sin(x2)]
        ])
    
    A0 = jacobian(xk)
    print("Initial Jacobian A0:")
    print(np.round(A0, 6))
    
    A0_inv = np.linalg.inv(A0)
    
    print("\n====== MODIFIED NEWTON METHOD ======")
    print(f"Initial guess x0 = {xk}")
    print(f"Epsilon = {eps}, Max iterations = {max_iter}")
    print("-" * 40)
    
    for k in range(1, max_iter+1):
        Fxk = F(xk)
        A0_invF = A0_inv @ Fxk
        x_next = xk - A0_invF
        
        print(f"\nIteration {k}:")
        print(f"  x{k} = {np.round(xk, 6)}")
        print(f"  F(x{k}) = {np.round(Fxk, 6)}")
        print(f"  A_inv*F(x{k}) = {np.round(A0_invF, 6)}")
        print(f"  x{k+1} = {np.round(x_next, 6)}")
        print(f"  ||x{k+1} - x{k}|| = {np.linalg.norm(x_next - xk):.6e}")
        
        # Check stopping criterion
        if np.linalg.norm(x_next - xk) < eps:
            print("\nConvergence criterion satisfied!")
            print(f"Solution found: x ≈ {np.round(x_next, 6)}")
            return x_next
        
        xk = x_next
    
    print("\nMethod did not converge within max iterations.")
    print(f"Last approximation: x = {np.round(xk, 6)}")
    return xk




def main():
    A = np.array([[4,1,0,1],
                  [1,3,2,0],
                  [0,2,4,0],
                  [1,0,0,2]])
    
    # min_eigenvalue(A)
    #jacobi_method(A)
    modified_newton_system()

if __name__ == "__main__":
    main()
