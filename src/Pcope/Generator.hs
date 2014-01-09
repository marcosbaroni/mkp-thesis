{-# LANGUAGE Rank2Types #-}
module Pcope.Generator where

import System.Random (RandomGen, StdGen)
import Data.Random.Normal (normals')

import Pcope.Instance
import Pcope.Tools

{--

Paramentros para gerador:
- Basicos:
  - n de acoes
  - n de anos
  - n de periodos
  - n de recursos
- Avançados:
  - variacao do tir das ações [0 -> 1]
  - Orçamento (global, anual e periodal): (Med e Var) do total das ações
  - Market (global, anual e periodal): (Med e Var) do total das ações
  - EnergyValue: (Med e Var)
  - Meta (anual): % do toal da recuperação
- Configuração:
  - (Min e Max) Tir
  - (Min e Max) Energy Value
  - (Min e Max) Global Market
  - (Min e Max) Global Market

--}

-- | Configurationg for the random generator.
data Config = Config {
	nActs :: Int,                      -- | Número de acoes
	nYears :: Int,                     -- | Número de anos
	nPers :: Int,                      -- | Número de periodos
	nRes :: Int,                       -- | Número de recursos
	tirVar :: Double,                  -- | Variação da tir
	budgetVar :: (Double, Double),     -- | (Med, Var) of budget
	marketVar :: (Double, Double),     -- | (Med, Var) of Market
	energyCostVar :: (Double, Double), -- | (Med, Var) of Energy Value
	goalVar :: (Double, Double),       -- | (Med, Var) of Energy Goal
:qa


:q
:q


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
