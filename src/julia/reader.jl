type Reader
	dat :: Array{String, 1}    # The stream
	pos :: Int                 # Position of reader
end

# Creates a reader from file.
function open_reader(filename)
	return Reader(split(readall(open(filename))), 1)
end

function reader_pop(reader, tipe=Int)
	x = convert(tipe, parse(reader.dat[reader.pos]))
	reader.pos += 1
	return x
end

function get(reader, tipe=Int, n=0, m=0)
	# Single
	x=0
	if n == 0
		x = reader_pop(reader, tipe)
		return x
	# Array
	elseif m == 0
		xs = Array{tipe, n}
		for i = 1:n
			xs[i] = reader_pop(reader, tipe)
		end
		return xs
	# Matrice
	else
		xss = Array{tipe, n, m}
		for i = 1:n
			for j = 1:m
				xss[i, j] = reader_pop(reader, tipe)
			end
		end
		return xss
	end
end

