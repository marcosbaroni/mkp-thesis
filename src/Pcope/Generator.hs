{-# LANGUAGE Rank2Types #-}
module Pcope.Generator where

import System.Random (RandomGen)

import Pcope.Instance
import Pcope.Tools

-- | Configurationg for the random generator.
data Config = Config {
	nActs :: Int,
	nYears :: Int,
	nPers :: Int,
	nRes :: Int,
	rate :: Double
	}

type Generator = (RandomGen a => a -> Config -> Pcope)

{--
stdGen :: Generator
stdGen g c = pcope {
		pN = nActs c,
		pY = nYears c,
		pP = nPers c,
		pR = nRes c,
		pr = rate c,
		pg = pgRand,
		po = poRand,
		pp = ppRand,
		ps = psRand,
		pActs = pActsRand
	}
	where
--}
