#ifndef PARETOSETLS_CPP
#define PARETOSETLS_CPP

#include <list>
#include <map>
#include <math.h>
#include <cstdio>
#include <iomanip>
#include <fstream>
#include <iostream>

#include "../../headers/adaptative_grid/param.h"
#include "../../headers/adaptative_grid/Grid.h"
#include "../../headers/global_modules/generate_initial_population/generate_rSolution.h"
#include "../../headers/adaptative_grid/ParetoSetLS.h"

using namespace std;

double getObj(Solution *s, int obj){
    if(obj == 0){
        return s->fitness.first;
    }

    return s->fitness.second;
}

bool dominatesP(Solution &s1, Solution &s2){
    return (s1.fitness.first >= s2.fitness.first && s1.fitness.second >= s2.fitness.second) &&
           (s1.fitness.first >  s2.fitness.first || s2.fitness.second >  s2.fitness.second);
}

bool equals(Solution &s1, Solution &s2){
    return s1.fitness.first == s2.fitness.first && s1.fitness.second == s2.fitness.second;
}

int ParetoSet::calcularGridPos(Solution &s) {
    int bit = 0;
    int gridPos = 0;
    for (int obj = 0; obj < NUMOBJETIVOS; obj++) {
        double inicio = rangeAtual[obj].min, fim = rangeAtual[obj].max, meio = (inicio + fim) / 2.0;
        for (int k = 0; k < PROFUNDIDADEGRID; k++) {
            if (getObj(&s, obj) >= meio) {
                gridPos |= (1 << bit);
                inicio = meio;
            } else {
                fim = meio;
            }
            meio = (inicio + fim) / 2.0;
            bit++;
        }
    }
    return gridPos;
}

void ParetoSet::updateGrid() {
    g.clearGrid();

    list<pair<Solution *, bool>>::iterator it = sol.begin();
    reiniciarRanges();
    while (it != sol.end()) {
        for (int k = 0; k < NUMOBJETIVOS; k++) {
            rangeAtual[k].min = rangeNovo[k].min = min(rangeAtual[k].min, getObj(it->first, k));
            rangeAtual[k].max = rangeNovo[k].max = max(rangeAtual[k].max, getObj(it->first, k));
        }
        it++;
    }

    it = sol.begin();
    while (it != sol.end()) {
        g.addGrid(calcularGridPos(*it->first));
        it++;
    }
}

void ParetoSet::reiniciarRanges() {
    #define INF 1e9
    for (int k = 0; k < NUMOBJETIVOS; k++) {
        rangeAtual[k].min = rangeNovo[k].min = INF;
        rangeAtual[k].max = rangeNovo[k].max = -INF;
    }
    #undef INF
}

ParetoSet::ParetoSet() {
    reiniciarRanges();
    memset(frequencia, 0, sizeof(frequencia));
}

ParetoSet::~ParetoSet() {
    clear();
}

int ParetoSet::getPositionCount(Solution &s) {
    return g.getPositionCount(calcularGridPos(s));
}

int ParetoSet::getPositionCount(int p) {
    return g.getPositionCount(p);
}

list<pair<Solution *, bool>> ParetoSet::getElementos() {
    return sol;
}

bool ParetoSet::adicionarSol(Solution *s) {
    ASS ( assert( confereGrid() ); )
    /* nem testa as solucoes piores */
	//if (s->getObj(0) >= ranges[0].max && s->getObj(1) >= ranges[1].max) {
		//fprintf(stderr,"Nem testei!\n");
		//return false;
	//}

    /* percorre o vetor de solucoes e de valores e, caso exista solucao dominada, retira e retorna true. caso contrario, retorna false */
    list<pair<Solution *, bool>>::iterator i = sol.begin();
    list<list<pair<Solution *, bool>>::iterator> remover;
    while (i != sol.end()) {
        if (dominatesP(*s, *i->first)) {
            remover.push_back(i);
            //printf("Dominada -> (%.3lf,%.3lf) por (%.3lf,%.3lf)!\n",(**i).getObj(0),(**i).getObj(1),s->getObj(0),s->getObj(1));
        }
        if (dominatesP(*i->first, *s) || equals(*i->first, *s)) {
            return false;
        }
        i++;
    }

    list<list<pair<Solution *, bool>>::iterator>::iterator j = remover.begin();
    while (j != remover.end()) {
        // remove do grid
        g.removeGrid(calcularGridPos(*(*j)->first));

        // remove do conjunto pareto
        sol.erase(*j);
        j++;
    }

    Solution *t = new Solution;
    *t = *s;
    // adiciona ao conjunto pareto
    sol.push_front(make_pair(t, false));
    // adiciona ao grid
    g.addGrid(calcularGridPos(*t));

    for (int k = 0; k < NUMOBJETIVOS; k++) {
        rangeNovo[k].min = min(rangeNovo[k].min, getObj(t, k));
        rangeNovo[k].max = max(rangeNovo[k].max, getObj(t, k));
    }

    // se houve uma mudanca grande nos ranges (maior que 10% valor), atualizar o grid
    for (int k = 0; k < NUMOBJETIVOS; k++) {
        if (fabs(rangeNovo[k].min - rangeAtual[k].min) > 0.1 * rangeAtual[k].min ||
            fabs(rangeNovo[k].max - rangeAtual[k].max) > 0.1 * rangeAtual[k].max) {
            //fprintf(stderr,"Atualizando grid!\n");
            updateGrid();
            break;
        }
    }

    ASS ( assert( confereGrid() ); )
    return true;
}

int ParetoSet::getSize() {
    return sol.size();
}

pair<Solution *, bool>ParetoSet::getSolucao(int p) {
    int c = 0;
    list<pair<Solution *, bool>>::iterator i = sol.begin();
    while (i != sol.end()) {
        if (c == p) return *i;
        i++;
        c++;
    }
    return *i;
}

void ParetoSet::clear() {
    list<pair<Solution *, bool>>::iterator i = sol.begin(), j;
    while (i != sol.end()) {
        j = i;
        i++;
    }
    sol.clear();
    g.clearGrid();
}

bool ParetoSet::confereGrid() {
    unsigned s = 0;
    for (int i = 0; i < g.getSize(); i++) s += g.getPositionCount(i);
    return s == sol.size();
}


#endif