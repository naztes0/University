module Main where

import Control.Exception (SomeException, displayException, try)
import System.Exit (exitFailure, exitSuccess)
import Test.HUnit (Assertion, assertBool, assertEqual)
import Validation

tests :: [(String, Assertion)]
tests =
    [ ("Cancel command works",
        assertBool "cancel must be recognized" (isCancelCommand "Cancel"))
    , ("Required field rejects spaces",
        assertBool "spaces must be empty" (not (isRequiredValue "   ")))
    , ("Positive number rejects zero",
        assertBool "zero must be rejected" (not (isPositiveNumber 0)))
    , ("User type is normalized",
        assertEqual "normalized type" (Just "teacher")
            (normalizeAllowed ["student", "teacher"] " Teacher "))
    , ("Unknown user type is rejected",
        assertEqual "unknown type" Nothing
            (normalizeAllowed ["student", "teacher"] "admin"))
    , ("Valid email is accepted",
        assertBool "email must be valid" (isValidEmail "user@example.com"))
    , ("Invalid email is rejected",
        assertBool "email must be invalid" (not (isValidEmail "user@example")))
    , ("Valid date is accepted",
        assertBool "date must be valid" (isValidDate "2026-11-15"))
    , ("Impossible date is rejected",
        assertBool "February 30 must be invalid" (not (isValidDate "2026-02-30")))
    , ("Single digit hour is normalized",
        assertEqual "normalized time" (Just "09:30") (normalizeTime "9:30"))
    , ("Invalid hour is rejected",
        assertBool "25:00 must be invalid" (not (isValidTime "25:00")))
    , ("End time must be later",
        assertBool "earlier end must fail" (not (isEndTimeAfter "18:00" "17:00")))
    , ("Date-time hour is normalized",
        assertEqual "normalized date-time" (Just "2026-11-15 09:30:00")
            (normalizeDateTime "2026-11-15 9:30:00"))
    , ("End date-time must be later",
        assertBool "later date-time must pass"
            (isEndDateTimeAfter "2026-11-15 09:00:00" "2026-11-15 10:00:00"))
    ]

runNamedTest :: (String, Assertion) -> IO Bool
runNamedTest (name, assertion) = do
    result <- try assertion :: IO (Either SomeException ())
    case result of
        Right _ -> do
            putStrLn ("[PASS] " ++ name)
            return True
        Left exception -> do
            putStrLn ("[FAIL] " ++ name)
            putStrLn ("       " ++ displayException exception)
            return False

main :: IO ()
main = do
    results <- mapM runNamedTest tests
    let passed = length (filter id results)
    let total = length results
    putStrLn ""
    putStrLn ("Unit tests passed: " ++ show passed ++ "/" ++ show total)
    if passed == total
        then exitSuccess
        else exitFailure
