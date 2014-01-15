#include <stdio.h>
#include <stdlib.h>
#include <jansson.h>
#include "util.h"
#include "pcope.h"

void action_free(Action *act){
	free(act->cost);
	free(act->ymarket);
	free(act->pmarket);
	free(act->recup);
	free(act);
	return;
}

Action *new_random_action(PCOPE *pcope){
	int i, nyears, ntotpers, nres;
	Action *act;

	nyears = pcope->nyears;
	ntotpers = pcope->ntotpers;
	nres = pcope->nresources;

	act = (Action*)malloc(sizeof(Action));
	act->cost = (double*)malloc(nres*sizeof(double));
	act->ymarket = (int*)malloc(nyears*sizeof(int));
	act->pmarket = (int*)malloc(ntotpers*sizeof(int));
	act->recup = (double*)malloc(ntotpers*sizeof(double));

	act->pcope = pcope;

	return act;
}

void pcope_free(PCOPE *pcope){
	int i;

	for( i = 0 ; i < pcope->nacts ; i++)
		action_free(pcope->acts[i]);

	for( i = 0 ; i < pcope->nresources; i++)
		free(pcope->ybudgets[i]);
	free(pcope->ybudgets);

	for( i = 0 ; i < pcope->nresources; i++)
		free(pcope->pbudgets[i]);
	free(pcope->pbudgets);

	free(pcope->ygoals);
	free(pcope->pgoals);
	free(pcope->acts);
	free(pcope);

	return;
}

PCOPE *pcope_random(int nacts, int nyears, int nperiods, int nresources, double irr){
	int i, ntotpers;
	PCOPE *pcope;
	pcope = (PCOPE*)malloc(sizeof(PCOPE));
	pcope->nacts = nacts;
	pcope->nyears = nyears;
	pcope->nperiods = nperiods;
	pcope->ntotpers = ntotpers = nyears*nperiods;
	pcope->nresources = nresources;
	pcope->irr = irr;

	pcope->ygoals = (double*)malloc(nyears*sizeof(double));
	pcope->pgoals = (double*)malloc(ntotpers*sizeof(double));
	pcope->ybudgets = (double**)malloc(nresources*sizeof(double*));
	pcope->pbudgets = (double**)malloc(nresources*sizeof(double*));

	for( i = 0 ; i < nresources ; i++ )
		pcope->ybudgets[i] = (double*)malloc(nyears*sizeof(double));
	for( i = 0 ; i < nresources ; i++ )
		pcope->pbudgets[i] = (double*)malloc(ntotpers*sizeof(double));

	pcope->acts = (Action**)malloc(nacts*sizeof(Action*));

	for( i = 0 ; i < nacts ; i++ )
		pcope->acts[i] = new_random_action(pcope);

	return pcope;
}

json_t *pcope_to_json(PCOPE *pcope){
	int i;
	json_t *p_aux;

	json_t *p_obj = json_object();
	json_t *p_nyrs, *p_nacts, *p_npers, *p_nres;
	json_t *p_irr;

	json_t *p_ggoal;
	json_t *p_ygoals, *p_pgoals;
	json_t *p_gbudget, *p_ybudgets, *p_pbudgets;
	json_t *p_acts;

	p_nacts = json_integer(pcope->nacts);
	p_nyrs = json_integer(pcope->nyears);
	p_npers = json_integer(pcope->nperiods);
	p_nres = json_integer(pcope->nresources);
	p_irr = json_real(pcope->irr);

	p_ggoal = json_real(pcope->ggoal);

	p_ygoals = json_array();
	for( i = 0 ; i < pcope->nyears ; i++ )
		json_array_append(p_ygoals, json_real(pcope->ygoals[i]));

	p_pgoals = json_array();
	for( i = 0 ; i < pcope->ntotpers; i++ )
		json_array_append(p_pgoals, json_real(pcope->pgoals[i]));

	json_object_set(p_obj, "N", p_nacts);
	json_object_set(p_obj, "Y", p_nyrs);
	json_object_set(p_obj, "P", p_npers);
	json_object_set(p_obj, "R", p_nres);
	json_object_set(p_obj, "r", p_irr);
	//json_object_set(p_obj, "", p_ggoal);
	json_object_set(p_obj, "g", p_ygoals);
	//json_object_set(p_obj, "g", p_pgoals);

	return p_obj;
}

int main(int argc, char **argv){
	PCOPE *pcope;
	json_t *j;
	char *str;

	pcope = pcope_random(5, 4, 3, 2, 0.1);
	j = pcope_to_json(pcope);

	str = json_dumps(j, JSON_INDENT(2));
	printf("%s\n", str);

	free(str);
	pcope_free(pcope);
	
	return 0;
}

