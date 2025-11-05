#!/bin/bash

# Define o diretório base de destino para os resultados do dataset
main_dir="../dataset/brkga"
LOG_DIR="./logs"

# Lista das instâncias específicas a serem criadas
INSTANCES=("501" "502" "503" "504" "505")

# Função de Limpeza Total (Garantida)
cleanup_all_previous_results() {
    echo "--------------------------------------------------------"
    echo "INICIANDO LIMPEZA TOTAL E FORÇADA DE RESULTADOS ANTERIORES..."
    
    # 1. Remove o diretório principal de resultados BRKGA (inclui TODAS as instâncias e runs)
    if [ -d "$main_dir" ]; then
        echo "Removendo o diretório principal de resultados: $main_dir"
        # O comando rm -rf remove recursivamente e força a exclusão
        rm -rf "$main_dir"
    else
        echo "Diretório de resultados $main_dir não encontrado. Sem necessidade de remoção."
    fi

    # 2. Remove o diretório de logs completo
    if [ -d "$LOG_DIR" ]; then
        echo "Removendo o diretório de logs: $LOG_DIR"
        rm -rf "$LOG_DIR"
    else
        echo "Diretório de logs $LOG_DIR não encontrado. Sem necessidade de remoção."
    fi
    
    echo "Limpeza concluída."
    echo "--------------------------------------------------------"
}

echo "------------- Preparando a execução (Limpeza e Criação de Pastas) -------------"

# 1. Executa a limpeza antes de qualquer coisa
cleanup_all_previous_results

# 2. Recria os diretórios necessários
echo "Recriando diretório principal: $main_dir"
mkdir -p "$main_dir"
echo "Recriando diretório de logs: $LOG_DIR"
mkdir -p "$LOG_DIR"

echo "Criação da estrutura de diretórios para as instâncias: ${INSTANCES[@]}..."

for D in "${INSTANCES[@]}"; do
    # Cria as 20 subpastas de run para cada instância
    for subdir in {1..20}; do
        mkdir -p "$main_dir/$D/$subdir"
    done
done

echo "---------- Estrutura de diretórios específica criada com sucesso! ----------"