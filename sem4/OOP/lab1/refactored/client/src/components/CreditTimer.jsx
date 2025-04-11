import React, { useContext } from "react";
import { AppContext } from "../context/AppContext";

const CreditTimer = () => {
  const { timerActive, timeRemaining, formatTimeRemaining, credit } =
    useContext(AppContext);

  if (!timerActive || credit >= 5) {
    return null;
  }
  return (
    <div className="fixed bottom-16 right-8 bg-white rounded-lg shadow-lg p-3 z-50 border-2 border-violet-400">
      <div className="flex flex-col items-center">
        <p className="text-xs text-gray-500 mb-1">Next credit in</p>
        <p className="text-lg font-bold text-violet-600">
          {formatTimeRemaining(timeRemaining)}
        </p>
      </div>
    </div>
  );
};

export default CreditTimer;
