import numpy as np
import cv2
import os 


#Function for transforming imgs into arrays
def read_img():
    x3_img=cv2.imread("x3.bmp", cv2.IMREAD_GRAYSCALE)
    y3_img=cv2.imread("y3.bmp", cv2.IMREAD_GRAYSCALE)
    #show imgs
     #display the image
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



def main():
    #Firstly init and fill X, Y matrix from imgs
    X,Y= read_img