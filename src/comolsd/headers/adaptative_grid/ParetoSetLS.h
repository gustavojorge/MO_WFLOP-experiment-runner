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
#include "../global_modules/generate_initial_population/population.h"
#include "./ParetoSet.h"

class ParetoSetLS {
    protected:
        std::list<pair<Solution *, bool>> sol;
        range rangeNovo[2], rangeAtual[2];
        int frequencia[NUMEROVERTICES][NUMEROVERTICES];
        Grid g;

        int calcularGridPos(Solution &s);
        void updateGrid();
        void reiniciarRanges();

    public:
        ParetoSetLS();
        virtual ~ParetoSetLS();

        int getPositionCount(Solution &s);
        int getPositionCount(int p);
        bool allExplored();
        std::list<pair<Solution *, bool>> getElementos();
        std::list<pair<Solution *, bool>>::iterator getBegin();
        std::list<pair<Solution *, bool>>::iterator getEnd();
        pair<Solution *, bool> * getRandomUnex();
        
        virtual bool adicionarSol(Solution *s);
        int getSize();
        pair<Solution *, bool>getSolucao(int p);
        void clear();
        bool confereGrid();
};

#endif
