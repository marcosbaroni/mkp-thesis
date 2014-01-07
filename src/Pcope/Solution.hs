module Pcope.Solution where

import Pcope.Instance
import Data.Vector (Vector)
import qualified Data.Vector as V

-- | A solving structure to help manipulate an action information.
data ActionInfo = ActionInfo {
	mLeft :: Int,     -- | Global market left
	uLeft :: Vector Int,   -- | Yearly market left
	zLeft :: Vector Int    -- | Periodal market left
	}
	deriving (Show, Eq)

-- | Empty (initial) action info constructor.
emptyAI :: Action -> ActionInfo
emptyAI a = ActionInfo {
	mLeft = am a,
	uLeft = au a,
	zLeft = az a
	}


-- | A solving structure to help manipulate a solution.
data SolutionInfo = SolutionInfo {
	solution :: Solution,             -- | The solution it self
	viable :: Bool,                   -- | If its a viable solution
	obj :: Double,                    -- | The objective function value
	gLeft :: Vector Double,           -- | Energy left for each year (goal)
	oLeft :: Vector Double,           -- | Global budget left
	pLeft :: Vector (Vector Double),  -- | Anual budget left
	sLeft :: Vector (Vector Double),           -- | Periodal budget left
	actInfos :: Vector ActionInfo     -- | Info of actions
	}
	deriving (Show, Eq)
instance Ord SolutionInfo where
	compare si1 si2 = compare (obj si1) (obj si2)

-- | Empty (initial) solution info constructor.
empty :: Pcope -> SolutionInfo
empty p = SolutionInfo {
	solution = V.replicate (pN p) (V.replicate (pN p) 0),
	viable = True,
	obj = 0.0,
	gLeft = pg p,
	oLeft = po p,
	pLeft = pp p,
	sLeft = ps p,
	actInfos = V.fromList $ map emptyAI (pActs p)
	}

