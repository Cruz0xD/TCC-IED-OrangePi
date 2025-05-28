# Projeto IED - Compilação e Execução

Este documento descreve os passos para compilar e executar os códigos fonte do projeto, que utilizam a biblioteca **libiec61850** e estão preparados para rodar tanto em computadores convencionais quanto em Orange Pi.

---

## 1. Preparação: Compilar a biblioteca libiec61850

Antes de compilar os códigos do projeto, é necessário compilar a biblioteca **libiec61850**.

1. Navegue até a pasta da biblioteca:

```bash
cd libiec61850-1.5.1
```

2. Compile a biblioteca usando o `make`:

```bash
make
```

Isso irá gerar a biblioteca compilada na pasta `libiec61850-1.5.1/build`.

---

## 2. Compilação dos códigos fonte

Os códigos fonte estão localizados na pasta `Algoritmos`. As instruções para compilação variam dependendo da plataforma.

---

### 2.1 Código Distancia21_A.c

- **Em computador convencional:**

```bash
gcc -Wall -Iincludes_libiec61850 -Llibiec61850-1.5.1/build \
Algoritmos/Distancia21_A.c Algoritmos/goose_pub.c Algoritmos/fft.c Algoritmos/phasor.c Algoritmos/mergingUnit.c \
-liec61850 -o IED_A -lm -lpthread
```

- **Em Orange Pi:**

```bash
gcc -Wall -Iincludes_libiec61850 -Llibiec61850-1.5.1/build \
Algoritmos/Distancia21_A.c Algoritmos/goose_pub.c Algoritmos/fft.c Algoritmos/phasor.c Algoritmos/mergingUnit.c \
-liec61850 -o IED_A -lm -lpthread -lwiringPi
```

- **Para executar (qualquer plataforma):**

```bash
sudo chrt -f 80 taskset -c 2 ./IED_A
```

---

### 2.2 Código Distancia21_B.c

- **Em computador convencional:**

```bash
gcc -Wall -Iincludes_libiec61850 -Llibiec61850-1.5.1/build \
Algoritmos/Distancia21_B.c Algoritmos/goose_sub.c Algoritmos/fft.c Algoritmos/phasor.c Algoritmos/mergingUnit.c \
-liec61850 -o IED_B -lm -lpthread
```

- **Em Orange Pi:**

```bash
gcc -Wall -Iincludes_libiec61850 -Llibiec61850-1.5.1/build \
Algoritmos/Distancia21_B.c Algoritmos/goose_sub.c Algoritmos/fft.c Algoritmos/phasor.c Algoritmos/mergingUnit.c \
-liec61850 -o IED_B -lm -lpthread -lwiringPi
```

- **Para executar (qualquer plataforma):**

```bash
sudo chrt -f 80 taskset -c 2 ./IED_B
```

---

### 2.3 Código Compser.c

- **Em computador convencional:**

```bash
gcc -Wall -Iincludes_libiec61850 -Llibiec61850-1.5.1/build \
Algoritmos/Compser.c Algoritmos/fft.c Algoritmos/phasor.c Algoritmos/mergingUnit.c \
-o IED_Compser -lm
```

- **Em Orange Pi:**

```bash
gcc -Wall -Iincludes_libiec61850 -Llibiec61850-1.5.1/build \
Algoritmos/Compser.c Algoritmos/fft.c Algoritmos/phasor.c Algoritmos/mergingUnit.c \
-o IED_Compser -lm -lwiringPi
```

- **Para executar (qualquer plataforma):**

```bash
sudo chrt -f 80 taskset -c 2 ./IED_Compser
```

---

## Observações

- O parâmetro `-Iincludes_libiec61850` indica o diretório onde estão os arquivos headers da libiec61850.
- O parâmetro `-Llibiec61850-1.5.1/build` indica o diretório onde está a biblioteca compilada.
- O parâmetro `-lwiringPi` é necessário apenas para compilação no Orange Pi.
- A execução com `sudo chrt -f 80 taskset -c 2` garante prioridade real-time e afinitidade de CPU para melhor desempenho.

---
