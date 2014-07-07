
typealias Float Float64

type MMKP
	n :: Int               # N. of itens
	m :: Int               # N. of dimensions
	o :: Int               # N. of knapsacks
	P :: Array{Float, 1}   # Profit of each item [1 x N]
	W :: Array{Float, 2}   # Weights [M x N]
	B :: Array{Float, 2}   # Capacities [M x O]
end

##################################################################
# Builds a random MMKP instance.
##################################################################
function random_mmkp(n, m, o)
	beta = 0.5
	W = rand(m, n)
	P = rand(n)'

	# Random Normalized Vector ("rnv"), for capacity splitting between knapscks
	rnv = rand(o)
	rnv = rnv/sum(rnv)

	B = W*ones(n)*beta*rnv'
	return MMKP(n, m, o, W, B)
end



type MMKPSol
	x :: Array{Bool, 2}           # Solution [O x N]
	obj :: Float                  # Objective value
	Bleft :: Array{Float, 2}      # Capacities left on each [dim x knapsack]
	which_knap :: Array{Float, 2} # Marks in which knapsack item is (0 for non)
	viable :: Bool                # If solution is viable
end

function ispositive(x)
	return x > 0
end

##################################################################
# Builds a MMKPSol from a Bool Array representing the solution.
##################################################################
function new_mmkp_sol(mmkp, x)
	obj = ones(mmkp.o)'*x
	Bleft = mmkpB - mmkp.W*x'
end

#
# 
#
function random_mmkp_solution(mmkp)
	x = [ones(mmkp.n)' ; zeros(mmkp.n, mmkp.o-1)']
	for i = 1:size(x)[2]
		x[:,i] = shuffle(x[:,i])
	end
	return new_mmkp_sol(x)
end

mmkp = random_mmkp(10, 3, 2)
x = random_mmkp_solution(mmkp)
