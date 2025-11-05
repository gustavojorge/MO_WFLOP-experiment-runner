#!/bin/bash

# --- CONFIGURAÇÃO ---
# Instâncias que você deseja rodar
INSTANCES=("501" "502" "503" "504" "505")
# Caminho para o script que executa o BRKGA para uma única instância/run
BRKGA_SCRIPT="./smaller_scripts/brkga.sh"
# O diretório de logs de alto nível
LOG_DIR="./logs"
# --------------------

# 1. Cria as pastas necessárias primeiro
bash create_specific_folders.sh

# 2. Cria o diretório de logs principal se necessário
mkdir -p "$LOG_DIR"

echo "================================================="
echo "Iniciando a execução serial das instâncias: ${INSTANCES[@]}"
echo "================================================="

for instance in "${INSTANCES[@]}"; do
    # O main.sh original rodava 20 vezes (runs 1 a 20)
    for run in {1..20}; do
        
        LOG_FILE="${LOG_DIR}/${instance}.txt"
        
        echo "Executando: Instância $instance, Run $run (Logs em $LOG_FILE)"

        # Executa o BRKGA para a instância/run e anexa a saída ao arquivo de log.
        # Usa {} >> para agrupar comandos e redirecionar a saída de todos eles.
        {
            echo "---------------------------------------------"
            echo "Rodando BRKGA-NSGA2 com instância $instance, Run: $run"
            
            # Chama o script brkga.sh, passando a instância e o número da rodada (run)
            bash "$BRKGA_SCRIPT" "$instance" "$run"
            
            echo "BRKGA-NSGA2 concluído para $instance, Run: $run."
            echo "---------------------------------------------"
        } >> "$LOG_FILE" 2>&1
        
    done
done

echo "================================================="
echo "Todas as instâncias específicas foram processadas."
echo "================================================="