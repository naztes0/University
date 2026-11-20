module ShortestPath
    ( ShortestPathResult (..)
    , PathResult (..)
    , shortestPathsSequential
    , shortestPathsParallel
    , pathTo
    , equivalentResults
    ) where

import Control.Concurrent (forkIO)
import Control.Concurrent.MVar (newEmptyMVar, putMVar, takeMVar)
import Control.DeepSeq (NFData (rnf), force)
import Control.Exception (evaluate)
import Data.IntMap.Strict (IntMap)
import qualified Data.IntMap.Strict as IntMap
import Graph

data ShortestPathResult = ShortestPathResult
    { resultDistances :: IntMap Weight
    , resultPredecessors :: IntMap Vertex
    , hasNegativeCycle :: Bool
    }
    deriving (Eq, Show)

instance NFData ShortestPathResult where
    rnf (ShortestPathResult distances predecessors negativeCycle) =
        rnf distances `seq` rnf predecessors `seq` rnf negativeCycle

data PathResult
    = PathFound [Vertex] Weight
    | PathNotFound
    | NegativeCycleDetected
    deriving (Eq, Show)

instance NFData PathResult where
    rnf (PathFound vertices distance) = rnf vertices `seq` rnf distance
    rnf PathNotFound = ()
    rnf NegativeCycleDetected = ()

type Proposal = (Weight, Vertex)

shortestPathsSequential :: Graph -> Vertex -> Either String ShortestPathResult
shortestPathsSequential graph source = do
    validateSource graph source
    let initialDistances = IntMap.singleton source 0
        initialPredecessors = IntMap.empty
        (finalDistances, finalPredecessors) =
            runSequentialIterations
                (max 0 (vertexCount graph - 1))
                (graphEdges graph)
                initialDistances
                initialPredecessors
    return
        ShortestPathResult
            { resultDistances = finalDistances
            , resultPredecessors = finalPredecessors
            , hasNegativeCycle = canRelaxAnyEdge finalDistances (graphEdges graph)
            }

shortestPathsParallel :: Int -> Graph -> Vertex -> IO (Either String ShortestPathResult)
shortestPathsParallel workerCount graph source =
    case validateParallelInput workerCount graph source of
        Left validationError -> return (Left validationError)
        Right () -> do
            let initialDistances = IntMap.singleton source 0
                initialPredecessors = IntMap.empty
            (finalDistances, finalPredecessors) <-
                runParallelIterations
                    workerCount
                    (max 0 (vertexCount graph - 1))
                    (graphEdges graph)
                    initialDistances
                    initialPredecessors
            return
                ( Right
                    ShortestPathResult
                        { resultDistances = finalDistances
                        , resultPredecessors = finalPredecessors
                        , hasNegativeCycle = canRelaxAnyEdge finalDistances (graphEdges graph)
                        }
                )

validateParallelInput :: Int -> Graph -> Vertex -> Either String ()
validateParallelInput workerCount graph source
    | workerCount <= 0 = Left "The number of threads must be greater than zero."
    | otherwise = validateSource graph source

validateSource :: Graph -> Vertex -> Either String ()
validateSource graph source
    | source < 0 || source >= vertexCount graph = Left "The start vertex does not exist."
    | otherwise = Right ()

runSequentialIterations :: Int -> [Edge] -> IntMap Weight -> IntMap Vertex -> (IntMap Weight, IntMap Vertex)
runSequentialIterations 0 _ distances predecessors = (distances, predecessors)
runSequentialIterations iterations edges distances predecessors =
    if changed
        then runSequentialIterations (iterations - 1) edges newDistances newPredecessors
        else (distances, predecessors)
  where
    proposals = relaxEdges distances edges
    (newDistances, newPredecessors, changed) =
        applyProposals proposals distances predecessors

runParallelIterations :: Int -> Int -> [Edge] -> IntMap Weight -> IntMap Vertex -> IO (IntMap Weight, IntMap Vertex)
runParallelIterations _ 0 _ distances predecessors = return (distances, predecessors)
runParallelIterations workerCount iterations edges distances predecessors = do
    proposals <- relaxEdgesParallel workerCount distances edges
    let (newDistances, newPredecessors, changed) =
            applyProposals proposals distances predecessors
    if changed
        then runParallelIterations workerCount (iterations - 1) edges newDistances newPredecessors
        else return (distances, predecessors)

relaxEdgesParallel :: Int -> IntMap Weight -> [Edge] -> IO (IntMap Proposal)
relaxEdgesParallel workerCount distances edges = do
    resultVariables <- mapM startWorker (splitInto workerCount edges)
    localResults <- mapM takeMVar resultVariables
    return (IntMap.unionsWith chooseBetterProposal localResults)
  where
    startWorker edgeChunk = do
        resultVariable <- newEmptyMVar
        _ <- forkIO $ do
            localProposals <- evaluate (force (relaxEdges distances edgeChunk))
            putMVar resultVariable localProposals
        return resultVariable

relaxEdges :: IntMap Weight -> [Edge] -> IntMap Proposal
relaxEdges distances =
    IntMap.fromListWith chooseBetterProposal . foldr collectProposal []
  where
    collectProposal edge proposals =
        case IntMap.lookup (edgeFrom edge) distances of
            Nothing -> proposals
            Just fromDistance ->
                ( edgeTo edge
                , (fromDistance + edgeWeight edge, edgeFrom edge)
                ) : proposals

chooseBetterProposal :: Proposal -> Proposal -> Proposal
chooseBetterProposal firstProposal secondProposal
    | fst firstProposal < fst secondProposal = firstProposal
    | fst secondProposal < fst firstProposal = secondProposal
    | snd firstProposal <= snd secondProposal = firstProposal
    | otherwise = secondProposal

applyProposals :: IntMap Proposal -> IntMap Weight -> IntMap Vertex -> (IntMap Weight, IntMap Vertex, Bool)
applyProposals proposals distances predecessors =
    IntMap.foldlWithKey' applyOne (distances, predecessors, False) proposals
  where
    applyOne (currentDistances, currentPredecessors, changed) vertex (candidateDistance, predecessor) =
        case IntMap.lookup vertex currentDistances of
            Just currentDistance
                | currentDistance <= candidateDistance ->
                    (currentDistances, currentPredecessors, changed)
            _ ->
                ( IntMap.insert vertex candidateDistance currentDistances
                , IntMap.insert vertex predecessor currentPredecessors
                , True
                )

canRelaxAnyEdge :: IntMap Weight -> [Edge] -> Bool
canRelaxAnyEdge distances = any canRelax
  where
    canRelax edge =
        case IntMap.lookup (edgeFrom edge) distances of
            Nothing -> False
            Just fromDistance ->
                case IntMap.lookup (edgeTo edge) distances of
                    Nothing -> True
                    Just toDistance -> fromDistance + edgeWeight edge < toDistance

splitInto :: Int -> [a] -> [[a]]
splitInto partCount values = splitBySizes partSizes values
  where
    (baseSize, largerParts) = length values `divMod` partCount
    partSizes =
        replicate largerParts (baseSize + 1)
            ++ replicate (partCount - largerParts) baseSize

splitBySizes :: [Int] -> [a] -> [[a]]
splitBySizes [] _ = []
splitBySizes (partSize : remainingSizes) values =
    currentPart : splitBySizes remainingSizes remainingValues
  where
    (currentPart, remainingValues) = splitAt partSize values

pathTo :: Vertex -> Vertex -> ShortestPathResult -> PathResult
pathTo source target result
    | hasNegativeCycle result = NegativeCycleDetected
    | otherwise =
        case IntMap.lookup target (resultDistances result) of
            Nothing -> PathNotFound
            Just distance ->
                case buildPath target [] IntMap.empty of
                    Nothing -> PathNotFound
                    Just path -> PathFound path distance
  where
    buildPath current path visited
        | current == source = Just (source : path)
        | IntMap.member current visited = Nothing
        | otherwise = do
            predecessor <- IntMap.lookup current (resultPredecessors result)
            buildPath predecessor (current : path) (IntMap.insert current current visited)

equivalentResults :: Either String ShortestPathResult -> Either String ShortestPathResult -> Bool
equivalentResults (Right firstResult) (Right secondResult) =
    resultDistances firstResult == resultDistances secondResult
        && hasNegativeCycle firstResult == hasNegativeCycle secondResult
equivalentResults (Left firstError) (Left secondError) = firstError == secondError
equivalentResults _ _ = False
