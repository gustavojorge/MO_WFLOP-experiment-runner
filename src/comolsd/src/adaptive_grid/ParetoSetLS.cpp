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

struct P {
    Solution solution;
    bool checked;
};

int ParetoSetLS::calcularGridPos(Solution &s) {
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

void ParetoSetLS::updateGrid() {
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

void ParetoSetLS::reiniciarRanges() {
    #define INF 1e9
    for (int k = 0; k < NUMOBJETIVOS; k++) {
        rangeAtual[k].min = rangeNovo[k].min = INF;
        rangeAtual[k].max = rangeNovo[k].max = -INF;
    }
    #undef INF
}

ParetoSetLS::ParetoSetLS() {
    reiniciarRanges();
    memset(frequencia, 0, sizeof(frequencia));
}

ParetoSetLS::~ParetoSetLS() {
    clear();
}

int ParetoSetLS::getPositionCount(Solution &s) {
    return g.getPositionCount(calcularGridPos(s));
}

bool ParetoSetLS::allExplored(){
    list<pair<Solution *, bool>>::iterator it;
    for (it = sol.begin(); it != sol.end(); it++){
        if(!(*it).second){
            return false;
        }
    }
    return true;
}

int ParetoSetLS::getPositionCount(int p) {
    return g.getPositionCount(p);
}

list<pair<Solution *, bool>> ParetoSetLS::getElementos() {
    return sol;
}

list<pair<Solution *, bool>>::iterator ParetoSetLS::getBegin(){
    return sol.begin();
}

list<pair<Solution *, bool>>::iterator ParetoSetLS::getEnd(){
    return sol.end();
}

bool ParetoSetLS::adicionarSol(Solution *s) {
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
        delete((*j)->first);
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

int ParetoSetLS::getSize() {
    return sol.size();
}

pair<Solution *, bool>ParetoSetLS::getSolucao(int p) {
    int c = 0;
    list<pair<Solution *, bool>>::iterator i = sol.begin();
    while (i != sol.end()) {
        if (c == p) return *i;
        i++;
        c++;
    }
    return *i;
}

void ParetoSetLS::clear() {
    list<pair<Solution *, bool>>::iterator i = sol.begin(), j;
    while (i != sol.end()) {
        j = i;
        i++;
    }
    sol.clear();
    g.clearGrid();
}

bool ParetoSetLS::confereGrid() {
    unsigned s = 0;
    for (int i = 0; i < g.getSize(); i++) s += g.getPositionCount(i);
    return s == sol.size();
}

list<pair<Solution *, bool>>::iterator ParetoSetLS::getRandomUnex(){
    int size = sol.size();

    int r = rand() % size;

    auto it = sol.begin();

    for(int i = 0; i < r; i++){
        it++;
    }

    do{
        if(it == sol.end()){
            it = sol.begin();
        }else{
            it++;
        }
    } while(!(it)->second);

    return it;
}



#endif