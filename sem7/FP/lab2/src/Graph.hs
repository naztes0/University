module Graph
    ( Vertex
    , Weight
    , Edge (..)
    , Graph (..)
    , makeGraph
    , parseGraph
    , loadGraph
    , generateGraph
    , maximumEdgeCount
    ) where

import Control.DeepSeq (NFData (rnf))
import Control.Exception (IOException, try)
import Text.Read (readMaybe)

type Vertex = Int

type Weight = Int

data Edge = Edge
    { edgeFrom :: Vertex
    , edgeTo :: Vertex
    , edgeWeight :: Weight
    }
    deriving (Eq, Show)

instance NFData Edge where
    rnf (Edge fromVertex toVertex weight) =
        rnf fromVertex `seq` rnf toVertex `seq` rnf weight

data Graph = Graph
    { vertexCount :: Int
    , graphEdges :: [Edge]
    }
    deriving (Eq, Show)

instance NFData Graph where
    rnf (Graph vertices edges) = rnf vertices `seq` rnf edges

makeGraph :: Int -> [Edge] -> Either String Graph
makeGraph vertices edges
    | vertices <= 0 = Left "The graph must contain at least one vertex."
    | otherwise =
        case filter (not . validEdge) edges of
            [] -> Right (Graph vertices edges)
            invalidEdge : _ -> Left ("Edge contains an unknown vertex: " ++ show invalidEdge)
  where
    validEdge edge =
        edgeFrom edge >= 0
            && edgeFrom edge < vertices
            && edgeTo edge >= 0
            && edgeTo edge < vertices

parseGraph :: String -> Either String Graph
parseGraph contents =
    case filter (not . null . words) (lines contents) of
        [] -> Left "The graph file is empty."
        header : edgeLines -> do
            (vertices, declaredEdgeCount) <- parseHeader header
            edges <- mapM parseEdgeLine (zip [2 :: Int ..] edgeLines)
            if length edges /= declaredEdgeCount
                then
                    Left
                        ( "The header declares "
                            ++ show declaredEdgeCount
                            ++ " edges, but the file contains "
                            ++ show (length edges)
                            ++ "."
                        )
                else makeGraph vertices edges

parseHeader :: String -> Either String (Int, Int)
parseHeader line =
    case words line of
        [verticesText, edgeCountText] -> do
            vertices <- readInteger "vertex count" verticesText
            edgeCount <- readInteger "edge count" edgeCountText
            if edgeCount < 0
                then Left "The edge count cannot be negative."
                else Right (vertices, edgeCount)
        _ -> Left "The first line must contain: vertex_count edge_count."

parseEdgeLine :: (Int, String) -> Either String Edge
parseEdgeLine (lineNumber, line) =
    case words line of
        [fromText, toText, weightText] -> do
            fromVertex <- readInteger (lineLabel "start vertex") fromText
            toVertex <- readInteger (lineLabel "end vertex") toText
            weight <- readInteger (lineLabel "weight") weightText
            Right (Edge fromVertex toVertex weight)
        _ -> Left ("Line " ++ show lineNumber ++ " must contain: from to weight.")
  where
    lineLabel valueName = valueName ++ " on line " ++ show lineNumber

readInteger :: String -> String -> Either String Int
readInteger valueName text =
    case readMaybe text of
        Just value -> Right value
        Nothing -> Left ("Invalid " ++ valueName ++ ": " ++ text)

loadGraph :: FilePath -> IO (Either String Graph)
loadGraph path = do
    fileResult <- try (readFile path) :: IO (Either IOException String)
    case fileResult of
        Left fileError -> return (Left ("Cannot read the file: " ++ show fileError))
        Right contents -> return (parseGraph contents)

maximumEdgeCount :: Int -> Int
maximumEdgeCount vertices = max 0 (vertices * (vertices - 1))

generateGraph :: Int -> Int -> Graph
generateGraph vertices requestedEdgeCount =
    Graph vertices (take safeEdgeCount allEdges)
  where
    safeEdgeCount = max 0 (min requestedEdgeCount (maximumEdgeCount vertices))
    allEdges =
        [ Edge fromVertex toVertex (generatedWeight fromVertex toVertex offset)
        | offset <- [1 .. vertices - 1]
        , fromVertex <- [0 .. vertices - 1]
        , let toVertex = (fromVertex + offset) `mod` vertices
        ]
    generatedWeight fromVertex toVertex offset =
        1 + ((fromVertex * 31 + toVertex * 17 + offset * 13) `mod` 20)
