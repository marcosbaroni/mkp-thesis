module Pcope.Instance where

import Data.Vector (Vector, fromList, (!), (//))
import qualified Data.Vector as V

-- | Auxiliary function for updating an element from a vector.
update :: Vector a -> Int -> a -> Vector a
update vec i x = vec // [(i,x)]

-- | Defines an energy loss recovery action.
data Action = Action {
	am :: Int,               -- | Global market
	au :: Vector Int,        -- | Yearly market
	az :: Vector Int,        -- | Periodal market
	ac :: Vector Double,     -- | Cost
	av :: Double,            -- | Value of energy
	ae :: Vector Double      -- | Recovery Curve
	}
	deriving (Show, Eq)


-- | Defines an instance of the problem.
data Pcope = Pcope {
	pN :: Int,                     -- | Nº of actions
	pY :: Int,                     -- | Nº of years
	pP :: Int,                     -- | Nº of periods per year
	pR :: Int,                     -- | Nº of resources
	pr :: Double,                  -- | Internal return rate
	pg :: Vector Double,           -- | Anual recover Goal
	po :: Vector Double,           -- | Global budget
	pp :: Vector (Vector Double),  -- | Anual Budget
	ps :: Vector (Vector Double),  -- | Periodal Budget
	pActs :: [Action]
	}
	deriving (Show, Eq)

-- | Total number of periods.
pYP :: Pcope -> Int
pYP p = (pY p)*(pP p)

-- | Defines a solution for the problem.
type Solution = Vector (Vector Int)


pActs1 = [
	Action {
		am = 100,
		au = V.replicate 4 40,
		az = V.replicate 12 10,
		ac = V.replicate 2 1,
		av = 1,
		ae = V.replicate 12 0.2
	},
	Action {
		am = 90,
		au = V.replicate 4 20,
		az = V.replicate 12 5,
		ac = V.replicate 2 2,
		av = 1,
		ae = V.replicate 12 0.4
	},
	Action {
		am = 190,
		au = V.replicate 4 80,
		az = V.replicate 12 10,
		ac = V.replicate 2 0.5,
		av = 1,
		ae = V.replicate 12 0.1
	}]

p1 = Pcope {
	pN = 3,
	pY = 4,
	pP = 3,
	pR = 2,
	pr = 0.03,
	pg = V.replicate 4 10,
	po = V.replicate 2 10,
	pp = V.replicate 4 $ V.replicate 2 100,
	ps = V.replicate (4*3) $ V.replicate 2 10,
	pActs = pActs1
}
