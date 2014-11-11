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

ex = (6, 6, [(1,2), (1,5), (2,3), (3,4), (4,5), (5,6)])
(na, ma, tlsa) = ex

buildGraph :: (Int, Int, [(Int, Int)]) -> Graph
buildGraph (nG, mG, tls) = Graph nG mG dgrsG adjsG
	where
	adjsG = V.map (U.fromList) $ V.fromList $ map (map (+(-1))) $ groups
	groups = grp tls
	dgrsG = U.fromList $ V.toList $ V.map U.length $ adjsG

grp :: Eq a => [(a, b)] -> [[b]]
grp [] = []
grp ((y, x):xs) = subGrp [[x]] y xs
	where
	subGrp (xs1:xss) y1 ((y2, x2):ys) =
		if y1 == y2
		then subGrp ((x2:xs1):xss) y1 ys
		else subGrp ([x2]:xs1:xss) y2 ys
	subGrp xss _ [] = xss

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
