#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "metrics.h"

void print_usage(const char *program_name) {
    printf("Uso: %s [intervalo_segundos]\n", program_name);
    printf("  intervalo_segundos: Intervalo entre medições (mínimo 5, padrão 5)\n");
    printf("\nExemplo:\n");
    printf("  %s        # Monitora a cada 5 segundos\n", program_name);
    printf("  %s 10     # Monitora a cada 10 segundos\n", program_name);
}

int main(int argc, char *argv[]) {
    int interval = 5; // Padrão: 5 segundos
    
    // Parse dos argumentos
    if (argc > 1) {
        interval = atoi(argv[1]);
        if (interval < 5) {
            printf("Aviso: Intervalo mínimo é 5 segundos. Usando 5 segundos.\n");
            interval = 5;
        }
    }
    
    // Verificar se é pedido de ajuda
    if (argc > 1 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)) {
        print_usage(argv[0]);
        return 0;
    }
    
    printf("=== Sistema de Monitoramento TP1 ===\n");
    printf("Monitorando sistema operacional...\n\n");
    
    // Executar monitoramento
    return run_monitor(interval);
}
