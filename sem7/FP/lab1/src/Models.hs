module Models where

class Printable a where
    printInfo :: a -> String

showOptional :: Maybe String -> String
showOptional Nothing = "not specified"
showOptional (Just value) = value

data DisplayClass = DisplayClass
    { displayClassId :: Int
    , displayClassName :: String
    , roomNumber :: String
    , capacity :: Int
    , openingTime :: String
    , closingTime :: String
    }
    deriving (Show)

instance Printable DisplayClass where
    printInfo displayClass =
        "Class #" ++ show (displayClassId displayClass)
            ++ ": " ++ displayClassName displayClass
            ++ ", room " ++ roomNumber displayClass
            ++ ", capacity " ++ show (capacity displayClass)
            ++ ", open " ++ openingTime displayClass
            ++ "-" ++ closingTime displayClass

data Workstation = Workstation
    { workstationId :: Int
    , workstationRoomNumber :: String
    , workstationNumber :: String
    , workstationStatus :: String
    }
    deriving (Show)

instance Printable Workstation where
    printInfo workstation =
        "Workstation #" ++ show (workstationId workstation)
            ++ ": " ++ workstationNumber workstation
            ++ ", room " ++ workstationRoomNumber workstation
            ++ ", status: " ++ workstationStatus workstation

data User = User
    { userId :: Int
    , userFullName :: String
    , userType :: String
    , userEmail :: Maybe String
    }
    deriving (Show)

instance Printable User where
    printInfo user =
        "User #" ++ show (userId user)
            ++ ": " ++ userFullName user
            ++ ", type: " ++ userType user
            ++ ", email: " ++ showOptional (userEmail user)

data Teacher = Teacher
    { teacherId :: Int
    , teacherFullName :: String
    , teacherDepartment :: String
    , teacherEmail :: Maybe String
    }
    deriving (Show)

instance Printable Teacher where
    printInfo teacher =
        "Teacher #" ++ show (teacherId teacher)
            ++ ": " ++ teacherFullName teacher
            ++ ", department: " ++ teacherDepartment teacher
            ++ ", email: " ++ showOptional (teacherEmail teacher)

data Subject = Subject
    { subjectId :: Int
    , subjectName :: String
    }
    deriving (Show)

instance Printable Subject where
    printInfo subject =
        "Subject #" ++ show (subjectId subject)
            ++ ": " ++ subjectName subject

data ScheduledClass = ScheduledClass
    { scheduledClassId :: Int
    , scheduledRoomNumber :: String
    , scheduledTeacherName :: String
    , scheduledSubjectName :: String
    , scheduledGroupName :: String
    , lessonDate :: String
    , lessonStartTime :: String
    , lessonEndTime :: String
    }
    deriving (Show)

instance Printable ScheduledClass where
    printInfo scheduledClass =
        "Scheduled class #" ++ show (scheduledClassId scheduledClass)
            ++ ": room " ++ scheduledRoomNumber scheduledClass
            ++ ", teacher: " ++ scheduledTeacherName scheduledClass
            ++ ", subject: " ++ scheduledSubjectName scheduledClass
            ++ ", group " ++ scheduledGroupName scheduledClass
            ++ ", " ++ lessonDate scheduledClass
            ++ " " ++ lessonStartTime scheduledClass
            ++ "-" ++ lessonEndTime scheduledClass

data FreeAccess = FreeAccess
    { freeAccessId :: Int
    , freeAccessRoomNumber :: String
    , accessDate :: String
    , accessStartTime :: String
    , accessEndTime :: String
    }
    deriving (Show)

instance Printable FreeAccess where
    printInfo freeAccess =
        "Free access #" ++ show (freeAccessId freeAccess)
            ++ ": room " ++ freeAccessRoomNumber freeAccess
            ++ ", " ++ accessDate freeAccess
            ++ " " ++ accessStartTime freeAccess
            ++ "-" ++ accessEndTime freeAccess

data WorkstationUsage = WorkstationUsage
    { usageId :: Int
    , usageWorkstationNumber :: String
    , usageRoomNumber :: String
    , usageUserName :: String
    , usageStartedAt :: String
    , usageEndedAt :: Maybe String
    }
    deriving (Show)

instance Printable WorkstationUsage where
    printInfo usage =
        "Usage #" ++ show (usageId usage)
            ++ ": workstation " ++ usageWorkstationNumber usage
            ++ ", room " ++ usageRoomNumber usage
            ++ ", user: " ++ usageUserName usage
            ++ ", started: " ++ usageStartedAt usage
            ++ ", ended: " ++ showOptional (usageEndedAt usage)

data ClassLoad = ClassLoad
    { classLoadRoomNumber :: String
    , classUsageCount :: Int
    , classUsageMinutes :: Int
    }
    deriving (Show)

instance Printable ClassLoad where
    printInfo classLoad =
        "Room " ++ classLoadRoomNumber classLoad
            ++ ": " ++ show (classUsageCount classLoad) ++ " usage records"
            ++ ", " ++ show (classUsageMinutes classLoad) ++ " minutes total"

data WorkstationLoad = WorkstationLoad
    { loadWorkstationNumber :: String
    , loadWorkstationRoomNumber :: String
    , workstationUsageCount :: Int
    , workstationUsageMinutes :: Int
    }
    deriving (Show)

instance Printable WorkstationLoad where
    printInfo workstationLoad =
        "Workstation " ++ loadWorkstationNumber workstationLoad
            ++ ", room " ++ loadWorkstationRoomNumber workstationLoad
            ++ ": " ++ show (workstationUsageCount workstationLoad) ++ " usage records"
            ++ ", " ++ show (workstationUsageMinutes workstationLoad) ++ " minutes total"
