import numpy as np 


##PSEUDO INVERS MATRIX METHODS##
# pi - pseudo inversed

def isPseudoInversed(A,A_pi, rtol=1e-3, atol=1e-1) -> bool:  # Послабили допуски
    #Check PIM properties
    #1. A * A+ * A = A
    cond1 = np.allclose(A @ A_pi @ A, A, rtol=rtol, atol=atol)
    if not cond1:
        diff = np.linalg.norm(A @ A_pi @ A - A)
        print(f'A * A+ * A = A : not satisfied (diff={diff:.6f})')
        return False
    
    #2. A+ * A * A+ = A+
    cond2 = np.allclose(A_pi @ A @ A_pi, A_pi, rtol=rtol, atol=atol)
    if not cond2:
        diff = np.linalg.norm(A_pi @ A @ A_pi - A_pi)
        print(f'A+ * A * A+ = A+ : not satisfied (diff={diff:.6f})')
        return False
    
    #3. A * A+ - symetric (m*m)
    AA_pi = A @ A_pi
    cond3 = np.allclose(AA_pi, AA_pi.T, rtol=rtol, atol=atol)
    if not cond3:
        diff = np.linalg.norm(AA_pi - AA_pi.T)
        print(f'A * A+ - not symmetric (diff={diff:.6f})')
        return False
    
    #4. A+ * A - symetric (n*n)
    A_pi_A = A_pi @ A
    cond4 = np.allclose(A_pi_A, A_pi_A.T, rtol=rtol, atol=atol)
    if not cond4:
        diff = np.linalg.norm(A_pi_A - A_pi_A.T)
        print(f'A+ * A - not symmetric (diff={diff:.6f})')
        return False
    
    print('\nMatrix is pseudo-inversed\n')
    return True


#find aproximate A+ with eps accuracy
def pim_MoorePenrose(A,eps=1e-6, delta=100):
    m,n=A.shape
    #Start aproximation
    A0=A.T @ np.linalg.inv(A @ A.T+delta**2*np.eye(m))

    #Stepping
    delta=delta/2

    while True: 
        A_pim= A.T @ np.linalg.inv(A @ A.T+delta**2*np.eye(m))
        if np.linalg.norm(A0-A_pim, ord=2)<eps:
            return A_pim
        delta=delta/2
        A0=A_pim

def pim_Grevilles(A, eps=1e-6,delta=None):
    m,n=A.shape

    
