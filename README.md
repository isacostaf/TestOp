# TP1 - Sistema de Monitoramento Básico

## 📋 Descrição
Sistema de monitoramento e observabilidade para capturar métricas do sistema operacional em tempo real, desenvolvido para o Trabalho Prático 1 de Sistemas Operativos.

## 🎯 Funcionalidades
- **Monitoramento de CPU**: Utilização por usuário e sistema
- **Monitoramento de Memória**: Total, livre e usada
- **Monitoramento de Carga**: Médias de 1, 5 e 15 minutos
- **Saída em JSON**: Formato NDJSON para integração com Grafana
- **Intervalo Configurável**: Mínimo 5 segundos
- **Armazenamento**: Diretório `/var/lib/monitoreo/`

## 🚀 Como Usar

### Compilação
```bash
# Instalar dependências
conan install . --output-folder=. --build=missing

# Configurar CMake
cmake -B build -DCMAKE_TOOLCHAIN_FILE=build/Release/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release

# Compilar
cmake --build build
```

### Execução
```bash
# Monitorar a cada 5 segundos (padrão)
./build/TP1

# Monitorar a cada 10 segundos
./build/TP1 10

# Ver ajuda
./build/TP1 --help
```

### Testes
```bash
# Executar testes
./build/test_metrics

# Executar testes via CTest
ctest --test-dir build
```

## 📊 Métricas Capturadas

### CPU
- `cpu_user`: Porcentagem de CPU usado por usuário
- `cpu_system`: Porcentagem de CPU usado pelo sistema
- `cpu_usage`: Porcentagem total de utilização

### Memória
- `mem_total`: Memória total em KB
- `mem_free`: Memória livre em KB
- `mem_used`: Memória usada em KB

### Carga
- `load_1m`: Carga média em 1 minuto
- `load_5m`: Carga média em 5 minutos
- `load_15m`: Carga média em 15 minutos

## 📁 Estrutura de Arquivos
```
TP1/
├── src/
│   ├── main.c          # Programa principal
│   ├── metrics.c       # Implementação das métricas
│   └── test_metrics.c  # Testes unitários
├── include/
│   └── metrics.h       # Headers das métricas
├── CMakeLists.txt      # Configuração CMake
└── conanfile.txt       # Dependências Conan
```

## 🔧 Dependências
- **cJSON**: Para geração de JSON
- **Unity**: Para testes unitários
- **CMake**: Sistema de build
- **Conan**: Gerenciador de dependências

## 📈 Formato de Saída
Arquivo: `/var/lib/monitoreo/metrics-YYYYMMDD.log`

Exemplo de linha JSON:
```json
{"timestamp":1693262400,"cpu_user":23.5,"cpu_system":12.1,"mem_total":8048580,"mem_free":1456292,"load_1m":0.42,"load_5m":0.35,"load_15m":0.30}
```

## 🧪 Testes
- **Cobertura mínima**: 20%
- **Casos de teste**: Leitura de métricas, conversão JSON, casos limite
- **Integração**: GitHub Actions com Ubuntu

## 🐧 Compatibilidade
- **Desenvolvido para**: Linux (Ubuntu)
- **Arquivos do sistema**: `/proc/stat`, `/proc/meminfo`, `/proc/loadavg`
- **CI/CD**: GitHub Actions com Ubuntu
