{-# LANGUAGE ScopedTypeVariables #-}

module Main where

import Control.Exception (SomeException, displayException, try)
import Graph
import ShortestPath
import System.Exit (exitFailure, exitSuccess)
import Test.HUnit (Assertion, assertBool, assertEqual, assertFailure)

knownGraph :: Graph
knownGraph =
    Graph
        5
        [ Edge 0 1 4
        , Edge 0 2 2
        , Edge 2 1 1
        , Edge 1 3 5
        , Edge 2 3 8
        , Edge 3 4 2
        , Edge 1 4 10
        ]

expectRight :: String -> Either String a -> IO a
expectRight message result =
    case result of
        Right value -> return value
        Left actualError -> do
            _ <- assertFailure (message ++ ": " ++ actualError)
            fail actualError

testKnownShortestPath :: Assertion
testKnownShortestPath = do
    result <- expectRight "Sequential algorithm failed" (shortestPathsSequential knownGraph 0)
    assertEqual
        "Known shortest path"
        (PathFound [0, 2, 1, 3, 4] 10)
        (pathTo 0 4 result)

testUnreachableVertex :: Assertion
testUnreachableVertex = do
    result <-
        expectRight
            "Unreachable graph failed"
            (shortestPathsSequential (Graph 3 [Edge 0 1 2]) 0)
    assertEqual "Unreachable vertex" PathNotFound (pathTo 0 2 result)

testSourceToItself :: Assertion
testSourceToItself = do
    result <- expectRight "Source graph failed" (shortestPathsSequential knownGraph 2)
    assertEqual "Source to itself" (PathFound [2] 0) (pathTo 2 2 result)

testNegativeWeight :: Assertion
testNegativeWeight = do
    let graph = Graph 3 [Edge 0 1 4, Edge 0 2 5, Edge 1 2 (-2)]
    result <- expectRight "Negative edge graph failed" (shortestPathsSequential graph 0)
    assertEqual "Negative edge" (PathFound [0, 1, 2] 2) (pathTo 0 2 result)

testNegativeCycle :: Assertion
testNegativeCycle = do
    let graph = Graph 3 [Edge 0 1 1, Edge 1 2 (-2), Edge 2 1 (-2)]
    result <- expectRight "Negative cycle graph failed" (shortestPathsSequential graph 0)
    assertBool "Negative cycle must be detected" (hasNegativeCycle result)
    assertEqual "Negative cycle path result" NegativeCycleDetected (pathTo 0 2 result)

testParallelTwoThreads :: Assertion
testParallelTwoThreads = do
    let graph = generateGraph 30 200
        sequentialResult = shortestPathsSequential graph 0
    parallelResult <- shortestPathsParallel 2 graph 0
    assertBool "Two-thread result differs from sequential" (equivalentResults sequentialResult parallelResult)

testParallelFourThreads :: Assertion
testParallelFourThreads = do
    let graph = generateGraph 30 200
        sequentialResult = shortestPathsSequential graph 0
    parallelResult <- shortestPathsParallel 4 graph 0
    assertBool "Four-thread result differs from sequential" (equivalentResults sequentialResult parallelResult)

testMoreThreadsThanEdges :: Assertion
testMoreThreadsThanEdges = do
    let graph = Graph 3 [Edge 0 1 7]
        sequentialResult = shortestPathsSequential graph 0
    parallelResult <- shortestPathsParallel 8 graph 0
    assertBool "Result changed when threads exceed edges" (equivalentResults sequentialResult parallelResult)

testGraphParser :: Assertion
testGraphParser = do
    graph <- expectRight "Parser rejected a valid graph" (parseGraph "3 2\n0 1 5\n1 2 7\n")
    assertEqual "Parsed vertex count" 3 (vertexCount graph)
    assertEqual "Parsed edges" [Edge 0 1 5, Edge 1 2 7] (graphEdges graph)

tests :: [(String, Assertion)]
tests =
    [ ("Known shortest path", testKnownShortestPath)
    , ("Unreachable vertex", testUnreachableVertex)
    , ("Path from a vertex to itself", testSourceToItself)
    , ("Graph with a negative edge", testNegativeWeight)
    , ("Negative cycle detection", testNegativeCycle)
    , ("Parallel result with 2 threads", testParallelTwoThreads)
    , ("Parallel result with 4 threads", testParallelFourThreads)
    , ("More threads than edges", testMoreThreadsThanEdges)
    , ("Graph file parser", testGraphParser)
    ]

runNamedTest :: (String, Assertion) -> IO Bool
runNamedTest (name, assertion) = do
    testResult <- try assertion
    case testResult of
        Right () -> do
            putStrLn ("[PASS] " ++ name)
            return True
        Left (exception :: SomeException) -> do
            putStrLn ("[FAIL] " ++ name)
            putStrLn ("       " ++ displayException exception)
            return False

main :: IO ()
main = do
    results <- mapM runNamedTest tests
    let passedCount = length (filter id results)
        totalCount = length results
    putStrLn ""
    putStrLn ("Tests passed: " ++ show passedCount ++ "/" ++ show totalCount)
    if and results then exitSuccess else exitFailure
