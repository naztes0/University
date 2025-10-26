import numpy as np
import cv2
import pim_methods as pim
import os 


#Function for transforming imgs into arrays
def read_img():
    x3_img=cv2.imread("x3.bmp", cv2.IMREAD_GRAYSCALE)
    y3_img=cv2.imread("y3.bmp", cv2.IMREAD_GRAYSCALE)
    # print("x3:", type(x3_img), x3_img is None)
    # print(x3_img.shape)
    # print(x3_img.dtype)

    #show imgs
    cv2.imshow("Image X3", x3_img)
    cv2.imshow("Image Y3", y3_img)
    #wait for the user to press a key
    cv2.waitKey(0)
    #close all windows
    cv2.destroyAllWindows()
    #floating data
    x = x3_img.astype(float)
    y = y3_img.astype(float)

    print(f"x size: {x.shape} , y size: {y.shape}")
    return x, y


def save_image(matrix, filename="output.bmp"):
    #make folder if not exists
    dir_res= "results"
    if not os.path.exists(dir_res):
        os.makedirs(dir_res)
    
    if not filename.endswith(".bmp"):
        filename=filename + ".bmp"
    save_path = os.path.join(dir_res,filename)

    #Normalizing matrix to [0,1] values
    normalized = (matrix - matrix.min()) / (matrix.max() - matrix.min())
    # Then scale to 0-255 range(as unit8 values)
    img = (normalized * 255).astype('uint8')
    # Save the img
    cv2.imwrite(save_path, img)
    
    # Display the image
    cv2.imshow("Result image", img)
    cv2.waitKey(0)
    cv2.destroyAllWindows()
    
    return img

def find_operator(X,Y,pim_func,V=None,eps=1e-6, delta=1000):
    if V is None:
        V = np.zeros((Y.shape[0], X.shape[0]))
    
    X_pi=pim_func(X,eps,delta)

    if not (pim.isPseudoInversed(X,X_pi)): return

    #General formula: A=YX+ + VZ^T(X^T)
    #Find Z(X^T)=I_m-XX+
    Z=np.identity(X.shape[0])-X@X_pi
    YX_pi= Y@X_pi
    VZ=V@Z.T
    A=YX_pi+VZ
    return A
    
def applyOperator(X,A):
    return A@X

def main():
    #Firstly init and fill X, Y matrix from imgs
    X,Y= read_img()
    # print("\nX mtrix:\n")
    # for i in range (X.shape[0]):
    #     for j in range(X.shape[1]):
    #         print(X[i,j])

    save_image(Y,"Y_orig")

    #Moore-Penrose method
    A_MP=find_operator(X,Y,pim.pim_MoorePenrose, eps=1e-6)
    Y_MP=applyOperator(X,A_MP)

    A_MG=find_operator(X,Y, pim.pim_Grevilles, eps=1e-6)
    save_image(Y_MP,"MP_result")
    
if __name__ == "__main__":
    main()



