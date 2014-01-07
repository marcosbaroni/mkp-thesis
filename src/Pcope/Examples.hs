module Pcope.Examples where

import Pcope.Instance
import qualified Data.Vector as V

-- | Example list of Action
pActs1 :: [Action]
pActs1 = 
	[Action {
		am = 100,
		au = V.replicate 4 40,
		az = V.replicate 12 10,
		ac = V.replicate 2 1,
		av = 1,
		ae = V.replicate 12 0.2
	}, Action {
		am = 90,
		au = V.replicate 4 20,
		az = V.replicate 12 5,
		ac = V.replicate 2 2,
		av = 1,
		ae = V.replicate 12 0.4
	}, Action {
		am = 190,
		au = V.replicate 4 80,
		az = V.replicate 12 10,
		ac = V.replicate 2 0.5,
		av = 1,
		ae = V.replicate 12 0.1
	}]

-- | Example of Problem Instance.
p1 :: Pcope
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

