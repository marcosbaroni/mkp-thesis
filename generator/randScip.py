

def main():
    if(len(sys.argv) != 5):
        print "seed, alpha, years, camps"
        return
    seed = int(sys.argv[1])
    alpha = float(sys.argv[2])
    years = int(sys.argv[3])
    camp = int(sys.argv[4])
	gen(seed, alpha, years, camp);

main()
