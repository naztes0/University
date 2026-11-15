module Menu (runMenu) where

import Control.Exception (Exception, catch, throwIO)
import Database
import Database.MySQL.Base (MySQLError, errMessage, errNumber)
import Models (Printable (printInfo))
import System.IO (hFlush, stdout)
import Validation

data ActionCancelled = ActionCancelled
    deriving (Show)

instance Exception ActionCancelled

runMenu :: Connection -> IO ()
runMenu connection = do
    putStrLn ""
    putStrLn "=== Main menu ==="
    putStrLn "1. Display classes"
    putStrLn "2. Workstations"
    putStrLn "3. Users"
    putStrLn "4. Teachers"
    putStrLn "5. Subjects"
    putStrLn "6. Scheduled classes"
    putStrLn "7. Free access"
    putStrLn "8. Workstation usage"
    putStrLn "9. Workload report"
    putStrLn "0. Exit"
    choice <- prompt "Choice: "
    case choice of
        "1" -> entityMenu "Display classes" (showDisplayClasses connection) (addDisplayClassAction connection) (updateDisplayClassAction connection) >> runMenu connection
        "2" -> entityMenu "Workstations" (showWorkstations connection) (addWorkstationAction connection) (updateWorkstationAction connection) >> runMenu connection
        "3" -> entityMenu "Users" (showUsers connection) (addUserAction connection) (updateUserAction connection) >> runMenu connection
        "4" -> entityMenu "Teachers" (showTeachers connection) (addTeacherAction connection) (updateTeacherAction connection) >> runMenu connection
        "5" -> entityMenu "Subjects" (showSubjects connection) (addSubjectAction connection) (updateSubjectAction connection) >> runMenu connection
        "6" -> entityMenu "Scheduled classes" (showScheduledClasses connection) (addScheduledClassAction connection) (updateScheduledClassAction connection) >> runMenu connection
        "7" -> entityMenu "Free access" (showFreeAccess connection) (addFreeAccessAction connection) (updateFreeAccessAction connection) >> runMenu connection
        "8" -> entityMenu "Workstation usage" (showWorkstationUsages connection) (addWorkstationUsageAction connection) (updateWorkstationUsageAction connection) >> runMenu connection
        "9" -> runDatabaseAction (showWorkloadReport connection) >> runMenu connection
        "0" -> putStrLn "Program finished."
        _ -> putStrLn "Unknown option. Try again." >> runMenu connection

entityMenu :: String -> IO () -> IO () -> IO () -> IO ()
entityMenu title showAction addAction updateAction = do
    putStrLn ""
    putStrLn ("=== " ++ title ++ " ===")
    putStrLn "1. Show all"
    putStrLn "2. Add"
    putStrLn "3. Update"
    putStrLn "0. Back"
    choice <- prompt "Choice: "
    case choice of
        "1" -> runDatabaseAction showAction >> entityMenu title showAction addAction updateAction
        "2" -> runMenuAction addAction >> entityMenu title showAction addAction updateAction
        "3" -> runMenuAction updateAction >> entityMenu title showAction addAction updateAction
        "0" -> return ()
        _ -> putStrLn "Unknown option. Try again." >> entityMenu title showAction addAction updateAction

runDatabaseAction :: IO () -> IO ()
runDatabaseAction action = catch action handleDatabaseError

runMenuAction :: IO () -> IO ()
runMenuAction action = catch (runDatabaseAction action) handleActionCancelled

handleActionCancelled :: ActionCancelled -> IO ()
handleActionCancelled _ = do
    putStrLn ""
    putStrLn "Action cancelled."

handleDatabaseError :: MySQLError -> IO ()
handleDatabaseError databaseError = do
    putStrLn ""
    putStrLn (databaseErrorMessage (errNumber databaseError) (errMessage databaseError))

databaseErrorMessage :: Int -> String -> String
databaseErrorMessage errorNumber originalMessage
    | errorNumber == 1062 = "Error: a record with this unique value already exists."
    | errorNumber == 1452 = "Error: the specified related ID does not exist."
    | errorNumber == 1292 = "Error: invalid date or time format."
    | errorNumber == 1406 = "Error: one of the entered values is too long."
    | otherwise = "Database error: " ++ originalMessage

prompt :: String -> IO String
prompt message = do
    putStr message
    hFlush stdout
    getLine

promptAction :: String -> IO String
promptAction message = do
    rawValue <- prompt message
    let value = trim rawValue
    if isCancelCommand rawValue
        then throwIO ActionCancelled
        else return value

promptRequired :: String -> IO String
promptRequired message = do
    value <- promptAction message
    if not (isRequiredValue value)
        then do
            putStrLn "This field cannot be empty."
            promptRequired message
        else return value

promptInt :: String -> IO Int
promptInt message = do
    value <- promptAction message
    case reads value of
        [(number, "")] -> return number
        _ -> do
            putStrLn "Enter a whole number."
            promptInt message

promptPositiveInt :: String -> IO Int
promptPositiveInt message = do
    number <- promptInt message
    if isPositiveNumber number
        then return number
        else do
            putStrLn "Enter a number greater than zero."
            promptPositiveInt message

promptExistingId :: String -> (Int -> IO Bool) -> IO Int
promptExistingId message exists = do
    recordId <- promptPositiveInt message
    found <- exists recordId
    if found
        then return recordId
        else do
            putStrLn "No record with this ID exists."
            promptExistingId message exists

promptAllowed :: String -> [String] -> IO String
promptAllowed message allowedValues = do
    value <- promptRequired message
    case normalizeAllowed allowedValues value of
        Just normalizedValue -> return normalizedValue
        Nothing -> do
            putStrLn ("Allowed values: " ++ unwords allowedValues)
            promptAllowed message allowedValues

promptOptional :: String -> IO (Maybe String)
promptOptional message = do
    value <- promptAction message
    if null value
        then return Nothing
        else return (Just value)

promptDate :: String -> IO String
promptDate message = do
    value <- promptRequired message
    if isValidDate value
        then return value
        else do
            putStrLn "Enter a valid date in YYYY-MM-DD format."
            promptDate message

promptTime :: String -> IO String
promptTime message = do
    value <- promptRequired message
    case normalizeTime value of
        Just normalizedValue -> return normalizedValue
        Nothing -> do
            putStrLn "Enter a valid time in HH:MM format."
            promptTime message

promptEndTime :: String -> String -> IO String
promptEndTime startValue message = do
    endValue <- promptTime message
    if isEndTimeAfter startValue endValue
        then return endValue
        else do
            putStrLn "End time must be later than start time."
            promptEndTime startValue message

promptDateTime :: String -> IO String
promptDateTime message = do
    value <- promptRequired message
    case normalizeDateTime value of
        Just normalizedValue -> return normalizedValue
        Nothing -> do
            putStrLn "Enter a valid date and time in YYYY-MM-DD HH:MM:SS format."
            promptDateTime message

promptOptionalEndDateTime :: String -> String -> IO (Maybe String)
promptOptionalEndDateTime startValue message = do
    endValue <- promptOptional message
    case endValue of
        Nothing -> return Nothing
        Just value ->
            case normalizeDateTime value of
                Just normalizedValue
                    | isEndDateTimeAfter startValue normalizedValue -> return (Just normalizedValue)
                _ -> do
                    putStrLn "Enter a valid end date and time later than the start."
                    promptOptionalEndDateTime startValue message

promptRoomNumber :: Connection -> Maybe Int -> String -> IO String
promptRoomNumber connection excludedId message = do
    room <- promptRequired message
    taken <- isRoomNumberTaken connection room excludedId
    if taken
        then do
            putStrLn "A class with this room number already exists. Enter another room number."
            promptRoomNumber connection excludedId message
        else return room

promptWorkstationNumber :: Connection -> Int -> Maybe Int -> String -> IO String
promptWorkstationNumber connection classId excludedId message = do
    number <- promptRequired message
    taken <- isWorkstationNumberTaken connection classId number excludedId
    if taken
        then do
            putStrLn "This workstation number already exists in the selected class. Enter another number."
            promptWorkstationNumber connection classId excludedId message
        else return number

promptUserEmail :: Connection -> Maybe Int -> String -> IO (Maybe String)
promptUserEmail connection excludedId message = do
    email <- promptOptional message
    case email of
        Nothing -> return Nothing
        Just value -> do
            if not (isValidEmail value)
                then do
                    putStrLn "Enter a valid email, for example user@example.com."
                    promptUserEmail connection excludedId message
                else do
                    taken <- isUserEmailTaken connection value excludedId
                    if taken
                        then do
                            putStrLn "A user with this email already exists. Enter another email."
                            promptUserEmail connection excludedId message
                        else return email

promptTeacherEmail :: Connection -> Maybe Int -> String -> IO (Maybe String)
promptTeacherEmail connection excludedId message = do
    email <- promptOptional message
    case email of
        Nothing -> return Nothing
        Just value -> do
            if not (isValidEmail value)
                then do
                    putStrLn "Enter a valid email, for example teacher@example.com."
                    promptTeacherEmail connection excludedId message
                else do
                    taken <- isTeacherEmailTaken connection value excludedId
                    if taken
                        then do
                            putStrLn "A teacher with this email already exists. Enter another email."
                            promptTeacherEmail connection excludedId message
                        else return email

promptSubjectName :: Connection -> Maybe Int -> String -> IO String
promptSubjectName connection excludedId message = do
    name <- promptRequired message
    taken <- isSubjectNameTaken connection name excludedId
    if taken
        then do
            putStrLn "A subject with this name already exists. Enter another name."
            promptSubjectName connection excludedId message
        else return name

printRecords :: Printable a => [a] -> IO ()
printRecords [] = do
    putStrLn ""
    putStrLn "No records found."
    putStrLn ""
printRecords records = do
    putStrLn ""
    mapM_ (putStrLn . printInfo) records
    putStrLn ""

actionTitle :: String -> IO ()
actionTitle title = do
    putStrLn ""
    putStrLn ("--- " ++ title ++ " ---")
    putStrLn "Type 0 or cancel at any prompt to go back."
    putStrLn ""

successMessage :: String -> IO ()
successMessage message = do
    putStrLn ""
    putStrLn message

showDisplayClasses :: Connection -> IO ()
showDisplayClasses connection = getDisplayClasses connection >>= printRecords

addDisplayClassAction :: Connection -> IO ()
addDisplayClassAction connection = do
    actionTitle "Add display class"
    name <- promptRequired "Name: "
    room <- promptRoomNumber connection Nothing "Room number: "
    classCapacity <- promptPositiveInt "Capacity: "
    opens <- promptTime "Opening time (HH:MM): "
    closes <- promptEndTime opens "Closing time (HH:MM): "
    addDisplayClass connection name room classCapacity opens closes
    successMessage "Display class added."

updateDisplayClassAction :: Connection -> IO ()
updateDisplayClassAction connection = do
    actionTitle "Update display class"
    classId <- promptExistingId "ID to update: " (displayClassExists connection)
    name <- promptRequired "New name: "
    room <- promptRoomNumber connection (Just classId) "New room number: "
    classCapacity <- promptPositiveInt "New capacity: "
    opens <- promptTime "New opening time (HH:MM): "
    closes <- promptEndTime opens "New closing time (HH:MM): "
    updateDisplayClass connection classId name room classCapacity opens closes
    successMessage "Display class updated."

showWorkstations :: Connection -> IO ()
showWorkstations connection = getWorkstations connection >>= printRecords

addWorkstationAction :: Connection -> IO ()
addWorkstationAction connection = do
    actionTitle "Add workstation"
    classId <- promptExistingId "Display class ID: " (displayClassExists connection)
    number <- promptWorkstationNumber connection classId Nothing "Workstation number: "
    status <- promptAllowed "Status (available/maintenance/broken): " ["available", "maintenance", "broken"]
    addWorkstation connection classId number status
    successMessage "Workstation added."

updateWorkstationAction :: Connection -> IO ()
updateWorkstationAction connection = do
    actionTitle "Update workstation"
    workstationId <- promptExistingId "ID to update: " (workstationExists connection)
    classId <- promptExistingId "New display class ID: " (displayClassExists connection)
    number <- promptWorkstationNumber connection classId (Just workstationId) "New workstation number: "
    status <- promptAllowed "New status (available/maintenance/broken): " ["available", "maintenance", "broken"]
    updateWorkstation connection workstationId classId number status
    successMessage "Workstation updated."

showUsers :: Connection -> IO ()
showUsers connection = getUsers connection >>= printRecords

addUserAction :: Connection -> IO ()
addUserAction connection = do
    actionTitle "Add user"
    fullName <- promptRequired "Full name: "
    userType <- promptAllowed "User type (student/teacher): " ["student", "teacher"]
    email <- promptUserEmail connection Nothing "Email (leave empty if not specified): "
    addUser connection fullName userType email
    successMessage "User added."

updateUserAction :: Connection -> IO ()
updateUserAction connection = do
    actionTitle "Update user"
    userId <- promptExistingId "ID to update: " (userExists connection)
    fullName <- promptRequired "New full name: "
    userType <- promptAllowed "New user type (student/teacher): " ["student", "teacher"]
    email <- promptUserEmail connection (Just userId) "New email (leave empty if not specified): "
    updateUser connection userId fullName userType email
    successMessage "User updated."

showTeachers :: Connection -> IO ()
showTeachers connection = getTeachers connection >>= printRecords

addTeacherAction :: Connection -> IO ()
addTeacherAction connection = do
    actionTitle "Add teacher"
    fullName <- promptRequired "Full name: "
    department <- promptRequired "Department: "
    email <- promptTeacherEmail connection Nothing "Email (leave empty if not specified): "
    addTeacher connection fullName department email
    successMessage "Teacher added."

updateTeacherAction :: Connection -> IO ()
updateTeacherAction connection = do
    actionTitle "Update teacher"
    teacherId <- promptExistingId "ID to update: " (teacherExists connection)
    fullName <- promptRequired "New full name: "
    department <- promptRequired "New department: "
    email <- promptTeacherEmail connection (Just teacherId) "New email (leave empty if not specified): "
    updateTeacher connection teacherId fullName department email
    successMessage "Teacher updated."

showSubjects :: Connection -> IO ()
showSubjects connection = getSubjects connection >>= printRecords

addSubjectAction :: Connection -> IO ()
addSubjectAction connection = do
    actionTitle "Add subject"
    name <- promptSubjectName connection Nothing "Subject name: "
    addSubject connection name
    successMessage "Subject added."

updateSubjectAction :: Connection -> IO ()
updateSubjectAction connection = do
    actionTitle "Update subject"
    subjectId <- promptExistingId "ID to update: " (subjectExists connection)
    name <- promptSubjectName connection (Just subjectId) "New subject name: "
    updateSubject connection subjectId name
    successMessage "Subject updated."

showScheduledClasses :: Connection -> IO ()
showScheduledClasses connection = getScheduledClasses connection >>= printRecords

addScheduledClassAction :: Connection -> IO ()
addScheduledClassAction connection = do
    actionTitle "Add scheduled class"
    classId <- promptExistingId "Display class ID: " (displayClassExists connection)
    teacherId <- promptExistingId "Teacher ID: " (teacherExists connection)
    subjectId <- promptExistingId "Subject ID: " (subjectExists connection)
    groupName <- promptRequired "Group name: "
    date <- promptDate "Lesson date (YYYY-MM-DD): "
    starts <- promptTime "Start time (HH:MM): "
    ends <- promptEndTime starts "End time (HH:MM): "
    addScheduledClass connection classId teacherId subjectId groupName date starts ends
    successMessage "Scheduled class added."

updateScheduledClassAction :: Connection -> IO ()
updateScheduledClassAction connection = do
    actionTitle "Update scheduled class"
    scheduleId <- promptExistingId "ID to update: " (scheduledClassExists connection)
    classId <- promptExistingId "New display class ID: " (displayClassExists connection)
    teacherId <- promptExistingId "New teacher ID: " (teacherExists connection)
    subjectId <- promptExistingId "New subject ID: " (subjectExists connection)
    groupName <- promptRequired "New group name: "
    date <- promptDate "New lesson date (YYYY-MM-DD): "
    starts <- promptTime "New start time (HH:MM): "
    ends <- promptEndTime starts "New end time (HH:MM): "
    updateScheduledClass connection scheduleId classId teacherId subjectId groupName date starts ends
    successMessage "Scheduled class updated."

showFreeAccess :: Connection -> IO ()
showFreeAccess connection = getFreeAccessPeriods connection >>= printRecords

addFreeAccessAction :: Connection -> IO ()
addFreeAccessAction connection = do
    actionTitle "Add free access period"
    classId <- promptExistingId "Display class ID: " (displayClassExists connection)
    date <- promptDate "Date (YYYY-MM-DD): "
    starts <- promptTime "Start time (HH:MM): "
    ends <- promptEndTime starts "End time (HH:MM): "
    addFreeAccess connection classId date starts ends
    successMessage "Free access period added."

updateFreeAccessAction :: Connection -> IO ()
updateFreeAccessAction connection = do
    actionTitle "Update free access period"
    freeAccessId <- promptExistingId "ID to update: " (freeAccessExists connection)
    classId <- promptExistingId "New display class ID: " (displayClassExists connection)
    date <- promptDate "New date (YYYY-MM-DD): "
    starts <- promptTime "New start time (HH:MM): "
    ends <- promptEndTime starts "New end time (HH:MM): "
    updateFreeAccess connection freeAccessId classId date starts ends
    successMessage "Free access period updated."

showWorkstationUsages :: Connection -> IO ()
showWorkstationUsages connection = getWorkstationUsages connection >>= printRecords

showWorkloadReport :: Connection -> IO ()
showWorkloadReport connection = do
    putStrLn ""
    putStrLn "=== Workload report ==="
    putStrLn ""
    putStrLn "Display classes:"
    getClassLoad connection >>= printRecords
    putStrLn "Workstations:"
    getWorkstationLoad connection >>= printRecords

addWorkstationUsageAction :: Connection -> IO ()
addWorkstationUsageAction connection = do
    actionTitle "Add workstation usage"
    workstationId <- promptExistingId "Workstation ID: " (workstationExists connection)
    userId <- promptExistingId "User ID: " (userExists connection)
    startedAt <- promptDateTime "Start date and time (YYYY-MM-DD HH:MM:SS): "
    endedAt <- promptOptionalEndDateTime startedAt "End date and time (leave empty if still active): "
    addWorkstationUsage connection workstationId userId startedAt endedAt
    successMessage "Workstation usage added."

updateWorkstationUsageAction :: Connection -> IO ()
updateWorkstationUsageAction connection = do
    actionTitle "Update workstation usage"
    usageId <- promptExistingId "ID to update: " (workstationUsageExists connection)
    workstationId <- promptExistingId "New workstation ID: " (workstationExists connection)
    userId <- promptExistingId "New user ID: " (userExists connection)
    startedAt <- promptDateTime "New start date and time (YYYY-MM-DD HH:MM:SS): "
    endedAt <- promptOptionalEndDateTime startedAt "New end date and time (leave empty if still active): "
    updateWorkstationUsage connection usageId workstationId userId startedAt endedAt
    successMessage "Workstation usage updated."
