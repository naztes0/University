{-# LANGUAGE OverloadedStrings #-}

module Database
    ( Connection
    , openDatabase
    , openDatabaseWithName
    , closeDatabase
    , getDisplayClasses
    , addDisplayClass
    , updateDisplayClass
    , isRoomNumberTaken
    , displayClassExists
    , getWorkstations
    , addWorkstation
    , updateWorkstation
    , isWorkstationNumberTaken
    , workstationExists
    , getUsers
    , addUser
    , updateUser
    , isUserEmailTaken
    , userExists
    , getTeachers
    , addTeacher
    , updateTeacher
    , isTeacherEmailTaken
    , teacherExists
    , getSubjects
    , addSubject
    , updateSubject
    , isSubjectNameTaken
    , subjectExists
    , getScheduledClasses
    , addScheduledClass
    , updateScheduledClass
    , scheduledClassExists
    , getFreeAccessPeriods
    , addFreeAccess
    , updateFreeAccess
    , freeAccessExists
    , getWorkstationUsages
    , addWorkstationUsage
    , updateWorkstationUsage
    , workstationUsageExists
    , getClassLoad
    , getWorkstationLoad
    ) where

import Data.Int (Int64)
import Database.MySQL.Simple
    ( Connection
    , ConnectInfo (..)
    , Only (Only)
    , close
    , connect
    , defaultConnectInfo
    , execute
    , query
    , query_
    )
import Models

openDatabase :: IO Connection
openDatabase = openDatabaseWithName "display_class_schedule"

openDatabaseWithName :: String -> IO Connection
openDatabaseWithName databaseName =
    connect
        defaultConnectInfo
            { connectHost = "127.0.0.1"
            , connectUser = "root"
            , connectPassword = ""
            , connectDatabase = databaseName
            }

closeDatabase :: Connection -> IO ()
closeDatabase = close

getDisplayClasses :: Connection -> IO [DisplayClass]
getDisplayClasses connection = do
    rows <- query_ connection
        "SELECT id, name, room_number, capacity, TIME_FORMAT(opening_time, '%H:%i'), TIME_FORMAT(closing_time, '%H:%i') FROM display_classes ORDER BY id"
        :: IO [(Int, String, String, Int, String, String)]
    return
        [ DisplayClass classId name room classCapacity opens closes
        | (classId, name, room, classCapacity, opens, closes) <- rows
        ]

addDisplayClass :: Connection -> String -> String -> Int -> String -> String -> IO ()
addDisplayClass connection name room classCapacity opens closes = do
    _ <- execute connection
        "INSERT INTO display_classes (name, room_number, capacity, opening_time, closing_time) VALUES (?, ?, ?, ?, ?)"
        (name, room, classCapacity, opens, closes)
    return ()

updateDisplayClass :: Connection -> Int -> String -> String -> Int -> String -> String -> IO ()
updateDisplayClass connection classId name room classCapacity opens closes = do
    _ <- execute connection
        "UPDATE display_classes SET name = ?, room_number = ?, capacity = ?, opening_time = ?, closing_time = ? WHERE id = ?"
        (name, room, classCapacity, opens, closes, classId)
    return ()

isRoomNumberTaken :: Connection -> String -> Maybe Int -> IO Bool
isRoomNumberTaken connection room excludedId = do
    rows <- case excludedId of
        Nothing -> query connection
            "SELECT id FROM display_classes WHERE room_number = ? LIMIT 1"
            (Only room)
        Just classId -> query connection
            "SELECT id FROM display_classes WHERE room_number = ? AND id <> ? LIMIT 1"
            (room, classId)
    return (not (null (rows :: [Only Int])))

displayClassExists :: Connection -> Int -> IO Bool
displayClassExists connection classId = do
    rows <- query connection
        "SELECT id FROM display_classes WHERE id = ? LIMIT 1"
        (Only classId)
    return (not (null (rows :: [Only Int])))

getWorkstations :: Connection -> IO [Workstation]
getWorkstations connection = do
    rows <- query_ connection
        "SELECT w.id, c.room_number, w.workstation_number, w.status FROM workstations w JOIN display_classes c ON c.id = w.class_id ORDER BY w.id"
        :: IO [(Int, String, String, String)]
    return
        [ Workstation workstationIdValue room number status
        | (workstationIdValue, room, number, status) <- rows
        ]

addWorkstation :: Connection -> Int -> String -> String -> IO ()
addWorkstation connection classId number status = do
    _ <- execute connection
        "INSERT INTO workstations (class_id, workstation_number, status) VALUES (?, ?, ?)"
        (classId, number, status)
    return ()

updateWorkstation :: Connection -> Int -> Int -> String -> String -> IO ()
updateWorkstation connection workstationIdValue classId number status = do
    _ <- execute connection
        "UPDATE workstations SET class_id = ?, workstation_number = ?, status = ? WHERE id = ?"
        (classId, number, status, workstationIdValue)
    return ()

isWorkstationNumberTaken :: Connection -> Int -> String -> Maybe Int -> IO Bool
isWorkstationNumberTaken connection classId number excludedId = do
    rows <- case excludedId of
        Nothing -> query connection
            "SELECT id FROM workstations WHERE class_id = ? AND workstation_number = ? LIMIT 1"
            (classId, number)
        Just workstationIdValue -> query connection
            "SELECT id FROM workstations WHERE class_id = ? AND workstation_number = ? AND id <> ? LIMIT 1"
            (classId, number, workstationIdValue)
    return (not (null (rows :: [Only Int])))

workstationExists :: Connection -> Int -> IO Bool
workstationExists connection workstationIdValue = do
    rows <- query connection
        "SELECT id FROM workstations WHERE id = ? LIMIT 1"
        (Only workstationIdValue)
    return (not (null (rows :: [Only Int])))

getUsers :: Connection -> IO [User]
getUsers connection = do
    rows <- query_ connection
        "SELECT id, full_name, user_type, email FROM users ORDER BY id"
        :: IO [(Int, String, String, Maybe String)]
    return
        [ User userIdValue fullName userTypeValue email
        | (userIdValue, fullName, userTypeValue, email) <- rows
        ]

addUser :: Connection -> String -> String -> Maybe String -> IO ()
addUser connection fullName userTypeValue email = do
    _ <- execute connection
        "INSERT INTO users (full_name, user_type, email) VALUES (?, ?, ?)"
        (fullName, userTypeValue, email)
    return ()

updateUser :: Connection -> Int -> String -> String -> Maybe String -> IO ()
updateUser connection userIdValue fullName userTypeValue email = do
    _ <- execute connection
        "UPDATE users SET full_name = ?, user_type = ?, email = ? WHERE id = ?"
        (fullName, userTypeValue, email, userIdValue)
    return ()

isUserEmailTaken :: Connection -> String -> Maybe Int -> IO Bool
isUserEmailTaken connection email excludedId = do
    rows <- case excludedId of
        Nothing -> query connection
            "SELECT id FROM users WHERE email = ? LIMIT 1"
            (Only email)
        Just userIdValue -> query connection
            "SELECT id FROM users WHERE email = ? AND id <> ? LIMIT 1"
            (email, userIdValue)
    return (not (null (rows :: [Only Int])))

userExists :: Connection -> Int -> IO Bool
userExists connection userIdValue = do
    rows <- query connection
        "SELECT id FROM users WHERE id = ? LIMIT 1"
        (Only userIdValue)
    return (not (null (rows :: [Only Int])))

getTeachers :: Connection -> IO [Teacher]
getTeachers connection = do
    rows <- query_ connection
        "SELECT id, full_name, department, email FROM teachers ORDER BY id"
        :: IO [(Int, String, String, Maybe String)]
    return
        [ Teacher teacherIdValue fullName department email
        | (teacherIdValue, fullName, department, email) <- rows
        ]

addTeacher :: Connection -> String -> String -> Maybe String -> IO ()
addTeacher connection fullName department email = do
    _ <- execute connection
        "INSERT INTO teachers (full_name, department, email) VALUES (?, ?, ?)"
        (fullName, department, email)
    return ()

updateTeacher :: Connection -> Int -> String -> String -> Maybe String -> IO ()
updateTeacher connection teacherIdValue fullName department email = do
    _ <- execute connection
        "UPDATE teachers SET full_name = ?, department = ?, email = ? WHERE id = ?"
        (fullName, department, email, teacherIdValue)
    return ()

isTeacherEmailTaken :: Connection -> String -> Maybe Int -> IO Bool
isTeacherEmailTaken connection email excludedId = do
    rows <- case excludedId of
        Nothing -> query connection
            "SELECT id FROM teachers WHERE email = ? LIMIT 1"
            (Only email)
        Just teacherIdValue -> query connection
            "SELECT id FROM teachers WHERE email = ? AND id <> ? LIMIT 1"
            (email, teacherIdValue)
    return (not (null (rows :: [Only Int])))

teacherExists :: Connection -> Int -> IO Bool
teacherExists connection teacherIdValue = do
    rows <- query connection
        "SELECT id FROM teachers WHERE id = ? LIMIT 1"
        (Only teacherIdValue)
    return (not (null (rows :: [Only Int])))

getSubjects :: Connection -> IO [Subject]
getSubjects connection = do
    rows <- query_ connection
        "SELECT id, name FROM subjects ORDER BY id"
        :: IO [(Int, String)]
    return [Subject subjectIdValue name | (subjectIdValue, name) <- rows]

addSubject :: Connection -> String -> IO ()
addSubject connection name = do
    _ <- execute connection "INSERT INTO subjects (name) VALUES (?)" (Only name)
    return ()

updateSubject :: Connection -> Int -> String -> IO ()
updateSubject connection subjectIdValue name = do
    _ <- execute connection
        "UPDATE subjects SET name = ? WHERE id = ?"
        (name, subjectIdValue)
    return ()

isSubjectNameTaken :: Connection -> String -> Maybe Int -> IO Bool
isSubjectNameTaken connection name excludedId = do
    rows <- case excludedId of
        Nothing -> query connection
            "SELECT id FROM subjects WHERE name = ? LIMIT 1"
            (Only name)
        Just subjectIdValue -> query connection
            "SELECT id FROM subjects WHERE name = ? AND id <> ? LIMIT 1"
            (name, subjectIdValue)
    return (not (null (rows :: [Only Int])))

subjectExists :: Connection -> Int -> IO Bool
subjectExists connection subjectIdValue = do
    rows <- query connection
        "SELECT id FROM subjects WHERE id = ? LIMIT 1"
        (Only subjectIdValue)
    return (not (null (rows :: [Only Int])))

getScheduledClasses :: Connection -> IO [ScheduledClass]
getScheduledClasses connection = do
    rows <- query_ connection
        "SELECT sc.id, c.room_number, t.full_name, s.name, sc.group_name, DATE_FORMAT(sc.lesson_date, '%Y-%m-%d'), TIME_FORMAT(sc.start_time, '%H:%i'), TIME_FORMAT(sc.end_time, '%H:%i') FROM scheduled_classes sc JOIN display_classes c ON c.id = sc.class_id JOIN teachers t ON t.id = sc.teacher_id JOIN subjects s ON s.id = sc.subject_id ORDER BY sc.lesson_date, sc.start_time"
        :: IO [(Int, String, String, String, String, String, String, String)]
    return
        [ ScheduledClass scheduleId room teacherNameValue subjectNameValue groupName date starts ends
        | (scheduleId, room, teacherNameValue, subjectNameValue, groupName, date, starts, ends) <- rows
        ]

addScheduledClass :: Connection -> Int -> Int -> Int -> String -> String -> String -> String -> IO ()
addScheduledClass connection classId teacherIdValue subjectIdValue groupName date starts ends = do
    _ <- execute connection
        "INSERT INTO scheduled_classes (class_id, teacher_id, subject_id, group_name, lesson_date, start_time, end_time) VALUES (?, ?, ?, ?, ?, ?, ?)"
        (classId, teacherIdValue, subjectIdValue, groupName, date, starts, ends)
    return ()

updateScheduledClass :: Connection -> Int -> Int -> Int -> Int -> String -> String -> String -> String -> IO ()
updateScheduledClass connection scheduleId classId teacherIdValue subjectIdValue groupName date starts ends = do
    _ <- execute connection
        "UPDATE scheduled_classes SET class_id = ?, teacher_id = ?, subject_id = ?, group_name = ?, lesson_date = ?, start_time = ?, end_time = ? WHERE id = ?"
        (classId, teacherIdValue, subjectIdValue, groupName, date, starts, ends, scheduleId)
    return ()

scheduledClassExists :: Connection -> Int -> IO Bool
scheduledClassExists connection scheduleId = do
    rows <- query connection
        "SELECT id FROM scheduled_classes WHERE id = ? LIMIT 1"
        (Only scheduleId)
    return (not (null (rows :: [Only Int])))

getFreeAccessPeriods :: Connection -> IO [FreeAccess]
getFreeAccessPeriods connection = do
    rows <- query_ connection
        "SELECT fa.id, c.room_number, DATE_FORMAT(fa.access_date, '%Y-%m-%d'), TIME_FORMAT(fa.start_time, '%H:%i'), TIME_FORMAT(fa.end_time, '%H:%i') FROM free_access fa JOIN display_classes c ON c.id = fa.class_id ORDER BY fa.access_date, fa.start_time"
        :: IO [(Int, String, String, String, String)]
    return
        [ FreeAccess freeAccessIdValue room date starts ends
        | (freeAccessIdValue, room, date, starts, ends) <- rows
        ]

addFreeAccess :: Connection -> Int -> String -> String -> String -> IO ()
addFreeAccess connection classId date starts ends = do
    _ <- execute connection
        "INSERT INTO free_access (class_id, access_date, start_time, end_time) VALUES (?, ?, ?, ?)"
        (classId, date, starts, ends)
    return ()

updateFreeAccess :: Connection -> Int -> Int -> String -> String -> String -> IO ()
updateFreeAccess connection freeAccessIdValue classId date starts ends = do
    _ <- execute connection
        "UPDATE free_access SET class_id = ?, access_date = ?, start_time = ?, end_time = ? WHERE id = ?"
        (classId, date, starts, ends, freeAccessIdValue)
    return ()

freeAccessExists :: Connection -> Int -> IO Bool
freeAccessExists connection freeAccessIdValue = do
    rows <- query connection
        "SELECT id FROM free_access WHERE id = ? LIMIT 1"
        (Only freeAccessIdValue)
    return (not (null (rows :: [Only Int])))

getWorkstationUsages :: Connection -> IO [WorkstationUsage]
getWorkstationUsages connection = do
    rows <- query_ connection
        "SELECT wu.id, w.workstation_number, c.room_number, u.full_name, DATE_FORMAT(wu.started_at, '%Y-%m-%d %H:%i:%s'), DATE_FORMAT(wu.ended_at, '%Y-%m-%d %H:%i:%s') FROM workstation_usage wu JOIN workstations w ON w.id = wu.workstation_id JOIN display_classes c ON c.id = w.class_id JOIN users u ON u.id = wu.user_id ORDER BY wu.started_at"
        :: IO [(Int, String, String, String, String, Maybe String)]
    return
        [ WorkstationUsage usageIdValue workstationNumberValue room userNameValue startedAt endedAt
        | (usageIdValue, workstationNumberValue, room, userNameValue, startedAt, endedAt) <- rows
        ]

addWorkstationUsage :: Connection -> Int -> Int -> String -> Maybe String -> IO ()
addWorkstationUsage connection workstationIdValue userIdValue startedAt endedAt = do
    _ <- execute connection
        "INSERT INTO workstation_usage (workstation_id, user_id, started_at, ended_at) VALUES (?, ?, ?, ?)"
        (workstationIdValue, userIdValue, startedAt, endedAt)
    return ()

updateWorkstationUsage :: Connection -> Int -> Int -> Int -> String -> Maybe String -> IO ()
updateWorkstationUsage connection usageIdValue workstationIdValue userIdValue startedAt endedAt = do
    _ <- execute connection
        "UPDATE workstation_usage SET workstation_id = ?, user_id = ?, started_at = ?, ended_at = ? WHERE id = ?"
        (workstationIdValue, userIdValue, startedAt, endedAt, usageIdValue)
    return ()

workstationUsageExists :: Connection -> Int -> IO Bool
workstationUsageExists connection usageIdValue = do
    rows <- query connection
        "SELECT id FROM workstation_usage WHERE id = ? LIMIT 1"
        (Only usageIdValue)
    return (not (null (rows :: [Only Int])))

getClassLoad :: Connection -> IO [ClassLoad]
getClassLoad connection = do
    rows <- query_ connection
        "SELECT c.room_number, COUNT(wu.id), CAST(COALESCE(SUM(TIMESTAMPDIFF(MINUTE, wu.started_at, COALESCE(wu.ended_at, NOW()))), 0) AS SIGNED) FROM display_classes c LEFT JOIN workstations w ON w.class_id = c.id LEFT JOIN workstation_usage wu ON wu.workstation_id = w.id GROUP BY c.id, c.room_number ORDER BY c.room_number"
        :: IO [(String, Int64, Int64)]
    return
        [ ClassLoad room (fromIntegral usageCount) (fromIntegral minutes)
        | (room, usageCount, minutes) <- rows
        ]

getWorkstationLoad :: Connection -> IO [WorkstationLoad]
getWorkstationLoad connection = do
    rows <- query_ connection
        "SELECT w.workstation_number, c.room_number, COUNT(wu.id), CAST(COALESCE(SUM(TIMESTAMPDIFF(MINUTE, wu.started_at, COALESCE(wu.ended_at, NOW()))), 0) AS SIGNED) FROM workstations w JOIN display_classes c ON c.id = w.class_id LEFT JOIN workstation_usage wu ON wu.workstation_id = w.id GROUP BY w.id, w.workstation_number, c.room_number ORDER BY c.room_number, w.workstation_number"
        :: IO [(String, String, Int64, Int64)]
    return
        [ WorkstationLoad number room (fromIntegral usageCount) (fromIntegral minutes)
        | (number, room, usageCount, minutes) <- rows
        ]
