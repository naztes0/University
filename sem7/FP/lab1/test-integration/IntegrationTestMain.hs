{-# LANGUAGE OverloadedStrings #-}
{-# LANGUAGE ScopedTypeVariables #-}

module Main where

import Control.Exception (SomeException, bracket, bracket_, displayException, throwIO, try)
import Data.List (find)
import Database
import Database.MySQL.Simple (Query, execute_)
import Models
import System.Exit (exitFailure, exitSuccess)
import Test.HUnit (Assertion, assertBool, assertEqual, assertFailure)

testDatabaseName :: String
testDatabaseName = "display_class_schedule_test"

schemaStatements :: [Query]
schemaStatements =
    [ "CREATE TABLE display_classes (id INT AUTO_INCREMENT PRIMARY KEY, name VARCHAR(100) NOT NULL, room_number VARCHAR(20) NOT NULL UNIQUE, capacity INT NOT NULL, opening_time TIME NOT NULL, closing_time TIME NOT NULL)"
    , "CREATE TABLE workstations (id INT AUTO_INCREMENT PRIMARY KEY, class_id INT NOT NULL, workstation_number VARCHAR(20) NOT NULL, status VARCHAR(20) NOT NULL, UNIQUE (class_id, workstation_number), FOREIGN KEY (class_id) REFERENCES display_classes(id))"
    , "CREATE TABLE users (id INT AUTO_INCREMENT PRIMARY KEY, full_name VARCHAR(100) NOT NULL, user_type VARCHAR(30) NOT NULL, email VARCHAR(100) UNIQUE)"
    , "CREATE TABLE teachers (id INT AUTO_INCREMENT PRIMARY KEY, full_name VARCHAR(100) NOT NULL, department VARCHAR(100) NOT NULL, email VARCHAR(100) UNIQUE)"
    , "CREATE TABLE subjects (id INT AUTO_INCREMENT PRIMARY KEY, name VARCHAR(100) NOT NULL UNIQUE)"
    , "CREATE TABLE scheduled_classes (id INT AUTO_INCREMENT PRIMARY KEY, class_id INT NOT NULL, teacher_id INT NOT NULL, subject_id INT NOT NULL, group_name VARCHAR(30) NOT NULL, lesson_date DATE NOT NULL, start_time TIME NOT NULL, end_time TIME NOT NULL, FOREIGN KEY (class_id) REFERENCES display_classes(id), FOREIGN KEY (teacher_id) REFERENCES teachers(id), FOREIGN KEY (subject_id) REFERENCES subjects(id))"
    , "CREATE TABLE free_access (id INT AUTO_INCREMENT PRIMARY KEY, class_id INT NOT NULL, access_date DATE NOT NULL, start_time TIME NOT NULL, end_time TIME NOT NULL, FOREIGN KEY (class_id) REFERENCES display_classes(id))"
    , "CREATE TABLE workstation_usage (id INT AUTO_INCREMENT PRIMARY KEY, workstation_id INT NOT NULL, user_id INT NOT NULL, started_at DATETIME NOT NULL, ended_at DATETIME, FOREIGN KEY (workstation_id) REFERENCES workstations(id), FOREIGN KEY (user_id) REFERENCES users(id))"
    ]

withDatabase :: String -> (Connection -> IO a) -> IO a
withDatabase databaseName = bracket (openDatabaseWithName databaseName) closeDatabase

prepareTestDatabase :: IO ()
prepareTestDatabase = do
    withDatabase "" $ \connection -> do
        _ <- execute_ connection "DROP DATABASE IF EXISTS display_class_schedule_test"
        _ <- execute_ connection "CREATE DATABASE display_class_schedule_test CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci"
        return ()
    withDatabase testDatabaseName $ \connection ->
        mapM_ (execute_ connection) schemaStatements

removeTestDatabase :: IO ()
removeTestDatabase =
    withDatabase "" $ \connection -> do
        _ <- execute_ connection "DROP DATABASE IF EXISTS display_class_schedule_test"
        return ()

expectRecord :: String -> Maybe a -> IO a
expectRecord message record =
    case record of
        Just value -> return value
        Nothing -> do
            assertFailure message
            fail message

runStep :: String -> IO a -> IO a
runStep name action = do
    result <- try action
    case result of
        Right value -> do
            putStrLn ("[PASS] " ++ name)
            return value
        Left (exception :: SomeException) -> do
            putStrLn ("[FAIL] " ++ name)
            putStrLn ("       " ++ displayException exception)
            throwIO exception

testDisplayClass :: Connection -> IO Int
testDisplayClass connection = do
    addDisplayClass connection "Integration class" "T-900" 10 "08:00" "18:00"
    created <- getDisplayClasses connection >>= expectRecord "Display class was not added" . find ((== "T-900") . roomNumber)
    assertEqual "Initial capacity" 10 (capacity created)
    updateDisplayClass connection (displayClassId created) "Integration class updated" "T-900" 12 "09:00" "19:00"
    updated <- getDisplayClasses connection >>= expectRecord "Display class was not updated" . find ((== "T-900") . roomNumber)
    assertEqual "Updated capacity" 12 (capacity updated)
    return (displayClassId updated)

testWorkstation :: Connection -> Int -> IO Int
testWorkstation connection classId = do
    addWorkstation connection classId "TEST-PC-01" "available"
    created <- getWorkstations connection >>= expectRecord "Workstation was not added" . find ((== "TEST-PC-01") . workstationNumber)
    assertEqual "Initial workstation status" "available" (workstationStatus created)
    updateWorkstation connection (workstationId created) classId "TEST-PC-01" "broken"
    updated <- getWorkstations connection >>= expectRecord "Workstation was not updated" . find ((== "TEST-PC-01") . workstationNumber)
    assertEqual "Updated workstation status" "broken" (workstationStatus updated)
    return (workstationId updated)

testUser :: Connection -> IO Int
testUser connection = do
    addUser connection "Integration User" "student" (Just "integration.user@example.com")
    created <- getUsers connection >>= expectRecord "User was not added" . find ((== Just "integration.user@example.com") . userEmail)
    assertEqual "Initial user type" "student" (userType created)
    updateUser connection (userId created) "Integration User Updated" "teacher" (Just "integration.user@example.com")
    updated <- getUsers connection >>= expectRecord "User was not updated" . find ((== Just "integration.user@example.com") . userEmail)
    assertEqual "Updated user type" "teacher" (userType updated)
    return (userId updated)

testTeacher :: Connection -> IO Int
testTeacher connection = do
    addTeacher connection "Integration Teacher" "Testing Department" (Just "integration.teacher@example.com")
    created <- getTeachers connection >>= expectRecord "Teacher was not added" . find ((== Just "integration.teacher@example.com") . teacherEmail)
    assertEqual "Initial department" "Testing Department" (teacherDepartment created)
    updateTeacher connection (teacherId created) "Integration Teacher" "Updated Department" (Just "integration.teacher@example.com")
    updated <- getTeachers connection >>= expectRecord "Teacher was not updated" . find ((== Just "integration.teacher@example.com") . teacherEmail)
    assertEqual "Updated department" "Updated Department" (teacherDepartment updated)
    return (teacherId updated)

testSubject :: Connection -> IO Int
testSubject connection = do
    addSubject connection "Integration Subject"
    created <- getSubjects connection >>= expectRecord "Subject was not added" . find ((== "Integration Subject") . subjectName)
    updateSubject connection (subjectId created) "Integration Subject Updated"
    updated <- getSubjects connection >>= expectRecord "Subject was not updated" . find ((== "Integration Subject Updated") . subjectName)
    assertEqual "Updated subject name" "Integration Subject Updated" (subjectName updated)
    return (subjectId updated)

testSchedule :: Connection -> Int -> Int -> Int -> IO ()
testSchedule connection classId teacherIdValue subjectIdValue = do
    addScheduledClass connection classId teacherIdValue subjectIdValue "TEST-GROUP" "2099-01-10" "10:00" "11:00"
    created <- getScheduledClasses connection >>= expectRecord "Scheduled class was not added" . find ((== "TEST-GROUP") . scheduledGroupName)
    updateScheduledClass connection (scheduledClassId created) classId teacherIdValue subjectIdValue "TEST-GROUP-UPDATED" "2099-01-11" "12:00" "13:00"
    updated <- getScheduledClasses connection >>= expectRecord "Scheduled class was not updated" . find ((== "TEST-GROUP-UPDATED") . scheduledGroupName)
    assertEqual "Updated lesson date" "2099-01-11" (lessonDate updated)

testFreeAccess :: Connection -> Int -> IO ()
testFreeAccess connection classId = do
    addFreeAccess connection classId "2099-01-12" "14:00" "16:00"
    created <- getFreeAccessPeriods connection >>= expectRecord "Free access was not added" . find ((== "2099-01-12") . accessDate)
    updateFreeAccess connection (freeAccessId created) classId "2099-01-13" "15:00" "17:00"
    updated <- getFreeAccessPeriods connection >>= expectRecord "Free access was not updated" . find ((== "2099-01-13") . accessDate)
    assertEqual "Updated free access start" "15:00" (accessStartTime updated)

testUsage :: Connection -> Int -> Int -> IO ()
testUsage connection workstationIdValue userIdValue = do
    addWorkstationUsage connection workstationIdValue userIdValue "2099-01-14 09:00:00" Nothing
    created <- getWorkstationUsages connection >>= expectRecord "Usage was not added" . find ((== "Integration User Updated") . usageUserName)
    assertEqual "Active usage has no end" Nothing (usageEndedAt created)
    updateWorkstationUsage connection (usageId created) workstationIdValue userIdValue "2099-01-14 09:00:00" (Just "2099-01-14 10:00:00")
    updated <- getWorkstationUsages connection >>= expectRecord "Usage was not updated" . find ((== usageId created) . usageId)
    assertEqual "Updated usage end" (Just "2099-01-14 10:00:00") (usageEndedAt updated)
    classLoad <- getClassLoad connection >>= expectRecord "Class workload was not calculated" . find ((== "T-900") . classLoadRoomNumber)
    assertEqual "Class usage count" 1 (classUsageCount classLoad)
    assertEqual "Class usage minutes" 60 (classUsageMinutes classLoad)
    workstationLoad <- getWorkstationLoad connection >>= expectRecord "Workstation workload was not calculated" . find ((== "TEST-PC-01") . loadWorkstationNumber)
    assertEqual "Workstation usage count" 1 (workstationUsageCount workstationLoad)
    assertEqual "Workstation usage minutes" 60 (workstationUsageMinutes workstationLoad)

runIntegrationTests :: IO ()
runIntegrationTests =
    withDatabase testDatabaseName $ \connection -> do
        classId <- runStep "Display class add/get/update" (testDisplayClass connection)
        workstationIdValue <- runStep "Workstation add/get/update" (testWorkstation connection classId)
        userIdValue <- runStep "User add/get/update" (testUser connection)
        teacherIdValue <- runStep "Teacher add/get/update" (testTeacher connection)
        subjectIdValue <- runStep "Subject add/get/update" (testSubject connection)
        runStep "Scheduled class add/get/update" (testSchedule connection classId teacherIdValue subjectIdValue)
        runStep "Free access add/get/update" (testFreeAccess connection classId)
        runStep "Workstation usage add/get/update" (testUsage connection workstationIdValue userIdValue)
        putStrLn ""
        putStrLn "Integration tests passed: 8/8"

main :: IO ()
main = do
    result <- try (bracket_ prepareTestDatabase removeTestDatabase runIntegrationTests)
    case result of
        Right _ -> exitSuccess
        Left (exception :: SomeException) -> do
            putStrLn ("Integration test suite failed: " ++ displayException exception)
            exitFailure
