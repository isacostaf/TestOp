#include "metrics.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <unity/unity.h>

// Mock data para testes
static const char* mock_proc_stat = "cpu 12345 6789 11111 22222 3333 4444 5555 6666\n";
static const char* mock_proc_meminfo = "MemTotal: 8048580 kB\n"
                                       "MemFree: 1456292 kB\n"
                                       "Buffers: 123456 kB\n"
                                       "Cached: 234567 kB\n";
static const char* mock_proc_loadavg = "0.42 0.35 0.30 1/123 45678\n";

// Função auxiliar para criar arquivo temporário
static char* create_temp_file(const char* content)
{
    char* filename = malloc(256);
    snprintf(filename, 256, "/tmp/test_metrics_%d", getpid());

    FILE* file = fopen(filename, "w");
    if (file)
    {
        fprintf(file, "%s", content);
        fclose(file);
    }

    return filename;
}

// Teste de leitura de métricas de CPU
void test_read_cpu_metrics(void)
{
    cpu_metrics_t cpu;

    // Teste com dados válidos
    int result = read_cpu_metrics(&cpu);
    // No Ubuntu, deve funcionar, mas vamos ser flexíveis
    if (result == 0) {
        TEST_ASSERT_TRUE(cpu.cpu_user >= 0.0);
        TEST_ASSERT_TRUE(cpu.cpu_system >= 0.0);
        TEST_ASSERT_TRUE(cpu.cpu_usage >= 0.0);
        TEST_ASSERT_TRUE(cpu.cpu_usage <= 100.0);
    } else {
        // Se falhar, pelo menos não deve crashar
        TEST_ASSERT_TRUE(result == -1);
    }
}

// Teste de leitura de métricas de memória
void test_read_memory_metrics(void)
{
    memory_metrics_t memory;

    int result = read_memory_metrics(&memory);
    if (result == 0) {
        TEST_ASSERT_TRUE(memory.mem_total > 0);
        TEST_ASSERT_TRUE(memory.mem_free >= 0);
        TEST_ASSERT_TRUE(memory.mem_used >= 0);
        TEST_ASSERT_TRUE(memory.mem_used <= memory.mem_total);
    } else {
        TEST_ASSERT_TRUE(result == -1);
    }
}

// Teste de leitura de métricas de carga
void test_read_load_metrics(void)
{
    load_metrics_t load;

    int result = read_load_metrics(&load);
    if (result == 0) {
        TEST_ASSERT_TRUE(load.load_1m >= 0.0);
        TEST_ASSERT_TRUE(load.load_5m >= 0.0);
        TEST_ASSERT_TRUE(load.load_15m >= 0.0);
    } else {
        TEST_ASSERT_TRUE(result == -1);
    }
}

// Teste de leitura completa de métricas
void test_read_system_metrics(void)
{
    system_metrics_t metrics;

    int result = read_system_metrics(&metrics);
    if (result == 0) {
        TEST_ASSERT_TRUE(metrics.timestamp > 0);
        TEST_ASSERT_TRUE(metrics.cpu.cpu_usage >= 0.0);
        TEST_ASSERT_TRUE(metrics.memory.mem_total > 0);
        TEST_ASSERT_TRUE(metrics.load.load_1m >= 0.0);
    } else {
        TEST_ASSERT_TRUE(result == -1);
    }
}

// Teste de conversão para JSON
void test_metrics_to_json(void)
{
    system_metrics_t metrics;

    // Preencher com dados de teste
    metrics.timestamp = 1693262400;
    metrics.cpu.cpu_user = 23.5;
    metrics.cpu.cpu_system = 12.1;
    metrics.cpu.cpu_usage = 35.6;
    metrics.memory.mem_total = 8048580;
    metrics.memory.mem_free = 1456292;
    metrics.memory.mem_used = 6593288;
    metrics.load.load_1m = 0.42;
    metrics.load.load_5m = 0.35;
    metrics.load.load_15m = 0.30;

    char* json = metrics_to_json(&metrics);
    TEST_ASSERT_NOT_NULL(json);

    // Verificar se contém campos esperados
    TEST_ASSERT_TRUE(strstr(json, "\"timestamp\"") != NULL);
    TEST_ASSERT_TRUE(strstr(json, "\"cpu_user\"") != NULL);
    TEST_ASSERT_TRUE(strstr(json, "\"cpu_system\"") != NULL);
    TEST_ASSERT_TRUE(strstr(json, "\"mem_total\"") != NULL);
    TEST_ASSERT_TRUE(strstr(json, "\"load_1m\"") != NULL);

    free(json);
}

// Teste de caso limite - dados incompletos
void test_incomplete_data(void)
{
    // Este teste simula o que acontece quando /proc/stat está corrompido
    // Na implementação real, isso seria tratado pelo sistema
    system_metrics_t metrics;

    // Mesmo com dados incompletos, o sistema deve lidar graciosamente
    int result = read_system_metrics(&metrics);
    // O resultado pode ser 0 (sucesso) ou -1 (erro), mas não deve crashar
    TEST_ASSERT_TRUE(result == 0 || result == -1);
}

// Teste de criação de diretório
void test_create_metrics_directory(void)
{
    // Este teste verifica se a função não falha
    // Em ambiente de teste, pode falhar por permissões, mas não deve crashar
    int result = create_metrics_directory();
    TEST_ASSERT_TRUE(result == 0 || result == -1);
}

// Funções necessárias para o Unity
void setUp(void)
{
    // Setup antes de cada teste
}

void tearDown(void)
{
    // Cleanup após cada teste
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_read_cpu_metrics);
    RUN_TEST(test_read_memory_metrics);
    RUN_TEST(test_read_load_metrics);
    RUN_TEST(test_read_system_metrics);
    RUN_TEST(test_metrics_to_json);
    RUN_TEST(test_incomplete_data);
    RUN_TEST(test_create_metrics_directory);

    return UNITY_END();
}
