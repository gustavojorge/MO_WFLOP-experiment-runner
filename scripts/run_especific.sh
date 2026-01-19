#!/bin/bash

# Tratamento de sinais para permitir interrupção com Ctrl+C
INTERRUPTED=0
cleanup() {
    INTERRUPTED=1
    echo ""
    echo "================================================="
    echo "Execução interrompida pelo usuário (Ctrl+C)"
    echo "Finalizando processos em execução..."
    echo "================================================="
    # Mata processos filhos se necessário
    jobs -p | xargs -r kill 2>/dev/null
    exit 130
}

# Captura SIGINT (Ctrl+C) e SIGTERM
trap cleanup SIGINT SIGTERM

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

# Verifica se foi interrompido
if [ $INTERRUPTED -eq 1 ]; then
    exit 130
fi

# 2. Cria o diretório de logs principal se necessário
mkdir -p "$LOG_DIR"

echo "================================================="
echo "Iniciando a execução serial das instâncias: ${INSTANCES[@]}"
echo "Pressione Ctrl+C para interromper a execução"
echo "================================================="

for instance in "${INSTANCES[@]}"; do
    # Verifica se foi interrompido antes de continuar
    if [ $INTERRUPTED -eq 1 ]; then
        break
    fi
    
    # O main.sh original rodava 20 vezes (runs 1 a 20)
    for run in {1..20}; do
        # Verifica se foi interrompido antes de cada execução
        if [ $INTERRUPTED -eq 1 ]; then
            break 2  # Quebra ambos os loops
        fi
        
        LOG_FILE="${LOG_DIR}/${instance}.txt"
        
        echo "Executando: Instância $instance, Run $run (Logs em $LOG_FILE)"

        # Executa o BRKGA para a instância/run e anexa a saída ao arquivo de log.
        # Usa {} >> para agrupar comandos e redirecionar a saída de todos eles.
        {
            echo "---------------------------------------------"
            echo "Rodando BRKGA-NSGA2 com instância $instance, Run: $run"
            echo "Timestamp: $(date)"
            
            # Chama o script brkga.sh, passando a instância e o número da rodada (run)
            # Captura código de saída para detectar problemas
            if bash "$BRKGA_SCRIPT" "$instance" "$run"; then
                echo "✓ BRKGA-NSGA2 concluído com SUCESSO para $instance, Run: $run."
            else
                EXIT_CODE=$?
                # Verifica se foi interrompido (código 130 = SIGINT)
                if [ $EXIT_CODE -eq 130 ] || [ $INTERRUPTED -eq 1 ]; then
                    echo "✗ BRKGA-NSGA2 foi INTERROMPIDO para $instance, Run: $run"
                    INTERRUPTED=1
                    break 2
                else
                    echo "✗ BRKGA-NSGA2 FALHOU para $instance, Run: $run (código: $EXIT_CODE)"
                    echo "  Verifique o log em: ${LOG_DIR}/${instance}.txt"
                fi
            fi
            echo "Timestamp: $(date)"
            echo "---------------------------------------------"
        } >> "$LOG_FILE" 2>&1
        
        # Verifica se foi interrompido após a execução
        if [ $INTERRUPTED -eq 1 ]; then
            break 2
        fi
    done
done

if [ $INTERRUPTED -eq 0 ]; then
    echo "================================================="
    echo "Todas as instâncias específicas foram processadas."
    echo "================================================="
fi