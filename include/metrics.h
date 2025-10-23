#ifndef METRICS_H
#define METRICS_H

#include <stdint.h>
#include <time.h>

// Estrutura para métricas de CPU
typedef struct
{
    double cpu_user;   // Porcentagem de CPU usado por usuário
    double cpu_system; // Porcentagem de CPU usado pelo sistema
    double cpu_usage;  // Porcentagem total de utilização de CPU
} cpu_metrics_t;

// Estrutura para métricas de memória
typedef struct
{
    uint64_t mem_total; // Memória total em KB
    uint64_t mem_free;  // Memória livre em KB
    uint64_t mem_used;  // Memória usada em KB
} memory_metrics_t;

// Estrutura para métricas de carga
typedef struct
{
    double load_1m;  // Carga média em 1 minuto
    double load_5m;  // Carga média em 5 minutos
    double load_15m; // Carga média em 15 minutos
} load_metrics_t;

// Estrutura principal de métricas
typedef struct
{
    time_t timestamp; // Timestamp da medição
    cpu_metrics_t cpu;
    memory_metrics_t memory;
    load_metrics_t load;
} system_metrics_t;

// Funções para leitura de métricas
int read_cpu_metrics(cpu_metrics_t* cpu);
int read_memory_metrics(memory_metrics_t* memory);
int read_load_metrics(load_metrics_t* load);
int read_system_metrics(system_metrics_t* metrics);

// Funções para formatação e saída
char* metrics_to_json(const system_metrics_t* metrics);
int write_metrics_to_file(const system_metrics_t* metrics, const char* filename);
int create_metrics_directory(void);

// Função principal do monitor
int run_monitor(int interval_seconds);

#endif // METRICS_H
