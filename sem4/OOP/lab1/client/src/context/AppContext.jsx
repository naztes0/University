import { createContext, useState, useEffect } from "react";
import { useAuth, useClerk, useUser } from "@clerk/clerk-react";
import axios from "axios";
import { toast } from "react-toastify";
import { useNavigate } from "react-router-dom";

export const AppContext = createContext();

const AppContextProvider = (props) => {
  const [credit, setCredit] = useState(false);
  const [image, setImage] = useState(false);
  const [resultImage, setResultImage] = useState(false);
  const [timerActive, setTimerActive] = useState(false);
  const [timeRemaining, setTimeRemaining] = useState(null);
  const [nextCreditAt, setNextCreditAt] = useState(null);

  const backenUrl = import.meta.env.VITE_BACKEND_URL;
  const navigate = useNavigate();

  const { getToken } = useAuth();
  const { isSignedIn } = useUser();
  const { openSignIn } = useClerk();

  // Format time remaining in hours, minutes, seconds
  const formatTimeRemaining = (milliseconds) => {
    if (!milliseconds) return "";

    const seconds = Math.floor((milliseconds / 1000) % 60);
    const minutes = Math.floor((milliseconds / (1000 * 60)) % 60);
    const hours = Math.floor(milliseconds / (1000 * 60 * 60));

    return `${hours.toString().padStart(2, "0")}:${minutes
      .toString()
      .padStart(2, "0")}:${seconds.toString().padStart(2, "0")}`;
  };

  // Timer countdown effect
  useEffect(() => {
    let interval;

    if (timerActive && timeRemaining) {
      interval = setInterval(() => {
        setTimeRemaining((prev) => {
          const newTime = prev - 1000;
          if (newTime <= 0) {
            // Reload credits data when timer reaches zero
            loadCreditsData();
            return 0;
          }
          return newTime;
        });
      }, 1000);
    }

    return () => clearInterval(interval);
  }, [timerActive, timeRemaining]);

  // Initial check of timer status when user signs in
  useEffect(() => {
    if (isSignedIn) {
      loadCreditsData();
      checkTimerStatus();
    }
  }, [isSignedIn]);

  const loadCreditsData = async () => {
    try {
      const token = await getToken();
      const { data } = await axios.get(backenUrl + "/api/user/credits", {
        headers: { token },
      });

      if (data.success) {
        setCredit(data.credits);
        setTimerActive(data.timerActive);
        setNextCreditAt(data.nextCreditAt);

        if (data.timeRemaining) {
          setTimeRemaining(data.timeRemaining);
        } else if (data.nextCreditAt) {
          setTimeRemaining(
            Math.max(0, new Date(data.nextCreditAt) - new Date())
          );
        }

        console.log("Credits data loaded:", data);
      }
    } catch (error) {
      console.log(error);
      toast.error(error.message);
    }
  };

  const checkTimerStatus = async () => {
    try {
      const token = await getToken();
      const { data } = await axios.post(
        backenUrl + "/api/user/check-timer",
        {},
        { headers: { token } }
      );

      if (data.success) {
        setTimerActive(data.timerActive);
        setNextCreditAt(data.nextCreditAt);
        setCredit(data.creditBalance);

        if (data.nextCreditAt) {
          setTimeRemaining(
            Math.max(0, new Date(data.nextCreditAt) - new Date())
          );
        }

        console.log("Timer status checked:", data);
      }
    } catch (error) {
      console.log(error);
      toast.error(error.message);
    }
  };

  const removeBg = async (image) => {
    try {
      if (!isSignedIn) {
        return openSignIn();
      }
      setImage(image);
      setResultImage(false);
      navigate("/result");

      const token = await getToken();
      const formData = new FormData();
      image && formData.append("image", image);

      const { data } = await axios.post(
        backenUrl + "/api/image/remove-bg",
        formData,
        { headers: { token } }
      );

      if (data.success) {
        setResultImage(data.resultImage);

        if (data.creditBalance !== undefined) {
          setCredit(data.creditBalance);
        }

        if (data.timerActive !== undefined) {
          setTimerActive(data.timerActive);
        }

        if (data.nextCreditAt) {
          setNextCreditAt(data.nextCreditAt);
          setTimeRemaining(
            Math.max(0, new Date(data.nextCreditAt) - new Date())
          );
        }
      } else {
        toast.error(data.message);
        if (data.creditBalance !== undefined) {
          setCredit(data.creditBalance);
        }
      }
    } catch (error) {
      console.log(error);
      toast.error(error.message);
    }
  };

  const value = {
    credit,
    setCredit,
    loadCreditsData,
    backenUrl,
    image,
    setImage,
    removeBg,
    resultImage,
    setResultImage,
    timerActive,
    timeRemaining,
    formatTimeRemaining,
    checkTimerStatus,
  };

  return (
    <AppContext.Provider value={value}>{props.children}</AppContext.Provider>
  );
};

export default AppContextProvider;
