include("reader.jl")

typealias Float Float64

# Instance type
type PCOPE
	N :: Int               # N. of actions
	Y :: Int             # N. of years
	R :: Int               # N. of resources
	r :: Float                # Rate of return
	g :: Array{Float}         # Yearly goal
	p :: Array{Float}         # Yearly budget
	m :: Array{Int}           # Global market
	u :: Array{Int, 2}        # Yearly market
	c :: Array{Float,2}       # Action cost
	v :: Array{Float}         # Energy value
	e :: Array{Float,2}       # Energy recover
	d :: Array{Int, 2}        # Depedency matrix
end

function pcope_from_file(filename)
	reader = open_reader(filename)

	N = get(reader)                   # N. of actions
	Y = get(reader)                   # N. of years
	R = get(reader)                   # N. of resources
	r = get(reader, Float)            # Rate of return
	g = get(reader, Float, Y)         # Yearly goal
	p = get(reader, Float, Y)         # Yearly budget
	m = get(reader, Int, N)           # Global market
	u = get(reader, Int, N, Y)        # Yearly market
	c = get(reader, Float, N, R)      # Action cost
	v = get(reader, Float, N)         # Energy value
	e = get(reader, Float, N, Y)      # Energy recover
	d = get(reader, Int, N, N)        # Depedency matrix
	return PCOPE(N, Y, R, r, g, p, m, u, c, v, e, d)
end

p = PCOPE(3, 4, 2, 0.0,
	[1.0, 1.0, 1.0],          # Y. goal
	[2.0, 2.0, 2.0],          # Y. budget
	[10,10,10],               # G. market
	[[3,3,3]',                # Y. market
	 [3,3,3]',      
	 [3,3,3]'],
	[[1.0, 1.0]',             # A. cost
	 [2.0, 0.0]',
	 [0.0, 2.0]'],
	[1.0, 1.0, 1.0],          # E. value
	[[3.0, 0.0, 0.0, 0.0]',   # E. recovery
	 [1.0, 1.0, 1.0, 0.0]',
	 [0.5, 2.0, 0.5, 0.0]'],
	[[0, 0, 0]',              # Depedency matrix
	 [0, 0, 0]',
	 [0, 0, 0]']
	)

