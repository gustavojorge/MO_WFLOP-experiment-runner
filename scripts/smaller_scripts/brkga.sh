#!/bin/bash

# Tratamento de sinais para propagar SIGINT (Ctrl+C) corretamente
# Quando o script recebe SIGINT, simplesmente propaga para processos filhos e sai
trap 'exit 130' SIGINT SIGTERM

instance="${1:-A}" 
run="${2:-1}"

echo "Executing BRKGA-NSGA2 with: Instance=$instance, Run=$run"

log_dir="../dataset/brkga/$instance/$run"
log_file="$log_dir/log.txt"

path="../dataset/brkga/$instance/$run/"
log_path="../dataset/nsga2/$instance/$run/log.txt"

# Verifica se o arquivo de log existe antes de ler
if [ ! -f "$log_path" ]; then
    echo "ERROR: Arquivo de log não encontrado: $log_path"
    exit 1
fi

angle=$(grep "Angle:" "$log_path" | awk '{print $2}')
wind=$(grep "Wind:" "$log_path" | awk '{print $2}')

mkdir -p "$log_dir"

# PROTEÇÕES: Limites de memória e tempo para evitar travamentos
# Limite de memória virtual: 8GB (8388608 KB)

# Limite de memória residente: 4GB (4194304 KB)  

# Limite de tempo: 2 horas (7200 segundos)
TIMEOUT_SECONDS=7200

{
    echo "============================== LOG =============================="
    echo "Instance: $instance"
    echo "Run: $run"
    echo "Metaheuristic: BRKGA-NSGA2"
    echo "Execution started at: $(date)"
    echo "Memory limits: Virtual=8GB, Resident=4GB"
    echo "Time limit: ${TIMEOUT_SECONDS}s (2 hours)"
    echo "================================================================="

    # Executa com timeout e monitoramento de memória
    # IMPORTANTE: Não usar bash -c ou sh -c desnecessariamente, pois bloqueia propagação de sinais
    # Executa diretamente o comando para permitir propagação correta de SIGINT (Ctrl+C)
    START_TIME=$(date +%s)
    EXIT_CODE=0
    
    if command -v timeout >/dev/null 2>&1; then
        # timeout propaga SIGINT corretamente quando executado diretamente
        timeout ${TIMEOUT_SECONDS}s ./exe/brkga "$instance" "$path" "$angle" "$wind"
        EXIT_CODE=$?
    else
        # Se timeout não estiver disponível, executa diretamente (sem subprocessos)
        # O trap já cuida de propagar o sinal corretamente
        ./exe/brkga "$instance" "$path" "$angle" "$wind"
        EXIT_CODE=$?
    fi
    
    # Calcula tempo de execução
    END_TIME=$(date +%s)
    ELAPSED=$((END_TIME - START_TIME))
    echo "Tempo de execução: ${ELAPSED}s"
    
    # Trata códigos de saída especiais
    if [ $EXIT_CODE -ne 0 ]; then
        if [ $EXIT_CODE -eq 124 ]; then
            echo "ERROR: Processo foi interrompido por timeout (${TIMEOUT_SECONDS}s)"
        elif [ $EXIT_CODE -eq 130 ]; then
            echo "ERROR: Processo foi interrompido pelo usuário (SIGINT/Ctrl+C)"
            exit 130
        else
            echo "ERROR: Processo terminou com código de erro: $EXIT_CODE"
        fi
        exit $EXIT_CODE
    fi

    echo "Execution ended at: $(date)"
    echo "================================================================="
} > "$log_file" 2>&1
