#ifndef SOLUTION_VALIDATOR_H
#define SOLUTION_VALIDATOR_H

#include "../../headers/global_modules/generate_initial_population/generate_rSolution.h"

// ------------------------------------------------------------
// Função: isValid
// ------------------------------------------------------------
// Verifica se uma solução (Solution) é válida.
//
// Critérios de validação:
// 1. Cada turbina deve estar corretamente posicionada no grid.
// 2. Não pode haver duas turbinas na mesma posição (x, y).
// 3. Cada turbina deve corresponder à fundação correta.
//
// Parâmetros:
//   - solution: referência para o objeto Solution a ser validado.
//
// Retorno:
//   - true  -> se a solução for válida.
//   - false -> se for detectado algum erro de consistência.
//
// Observação:
//   Essa função depende das variáveis globais:
//   - num_zones
//   - foundations
// ------------------------------------------------------------

bool isValid(Solution &solution);

#endif // SOLUTION_VALIDATOR_H