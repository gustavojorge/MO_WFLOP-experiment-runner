#ifndef PARETOSETLS_H
#define PARETOSETLS_H

#include <list>
#include <map>
#include <cmath>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include "param.h"
#include "Grid.h"
#include "../global_modules/generate_initial_population/generate_rSolution.h"

typedef struct {
    double min, max;
} range;

double getObj(Solution *s, int obj);
bool dominatesP(Solution &s1, Solution &s2);
bool equals(Solution &s1, Solution &s2);

class ParetoSet {
    protected:
        std::list<pair<Solution *, bool>> sol;
        range rangeNovo[2], rangeAtual[2];
        int frequencia[NUMEROVERTICES][NUMEROVERTICES];
        Grid g;

        int calcularGridPos(Solution &s);
        void updateGrid();
        void reiniciarRanges();

    public:
        ParetoSet();
        virtual ~ParetoSet();

        int getPositionCount(Solution &s);
        int getPositionCount(int p);
        std::list<pair<Solution *, bool>> getElementos();
        
        virtual bool adicionarSol(Solution *s);
        int getSize();
        pair<Solution *, bool>getSolucao(int p);
        void clear();
        bool confereGrid();
};

#endif
