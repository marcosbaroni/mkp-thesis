import Data.List (delete, sortBy, transpose)
import Data.Ord (comparing)
import Data.String (words) 

----------------------------- DATA TYPES ---------------------------------------
type Number = Double

-- | An MKP item
--      tuple: (profit, capacities)
type Item = (Number, [Number])

-- | The MKP instance representation
--      tuple: (list of items, capacities)
type MKP = ([Item], [Number])

-- | The MKP solution representation 
--      tuple: (selected itens, profit, weights)
type MKPSolution = ([Int], Number, [Number])

-- | Insert the given item on a MKP solution, updating its profit and weights.
addItem :: Item -> Int -> MKPSolution -> MKPSolution
addItem (itemProfit, itemWeights) idx (solIdxs, solProfit, solWeights) = (solIdxs', solProfit', solWeight')
	where
	solIdxs' = solIdxs ++ [idx]
	solProfit' = solProfit + itemProfit
	solWeight' = map (uncurry (+)) $ zip itemWeights solWeights
--------------------------------------------------------------------------------

-------------------------- DOMINATING SETS  ------------------------------------
-- | Answer if the first set is not dominated by the second.
notDominatedBy :: MKPSolution -> MKPSolution -> Bool
notDominatedBy (_, p1, cs1) (_, p2, cs2) = betterProfit || dominateWeights
	where
	betterProfit = (p1 > p2)
	dominateWeights = or $ map (uncurry (<)) $ (zip cs1 cs2)

-- | Returns all dominating sets of a MKP instance.
domSets :: MKP -> [MKPSolution]
domSets (items, _) = domSets' 1 items []
	where
	-- recusrively computes dominating sets
	domSets' _ [] set = set
	domSets' idx (it:items) sets = domSets' (idx+1) items newSets
		where
		newSets = [x | x <- merged, and $ map (notDominatedBy x) (delete x merged)]
		merged  = sets ++ map (addItem it idx) sets ++ [([idx], fst it, snd it)]
--------------------------------------------------------------------------------


---------------------------------- SOLVING MKP ---------------------------------
-- | Solves the MKP using domating sets generation.
--     Among the feasible sets the most protitable is selected.
solve :: MKP -> MKPSolution
solve mkp = optimum
	where
	getProfit (_, p, _) = p
	dummySet = ([], 0, snd mkp)   -- for filtering
	feasibles = filter (not.(notDominatedBy dummySet)) $ domSets mkp
	optimum = head $ reverse $ sortBy (comparing getProfit) feasibles
--------------------------------------------------------------------------------


---------------------------------------------
-- 10
-- 2
-- 387 489 354 621 161 191 566 391 354 474
-- 422 513 415 734 822 561 869 382 24 53
-- 431 908 392 245 902 956 12 654 323 454
-- 2398 2639
---------------------------------------------
mkp1 :: MKP
mkp1 = ([
	(387, [422, 431]),
	(489, [513, 908]),
	(354, [415, 392]),
	(621, [734, 245]),
	(161, [822, 902]),
	(191, [561, 956]),
	(566, [869, 012]),
	(391, [382, 654]),
	(354, [354, 323]),
	(474, [474, 454])],
	[2398, 2639])

mkp2 :: MKP
mkp2 = ([
	(576, [508, 260]),
	(269, [241, 236]),
	(451, [134, 951]),
	(620, [121, 572]),
	(755, [197, 063])],
	[601, 1041])

---------------------------- INPUT/OUTPUT --------------------------------------
-- | Split elements of a list in sublists of a given length. (Auxiliary function)
subGroup :: [a] -> Int -> [[a]]
subGroup ls n = sub' ls n []
	where
	sub' [] n rs = rs
	sub' ls n rs = sub' (drop n ls) n (rs ++ [take n ls])

-- | Parses a MKP instance.
readMKP :: String -> MKP
readMKP str = (zip profits weights, capacities)
	where
	ws = words str
	(n, m) = (read $ ws!!0, read $ ws!!1)
	nums = map read $ drop 2 ws
	profits = take n nums
	weights = transpose $ subGroup (take (n*m) $ drop n nums) n
	capacities = drop ((n*m)+n) nums
--------------------------------------------------------------------------------

---------------------------- MAIN PROGRAM --------------------------------------
-- | Reads a MKP instance from stdin and prints its optimun solution using 
--     adapted Nemhauser-Ullman algorithm.
optMKP = getContents >>= return.show.solve.readMKP >>= print

-- | Reads a MKP instance from stdin and prints the total number of
--     dominating sets.
domSetsSize = getContents >>= return.length.domSets.readMKP >>= print

main = domSetsSize
--------------------------------------------------------------------------------

