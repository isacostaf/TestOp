#include "metrics.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <cjson/cjson.h>

#define PROC_STAT_PATH "/proc/stat"
#define PROC_MEMINFO_PATH "/proc/meminfo"
#define PROC_LOADAVG_PATH "/proc/loadavg"
#define METRICS_DIR "/var/lib/monitoreo"

// Função auxiliar para ler arquivo completo
static char* read_file_content(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        return NULL;
    }
    
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char *content = malloc(length + 1);
    if (!content) {
        fclose(file);
        return NULL;
    }
    
    fread(content, 1, length, file);
    content[length] = '\0';
    fclose(file);
    
    return content;
}

// Função para ler métricas de CPU do /proc/stat
int read_cpu_metrics(cpu_metrics_t *cpu) {
    char *content = read_file_content(PROC_STAT_PATH);
    if (!content) {
        return -1;
    }
    
    // Parse da primeira linha (cpu aggregate)
    unsigned long user, nice, system, idle, iowait, irq, softirq, steal;
    int parsed = sscanf(content, "cpu %lu %lu %lu %lu %lu %lu %lu %lu", 
                       &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal);
    
    free(content);
    
    if (parsed < 4) {
        return -1;
    }
    
    // Calcular totais
    unsigned long total_idle = idle + iowait;
    unsigned long total_non_idle = user + nice + system + irq + softirq + steal;
    unsigned long total = total_idle + total_non_idle;
    
    if (total == 0) {
        return -1;
    }
    
    // Calcular percentuais
    cpu->cpu_user = (double)(user + nice) / total * 100.0;
    cpu->cpu_system = (double)system / total * 100.0;
    cpu->cpu_usage = (double)total_non_idle / total * 100.0;
    
    return 0;
}

// Função para ler métricas de memória do /proc/meminfo
int read_memory_metrics(memory_metrics_t *memory) {
    char *content = read_file_content(PROC_MEMINFO_PATH);
    if (!content) {
        return -1;
    }
    
    char *line = strtok(content, "\n");
    unsigned long mem_total = 0, mem_free = 0, buffers = 0, cached = 0;
    
    while (line) {
        if (strncmp(line, "MemTotal:", 9) == 0) {
            sscanf(line, "MemTotal: %lu kB", &mem_total);
        } else if (strncmp(line, "MemFree:", 8) == 0) {
            sscanf(line, "MemFree: %lu kB", &mem_free);
        } else if (strncmp(line, "Buffers:", 8) == 0) {
            sscanf(line, "Buffers: %lu kB", &buffers);
        } else if (strncmp(line, "Cached:", 7) == 0) {
            sscanf(line, "Cached: %lu kB", &cached);
        }
        line = strtok(NULL, "\n");
    }
    
    free(content);
    
    if (mem_total == 0) {
        return -1;
    }
    
    memory->mem_total = mem_total;
    memory->mem_free = mem_free;
    memory->mem_used = mem_total - mem_free - buffers - cached;
    
    return 0;
}

// Função para ler métricas de carga do /proc/loadavg
int read_load_metrics(load_metrics_t *load) {
    FILE *file = fopen(PROC_LOADAVG_PATH, "r");
    if (!file) {
        return -1;
    }
    
    int parsed = fscanf(file, "%lf %lf %lf", &load->load_1m, &load->load_5m, &load->load_15m);
    fclose(file);
    
    return (parsed == 3) ? 0 : -1;
}

// Função para ler todas as métricas do sistema
int read_system_metrics(system_metrics_t *metrics) {
    metrics->timestamp = time(NULL);
    
    if (read_cpu_metrics(&metrics->cpu) != 0) {
        return -1;
    }
    
    if (read_memory_metrics(&metrics->memory) != 0) {
        return -1;
    }
    
    if (read_load_metrics(&metrics->load) != 0) {
        return -1;
    }
    
    return 0;
}

// Função para converter métricas para JSON
char* metrics_to_json(const system_metrics_t *metrics) {
    cJSON *json = cJSON_CreateObject();
    if (!json) {
        return NULL;
    }
    
    cJSON_AddNumberToObject(json, "timestamp", (double)metrics->timestamp);
    cJSON_AddNumberToObject(json, "cpu_user", metrics->cpu.cpu_user);
    cJSON_AddNumberToObject(json, "cpu_system", metrics->cpu.cpu_system);
    cJSON_AddNumberToObject(json, "mem_total", (double)metrics->memory.mem_total);
    cJSON_AddNumberToObject(json, "mem_free", (double)metrics->memory.mem_free);
    cJSON_AddNumberToObject(json, "load_1m", metrics->load.load_1m);
    cJSON_AddNumberToObject(json, "load_5m", metrics->load.load_5m);
    cJSON_AddNumberToObject(json, "load_15m", metrics->load.load_15m);
    
    char *json_string = cJSON_Print(json);
    cJSON_Delete(json);
    
    return json_string;
}

// Função para criar diretório de métricas
int create_metrics_directory(void) {
    struct stat st = {0};
    
    if (stat(METRICS_DIR, &st) == -1) {
        if (mkdir(METRICS_DIR, 0755) == -1) {
            return -1;
        }
    }
    
    return 0;
}

// Função para escrever métricas em arquivo
int write_metrics_to_file(const system_metrics_t *metrics, const char *filename) {
    char *json_string = metrics_to_json(metrics);
    if (!json_string) {
        return -1;
    }
    
    FILE *file = fopen(filename, "a");
    if (!file) {
        free(json_string);
        return -1;
    }
    
    fprintf(file, "%s\n", json_string);
    fclose(file);
    free(json_string);
    
    return 0;
}

// Função principal do monitor
int run_monitor(int interval_seconds) {
    if (interval_seconds < 5) {
        interval_seconds = 5; // Mínimo de 5 segundos
    }
    
    if (create_metrics_directory() != 0) {
        fprintf(stderr, "Erro ao criar diretório de métricas\n");
        return -1;
    }
    
    // Gerar nome do arquivo com data atual
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char filename[256];
    snprintf(filename, sizeof(filename), "%s/metrics-%04d%02d%02d.log", 
             METRICS_DIR, tm_info->tm_year + 1900, tm_info->tm_mon + 1, tm_info->tm_mday);
    
    printf("Iniciando monitoramento...\n");
    printf("Intervalo: %d segundos\n", interval_seconds);
    printf("Arquivo de saída: %s\n", filename);
    printf("Pressione Ctrl+C para parar\n\n");
    
    while (1) {
        system_metrics_t metrics;
        
        if (read_system_metrics(&metrics) == 0) {
            if (write_metrics_to_file(&metrics, filename) == 0) {
                printf("Métricas registradas: CPU=%.1f%%, Mem=%lluKB, Load=%.2f\n", 
                       metrics.cpu.cpu_usage, (unsigned long long)metrics.memory.mem_used, metrics.load.load_1m);
            } else {
                fprintf(stderr, "Erro ao escrever métricas\n");
            }
        } else {
            fprintf(stderr, "Erro ao ler métricas do sistema\n");
        }
        
        sleep(interval_seconds);
    }
    
    return 0;
}
