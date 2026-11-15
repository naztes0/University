module Main where

import Control.Exception (catch)
import Database (closeDatabase, openDatabase)
import Database.MySQL.Base (MySQLError, errMessage)
import Menu (runMenu)

main :: IO ()
main = runProgram `catch` handleConnectionError

runProgram :: IO ()
runProgram = do
    putStrLn "Lab1"
    connection <- openDatabase
    putStrLn "Connected to MySQL successfully"
    runMenu connection
    closeDatabase connection

handleConnectionError :: MySQLError -> IO ()
handleConnectionError databaseError = do
    putStrLn "Could not connect to MySQL. Make sure the MySQL service is running."
    putStrLn ("Details: " ++ errMessage databaseError)
