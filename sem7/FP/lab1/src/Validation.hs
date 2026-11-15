module Validation
    ( trim
    , isCancelCommand
    , isRequiredValue
    , isPositiveNumber
    , normalizeAllowed
    , isValidEmail
    , isValidDate
    , normalizeTime
    , isValidTime
    , isEndTimeAfter
    , isValidDateTime
    , normalizeDateTime
    , isEndDateTimeAfter
    ) where

import Data.Char (isDigit, isSpace, toLower)
import Data.Maybe (isJust)
import Data.Time (Day, LocalTime, TimeOfDay, defaultTimeLocale, parseTimeM)

trim :: String -> String
trim = reverse . dropWhile isSpace . reverse . dropWhile isSpace

isCancelCommand :: String -> Bool
isCancelCommand rawValue =
    let value = trim rawValue
        normalizedValue = map toLower value
    in normalizedValue == "0" || normalizedValue == "cancel" || value == "\ESC"

isRequiredValue :: String -> Bool
isRequiredValue = not . null . trim

isPositiveNumber :: Int -> Bool
isPositiveNumber number = number > 0

normalizeAllowed :: [String] -> String -> Maybe String
normalizeAllowed allowedValues rawValue =
    let value = map toLower (trim rawValue)
    in if value `elem` allowedValues
        then Just value
        else Nothing

isValidEmail :: String -> Bool
isValidEmail rawEmail =
    let email = trim rawEmail
    in case break (== '@') email of
        (localPart, '@' : domainPart) ->
            not (null localPart)
                && '.' `elem` domainPart
                && not (any isSpace email)
        _ -> False

parseDate :: String -> Maybe Day
parseDate = parseTimeM True defaultTimeLocale "%Y-%m-%d"

padHour :: String -> String
padHour value =
    case break (== ':') value of
        ([hourDigit], rest)
            | isDigit hourDigit -> '0' : hourDigit : rest
        _ -> value

normalizeTime :: String -> Maybe String
normalizeTime rawValue =
    let value = padHour (trim rawValue)
    in case parseTimeM True defaultTimeLocale "%H:%M" value :: Maybe TimeOfDay of
        Just _ -> Just value
        Nothing -> Nothing

parseClockTime :: String -> Maybe TimeOfDay
parseClockTime value = do
    normalizedValue <- normalizeTime value
    parseTimeM True defaultTimeLocale "%H:%M" normalizedValue

normalizeDateTime :: String -> Maybe String
normalizeDateTime rawValue =
    case words (trim rawValue) of
        [datePart, timePart] ->
            let value = datePart ++ " " ++ padHour timePart
            in case parseTimeM True defaultTimeLocale "%Y-%m-%d %H:%M:%S" value :: Maybe LocalTime of
                Just _ -> Just value
                Nothing -> Nothing
        _ -> Nothing

parseDateTime :: String -> Maybe LocalTime
parseDateTime value = do
    normalizedValue <- normalizeDateTime value
    parseTimeM True defaultTimeLocale "%Y-%m-%d %H:%M:%S" normalizedValue

isValidDate :: String -> Bool
isValidDate = isJust . parseDate

isValidTime :: String -> Bool
isValidTime = isJust . normalizeTime

isEndTimeAfter :: String -> String -> Bool
isEndTimeAfter startValue endValue =
    case (parseClockTime startValue, parseClockTime endValue) of
        (Just startTime, Just endTime) -> endTime > startTime
        _ -> False

isValidDateTime :: String -> Bool
isValidDateTime = isJust . normalizeDateTime

isEndDateTimeAfter :: String -> String -> Bool
isEndDateTimeAfter startValue endValue =
    case (parseDateTime startValue, parseDateTime endValue) of
        (Just startTime, Just endTime) -> endTime > startTime
        _ -> False
