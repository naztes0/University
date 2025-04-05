import React, { useContext } from "react";
import { assets } from "../assets/assets";
import { AppContext } from "../context/AppContext";
import { useNavigate } from "react-router-dom";

const Result = () => {
  const { resultImage, image } = useContext(AppContext);
  const navigate= useNavigate()
  
  

  return (
    <div className="mx-4 my-2 lg:mx-44 mt-9 mb-14 min-h-[80vh] sm:min-h-[75vh]">
      <div className="bg-white rounded-lg px-8 py-10 pb-12 drop-shadow-md">
        {/* Image Container */}
        <div className="flex flex-col sm:grid grid-cols-2 gap-8 ">
          {/* Left Side */}
          <div>
            <p className="font-semibold text-gray-600 mb-2">Original</p>
            <img
              className="rounded-md border"
              src={image ? URL.createObjectURL(image) : ""}
              alt=""
            />
          </div>
          {/* Right Side */}
          <div className="flex flex-col">
            <p className="font-semibold text-gray-600 mb-2">
              Background Removed
            </p>
            <div className="rounded-md border border-gray-300 h-full relative bg-layer overflow-hidden">
              <img src={resultImage ? resultImage : ""} alt="" />
              {!resultImage&&image && (
                <div className=" flex justify-center items-center p-4 h-full">
                  <div className="border-4 border-violet-600 rounded-full h-12 w-12 border-t-transparent animate-spin"></div>
                </div>
              )}
            </div>
          </div>
        </div>

        {/* BUTTONS */}
        {resultImage && (
          <div className="flex justify-center sm:justify-end items-center flex-wrap mt-8 gap-4">
            <button onClick={()=>navigate("/")} className="px-8 py-2.5 text-violet-600 text-sm border border-violet-600 rounded-full hover:scale-105 transition-all duration-500">
              Try another image
            </button>
            <a
              href={resultImage}
              download
              className="px-8 py-2.5 bg-gradient-to-tr from-violet-700 to-fuchsia-500 text-white rounded-full hover:scale-105 transition-all duration-500"
            >
              Download image
            </a>
          </div>
        )}
      </div>
    </div>
  );
};

export default Result;
