module Pcope.Tools where

import System.Random (RandomGen, randomR)
import Data.List (unfoldr)

-- | Random normal distributed number generator (mean: 1.0, std. dev.: 1.0).
normalD :: RandomGen g => g -> Double
normalD gen = x1*z
	where
	rands = unfoldr (Just . randomR (-1.0,1.0)) gen
	dups = zip rands $ tail rands
	fst3 (a,_,_) = a
	(w, x1, x2) = head $ filter ((<1.0) . fst3) $ [(a**2+b**2, a, b) | (a,b) <- dups]
	z = sqrt ( (-2)*(log w) / w)

-- | List of random number on normal distribution.
normalDs :: RandomGen g => g -> (Double, Double) -> ([Double], g)
	-- g                       -- ^ The random generator
	-- -> (Double, Double)     -- ^ (Mean, StdDev)
	-- -> ([Double], g)        -- ^ (List of random numbers, new generator)
normalDs = undefined
--NormalDs gen (m, sd) = (ls, g2)
--	where
--	ls =  

