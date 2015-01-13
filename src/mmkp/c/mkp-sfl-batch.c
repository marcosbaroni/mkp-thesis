#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "mmkp/mkp.h"
#include "mmkp/util.h"
#include "mmkp/sfl.h"

/* <n> <m> <tight> <n meme> <size meme> <size submeme> <subniter> <crossing> <new sol> <best_iter> <lp> <opt> <lp trunc> <obj>*/
int main(int argc, char **argv){
	MKP *mkp;
	SFL_Interface *sfli;
	int i, seed, n, m, nmeme, size_meme, size_submeme, niter, subniter, cross, newsol, best_iter;
	long long max_coef;
	double opt, lp_trunc, obj, tight;
	MKPSol *sol;
	niter = 300;
	max_coef = 1000;


	if(argc < 2) {
		printf("usage: %s <seed>\n", argv[0]);
		printf("prints: n;m;tight;nmeme;size_meme;size_submeme;subniter;cross;newsol;best_iter;opt;lp_trunc;obj\n");
		return 1;
	}
	seed = atoll(argv[1]);
	srand(seed);

  /* n */
  for( n = 100; n < 501 ; n += 100 ){                                    /* n */
    /* m */
    for( m = 10 ; m < 31 ; m += 10 ){                                    /* m */
      /* tight */
      for( tight = 0.25 ; tight < 1.0 ; tight += 0.25 ){           /* tight */
        /* generating problem */
        mkp = mkp_random(n, m, tight, max_coef);

        /* getting OPT */
        sol = mkpsol_solve_with_scip(mkp, 1200.0, 0);
        opt = (double)sol->obj;
        mkpsol_free(sol);

        /* getting LP trunc */
        sol = mkp_get_lp_trunc(mkp);
        lp_trunc = (double)sol->obj;
        mkpsol_free(sol);

        /* n. of memeplexs */
        for( nmeme = 10 ; nmeme < 41 ; nmeme += 10 ){          /* n. of meme */
        /* size of memeplexs */
          for( size_meme = 10 ; size_meme < 41 ; size_meme += 10){ /* size of meme */
            /* size of submemeplexs */
            for( size_submeme = 5 ; size_submeme < size_meme ; size_submeme += 5 ){
              /* subniter */
              for( subniter = 10 ; subniter < 41 ; subniter += 5 ){
                /* crossing */
                for( cross = 1 ; cross < 4 ; cross++ ){
                  for( newsol = 1 ; newsol < 3 ; newsol++ ){
                    sfli = mkp_sfl_interface(cross, newsol);
									  for( i = 0 ; i < 5 ; i++ ){
                      sol = (MKPSol*)sfl(sfli, mkp, nmeme, size_meme, size_submeme, niter, &best_iter);
										  obj = (double)sol->obj;

                      printf("%d;", n);
                      printf("%d;", m);
                      printf("%.2lf;", tight);
                      printf("%d;", nmeme);
                      printf("%d;", size_meme);
                      printf("%d;", size_submeme);
                      printf("%d;", subniter);
                      printf("%d;", cross);
                      printf("%d;", newsol);
                      printf("%d;", best_iter);
                      printf("%.0lf;", opt);
                      printf("%.0lf;", lp_trunc);
                      printf("%.0lf", obj);
                      printf("\n");
										}
                    sfli_free(sfli);
								  }
                }
              }
            }
          }
        }
      }
    }
  }
	return 0;
}

