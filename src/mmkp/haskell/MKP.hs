import Data.List (delete) 
import Data.String (words) 

----------------------------- DATA TYPES ---------------------------------------
type Number = Double

-- | An MKP item
--      (profit, capacities)
type Item = (Number, [Number])

-- | The MKP instance representation
--      (list of items, capacities)
type MKP = ([Item], [Number])

-- | The MKP solution representation 
--      (selected itens, profit, weights)
type MKPSolution = ([Int], Number, [Number])
--------------------------------------------------------------------------------


-------------------------- DOMINATING SETS  ------------------------------------
-- | Answes if the first set dominates the second set.
dominates :: MKPSolution -> MKPSolution -> Bool
dominates (_, p1, c1) (_, p2, c2) = betterProfit || dominateWeights
	where
	betterProfit = (p1 > p2)
	dominateWeights = or $ map (uncurry (<)) $ (zip c1 c2)

-- | Returns all dominating sets of a MKP instance.
domSets :: MKP -> [MKPSolution]
domSets (items, _) = domSets' 1 items []
	where
	domSets' _ [] set = set
	domSets' idx (it:items) sets = domSets' (idx+1) items newSets
		where
		newSets = [x | x <- merged, and $ map (dominates x) (delete x merged)]
		merged  = sets ++ map (addItem it idx) sets ++ [([idx], fst it, snd it)]
--------------------------------------------------------------------------------

-- | Insert the given item on a MKP solution.
addItem :: Item -> Int -> MKPSolution -> MKPSolution
addItem (p, ws) idx (items, pSol, wSol) = (newItems, newP, newW)
	where
	newItems = idx:items
	newP = pSol+p
	newW = map (uncurry (+)) $ zip ws wSol

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


---------------------------- INPUT/OUTPUT --------------------------------------
readMKP :: String -> MKP
readMKP str = undefined
	where
	nums = words str
--------------------------------------------------------------------------------
