{-# LANGUAGE Rank2Types #-}
module Pcope.Generator where

import Pcope.Instance
import System.Random (RandomGen)

-- | Configurationg for the random generator.
data Config = Config {
	nYears :: Int,
	nActs :: Int
	-- | ...
	}

type Generator = (RandomGen a => a -> Config -> Pcope)

