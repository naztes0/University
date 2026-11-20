module Menu (runMenu) where

import Benchmark (benchmarkGraph, measureAction, printBenchmarkRows)
import Data.List (intercalate)
import GHC.Conc (getNumProcessors, setNumCapabilities)
import Graph
import ShortestPath
import System.IO (hFlush, stdout)
import Text.Printf (printf)
import Text.Read (readMaybe)

runMenu :: IO ()
runMenu = do
    putStrLn ""
    putStrLn "=== Parallel shortest path ==="
    putStrLn "1. Run sample graph"
    putStrLn "2. Load graph from file"
    putStrLn "3. Run performance benchmark"
    putStrLn "0. Exit"
    choice <- prompt "Choice: "
    case choice of
        "1" -> runGraphFile "data/sample-graph.txt" >> runMenu
        "2" -> do
            path <- promptRequired "Graph file path: "
            runGraphFile path
            runMenu
        "3" -> runBenchmark >> runMenu
        "0" -> putStrLn "Program finished."
        _ -> putStrLn "Unknown option. Try again." >> runMenu

runGraphFile :: FilePath -> IO ()
runGraphFile path = do
    graphResult <- loadGraph path
    case graphResult of
        Left graphError -> putStrLn ("Error: " ++ graphError)
        Right graph -> runShortestPath graph

runShortestPath :: Graph -> IO ()
runShortestPath graph = do
    putStrLn ""
    putStrLn
        ( "Graph loaded: "
            ++ show (vertexCount graph)
            ++ " vertices, "
            ++ show (length (graphEdges graph))
            ++ " edges."
        )
    source <- promptInRange "Start vertex: " 0 (vertexCount graph - 1)
    target <- promptInRange "Target vertex: " 0 (vertexCount graph - 1)
    threadCount <- promptPositiveInt "Number of threads: "
    setNumCapabilities threadCount
    (seconds, searchResult) <-
        measureAction (shortestPathsParallel threadCount graph source)
    putStrLn ""
    case searchResult of
        Left searchError -> putStrLn ("Error: " ++ searchError)
        Right result -> printPathResult (pathTo source target result)
    printf "Time: %.3f ms\n" (seconds * 1000)

printPathResult :: PathResult -> IO ()
printPathResult pathResult =
    case pathResult of
        PathFound path distance -> do
            putStrLn ("Shortest path: " ++ intercalate " -> " (map show path))
            putStrLn ("Distance: " ++ show distance)
        PathNotFound -> putStrLn "No path exists between these vertices."
        NegativeCycleDetected ->
            putStrLn "A reachable negative cycle exists, so no finite shortest path can be given."

runBenchmark :: IO ()
runBenchmark = do
    processorCount <- getNumProcessors
    putStrLn ""
    putStrLn "=== Performance benchmark ==="
    putStrLn ("Logical processors detected: " ++ show processorCount)
    vertices <- promptAtLeast "Number of vertices (recommended 300): " 2
    let minimumEdges = vertices - 1
        maximumEdges = maximumEdgeCount vertices
    edges <-
        promptInRange
            ( "Number of edges ("
                ++ show minimumEdges
                ++ "-"
                ++ show maximumEdges
                ++ ", recommended 5000): "
            )
            minimumEdges
            maximumEdges
    maximumThreads <- promptPositiveInt "Maximum number of threads: "
    let graph = generateGraph vertices edges
    putStrLn "Running each configuration 3 times..."
    benchmarkResult <- benchmarkGraph 3 maximumThreads graph 0
    case benchmarkResult of
        Left benchmarkError -> putStrLn ("Error: " ++ benchmarkError)
        Right rows -> do
            printBenchmarkRows rows
            putStrLn ""
            putStrLn "Speedup = one-thread time / parallel time"
            putStrLn "Efficiency = speedup / thread count * 100%"

prompt :: String -> IO String
prompt message = do
    putStr message
    hFlush stdout
    getLine

promptRequired :: String -> IO String
promptRequired message = do
    value <- prompt message
    if null (words value)
        then putStrLn "This field cannot be empty." >> promptRequired message
        else return value

promptInt :: String -> IO Int
promptInt message = do
    value <- prompt message
    case readMaybe value of
        Just number -> return number
        Nothing -> putStrLn "Enter a whole number." >> promptInt message

promptPositiveInt :: String -> IO Int
promptPositiveInt message = promptAtLeast message 1

promptAtLeast :: String -> Int -> IO Int
promptAtLeast message minimumValue = do
    value <- promptInt message
    if value >= minimumValue
        then return value
        else do
            putStrLn ("Enter a number not smaller than " ++ show minimumValue ++ ".")
            promptAtLeast message minimumValue

promptInRange :: String -> Int -> Int -> IO Int
promptInRange message minimumValue maximumValue = do
    value <- promptInt message
    if value >= minimumValue && value <= maximumValue
        then return value
        else do
            putStrLn
                ( "Enter a number from "
                    ++ show minimumValue
                    ++ " to "
                    ++ show maximumValue
                    ++ "."
                )
            promptInRange message minimumValue maximumValue
