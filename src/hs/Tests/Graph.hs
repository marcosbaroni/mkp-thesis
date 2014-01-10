import qualified Data.Vector as V
import qualified Data.Vector.Unboxed as U
import qualified Data.ByteString as BS
import Data.Either (rights)
import Data.Attoparsec.ByteString.Char8
import Data.List (sort, intercalate, groupBy)
import System.Environment (getArgs)

data Graph = Graph {
	n :: Int,
	m :: Int,
	dgrs :: U.Vector Int,
	adjs :: V.Vector (U.Vector Int)}

eigen :: Graph -> Int -> U.Vector Double
eigen g u = eigen' u g (U.replicate (n g) $ 1.0)
	where
	eigen' 0 g v = U.map (/ (U.foldr1 max v)) v
	eigen' k g v = eigen' (k-1) g (U.imap f $ v)
		where
		f i _ = iterSum ((adjs g) V.! i) v

iterSum ads v = U.foldr1 (+) $ (U.map (\x -> v U.! x) $ ads)

buildGraph :: (Int, Int, [(Int, Int)]) -> Graph
buildGraph (nG, mG, tls) = Graph nG mG dgrsG adjsG
	where
	adjsG = V.map (U.fromList) $ V.fromList $ map (map ((+(-1)).snd)) $ groupBy (\a b -> (fst a) == (fst b)) tls
	dgrsG = U.fromList $ V.toList $ V.map U.length $ adjsG

readGraph :: FilePath -> IO Graph
readGraph fp = do
	txt <- BS.readFile fp
	let gB = parseOnly pIList txt
	return (buildGraph $ head $ rights [gB])

pIList :: Parser (Int, Int, [(Int, Int)])
pIList = do
	n <- decimal
	endOfLine
	m <- decimal
	endOfLine
	is <- manyTill tuple endOfInput
	return (n, m, is)
	where
	tuple = do
		a <- number
		skipSpace
		b <- number
		endOfLine
		return (floor a, floor b)

main = do
	fn:_ <- getArgs
	putStrLn fn
	g <- readGraph fn
	let v = eigen g 10
	putStrLn $ show $ v

