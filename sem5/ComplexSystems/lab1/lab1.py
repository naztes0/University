import numpy as np 
import matplotlib
matplotlib.use("TkAgg")
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit



#DFT - Discrete Fourie Transformation
def dft(samples:np.ndarray)->np.ndarray:
    """
    Own implementation of Discrete Fourier Transform (DFT).
    """
    N=len(samples)
    dft_computation=np.zeros(N,dtype=complex)
    for k in range(N):
        for m in range(N):
            dft_computation[k] += (samples[m]*np.exp(-2j*np.pi*k*m/N))
    return dft_computation



def dft_comparer(samples:np.ndarray)->None:
    """
    Comparing DFT result of lib numpy's func and own DFT func
    """
    my_dft=dft(samples)
    numpy_fft_res=np.fft.fft(samples)
    max_absolute_error = np.max(np.abs(my_dft - numpy_fft_res))

    if max_absolute_error < 1e-6:
        print("Own Discrete Fourier Transformation matches NumPy's FFT.")
    else:
        print("Own Discrete Fourier Transformation does not match NumPy's FFT.")
        print(f"Maximum absolute error: {max_absolute_error}")




def find_frequencies_contribution(samples:np.ndarray, interval:int=5):
    """
    Find main frequency contributions in the signal using DFT peaks.
    """

    step=1/interval # deltaf=1/T
    samples_number=len(samples)
    dft_abs=np.abs(dft(samples)) #Find the magnitude
    print(dft_abs)
    peak_indices=[]

    #As the right part of signal is a mirrored left one using only the half of range
    for k in range(1,(samples_number//2)-1):
        if(dft_abs[k]>dft_abs[k-1] and dft_abs[k]>dft_abs[k+1]):
            peak_indices.append(k)
 
    plot_dft(dft_abs, step, 'dicrete_fourier_transform.png',peak_indices)

    frequencies = [k*step for k in peak_indices]
    return frequencies

def plot_dft(dft:np.ndarray,step:float, save_path:str, max_peaks:np.array=None)->None:
    """
    Plot the DFT magnitude spectrum and highlight peaks.
    """
     
    num_samples=len(dft)//2
    frequencies = np.arange(1, num_samples) * step
    dft_abs_half=dft[:num_samples]
    peak_frequency = 0
    if(max_peaks):
        peak_frequencies=np.array(max_peaks)*step
        peak_frequency=max(peak_frequencies)
        peak_magnitude=dft_abs_half[max_peaks]
        plt.scatter(peak_frequencies,peak_magnitude,color='red',zorder=4)
    
    print("Peak frequence: ", peak_frequency,"Hz")
    plt.plot(frequencies, dft[1:num_samples], label="DFT Magnitude")
    plt.title("Discrete Fourier Transform Spectrum")
    plt.legend()
    plt.xlabel("Frequence (Hz)")
    plt.ylabel("Magnitude")
    plt.savefig(save_path)
    plt.show()
   
def fit_model(t,data, peak_frequencies):
    """
    Fit polynomial + sinusoidal model to the signal.
    """
    def model (t, a1,a2,a3,*params):
        k=len(params)//2
        y=a1*t**3+a2*t**2+a3*t 
        for i in range(k):
            fi=params[i]
            ai=params[i+k]
            y+= ai*np.sin(2*np.pi*fi*t)
        return y
    initial_guess=[0,0,0]+peak_frequencies+[1]*len(peak_frequencies)
    params,covariance = curve_fit(model,t,data,p0=initial_guess)
    fitted_values=model(t, *params)
    return params, fitted_values

def main():

    observations=np.loadtxt('f12.txt')

    #Find the Discrete Fourier Transform
    discrete_fourier_transform=dft(observations)
    #Check the correctness of own DFT func 
    dft_comparer(observations)

    peak_frequencies=find_frequencies_contribution(observations)

    time_step=0.01
    t=np.arange(len(observations))*time_step

    #Fitting the model
    params, fitted_values=fit_model(t,observations,peak_frequencies)
    print("Found parameters:", params)
    plt.figure(figsize=(12, 6))
    plt.plot(t, observations, label='Observations')
    plt.plot(t, fitted_values, label='Fitted Model', linestyle='--')
    plt.legend()
    plt.xlabel('Time')
    plt.ylabel('y(t)')
    plt.title('Comparison of Observations and Fitted Model')
    plt.savefig("fitmodel_comparison.png")
    plt.show()

if __name__ == "__main__":
    main()
