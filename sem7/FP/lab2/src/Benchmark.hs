module Benchmark
    ( BenchmarkRow (..)
    , measureAction
    , benchmarkGraph
    , printBenchmarkRows
    ) where

import Control.DeepSeq (NFData, force)
import Control.Exception (evaluate)
import Control.Monad (forM)
import Data.List (nub)
import Data.Time.Clock (diffUTCTime, getCurrentTime)
import GHC.Conc (setNumCapabilities)
import Graph
import ShortestPath
import Text.Printf (printf)

data BenchmarkRow = BenchmarkRow
    { benchmarkThreads :: Int
    , benchmarkSeconds :: Double
    , benchmarkSpeedup :: Double
    , benchmarkEfficiency :: Double
    , benchmarkCorrect :: Bool
    }
    deriving (Eq, Show)

measureAction :: NFData a => IO a -> IO (Double, a)
measureAction action = do
    startedAt <- getCurrentTime
    value <- action
    evaluatedValue <- evaluate (force value)
    finishedAt <- getCurrentTime
    return (realToFrac (diffUTCTime finishedAt startedAt), evaluatedValue)

benchmarkGraph :: Int -> Int -> Graph -> Vertex -> IO (Either String [BenchmarkRow])
benchmarkGraph repetitions maximumThreads graph source
    | repetitions <= 0 = return (Left "The repetition count must be greater than zero.")
    | maximumThreads <= 0 = return (Left "The number of threads must be greater than zero.")
    | source < 0 || source >= vertexCount graph = return (Left "The start vertex does not exist.")
    | otherwise = do
        setNumCapabilities 1
        (sequentialSeconds, sequentialResult) <-
            averageMeasurements repetitions (sequentialAction graph source)
        remainingRows <-
            mapM
                (benchmarkParallel sequentialSeconds sequentialResult)
                (filter (> 1) (benchmarkThreadCounts maximumThreads))
        let sequentialRow =
                BenchmarkRow
                    { benchmarkThreads = 1
                    , benchmarkSeconds = sequentialSeconds
                    , benchmarkSpeedup = 1
                    , benchmarkEfficiency = 100
                    , benchmarkCorrect = True
                    }
        return (Right (sequentialRow : remainingRows))
  where
    benchmarkParallel sequentialSeconds sequentialResult threadCount = do
        setNumCapabilities threadCount
        (parallelSeconds, parallelResult) <-
            averageMeasurements repetitions (parallelAction threadCount graph source)
        let speedup = sequentialSeconds / parallelSeconds
        return
            BenchmarkRow
                { benchmarkThreads = threadCount
                , benchmarkSeconds = parallelSeconds
                , benchmarkSpeedup = speedup
                , benchmarkEfficiency = speedup / fromIntegral threadCount * 100
                , benchmarkCorrect = equivalentResults sequentialResult parallelResult
                }

type TimedAlgorithm = Int -> IO (IO (Either String ShortestPathResult))

sequentialAction :: Graph -> Vertex -> TimedAlgorithm
sequentialAction graph source runNumber = do
    freshGraph <- evaluate (force (copyGraph runNumber graph))
    return (return (shortestPathsSequential freshGraph source))

parallelAction :: Int -> Graph -> Vertex -> TimedAlgorithm
parallelAction threadCount graph source runNumber = do
    freshGraph <- evaluate (force (copyGraph runNumber graph))
    return (shortestPathsParallel threadCount freshGraph source)

averageMeasurements :: Int -> TimedAlgorithm -> IO (Double, Either String ShortestPathResult)
averageMeasurements repetitions prepareAction = do
    measurements <-
        forM [1 .. repetitions] $ \runNumber -> do
            action <- prepareAction runNumber
            measureAction action
    let averageSeconds = sum (map fst measurements) / fromIntegral repetitions
    return (averageSeconds, snd (last measurements))

copyGraph :: Int -> Graph -> Graph
copyGraph runNumber graph =
    runNumber `seq` Graph (vertexCount graph) (map copyEdge (graphEdges graph))
  where
    copyEdge edge = Edge (edgeFrom edge) (edgeTo edge) (edgeWeight edge)

benchmarkThreadCounts :: Int -> [Int]
benchmarkThreadCounts maximumThreads =
    nub (takeWhile (< maximumThreads) powersOfTwo ++ [maximumThreads])
  where
    powersOfTwo = iterate (* 2) 1

printBenchmarkRows :: [BenchmarkRow] -> IO ()
printBenchmarkRows rows = do
    putStrLn ""
    putStrLn "Threads | Time, ms   | Speedup | Efficiency | Correct"
    putStrLn "------------------------------------------------------"
    mapM_ printRow rows
  where
    printRow row =
        printf
            "%-7d | %-10.3f | %-7.2f | %9.1f%% | %s\n"
            (benchmarkThreads row)
            (benchmarkSeconds row * 1000)
            (benchmarkSpeedup row)
            (benchmarkEfficiency row)
            (if benchmarkCorrect row then "yes" else "NO")
