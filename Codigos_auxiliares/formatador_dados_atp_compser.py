# Gere no ATP Plot um arquivo dados.adf no seguinte formato:

# //ADF file created by "MC's PlotXY" program
# t	vX0004a	vX0004b	vX0004c	iX0004aX0003a	iX0004bX0003b	iX0004cX0003c
# 0	447969	-176229	-271740	-590.862	330.912	259.949
# 1e-006	447949	-176073	-271876	-590.877	330.727	260.15
# 2e-006	447928	-175916	-272012	-590.893	330.542	260.351
# 3e-006	447907	-175759	-272148	-590.908	330.357	260.551
# 4e-006	447886	-175602	-272283	-590.923	330.171	260.752

# A primeira coluna [0] é o instante de tempo da amostra, as colunas [1] a [3] são as tensões VA1, VB1 e VC1 e as colunas [4] a [6] são as tensões VA2, VB2 e VC2
# O código interpola do dados em uma nova grade de tempo com frequência de amostragem fs

import numpy as np

# Constantes
RTP = 500000 / 115  # Escala para tensão
fs = 1920           # Frequência de amostragem. Ajuste aqui conforme o tamanho da janela de amostras definida em mergingUnit.h

# Ler arquivo de dados
with open("dados.adf", "r") as file:
    lines = file.readlines()

# Remover comentários e cabeçalhos
lines = [line.strip() for line in lines if line.strip()
         and not line.startswith("//")]

# Inicializar listas para cada coluna
tempos = []
vA1, vB1, vC1 = [], [], []
vA2, vB2, vC2 = [], [], []

# Processar linhas de dados (ignorando a primeira, que é o cabeçalho de colunas)
for line in lines[1:]:
    valores = line.split()
    if len(valores) >= 7:
        tempos.append(float(valores[0]))
        vA1.append(float(valores[1]) / RTP)
        vB1.append(float(valores[2]) / RTP)
        vC1.append(float(valores[3]) / RTP)
        vA2.append(float(valores[4]) / RTP)
        vB2.append(float(valores[5]) / RTP)
        vC2.append(float(valores[6]) / RTP)

# Nova grade de tempo
t_min, t_max = tempos[0], tempos[-1]
t_novo = np.arange(t_min, t_max, 1/fs)

# Interpolação linear
vA1_interp = np.interp(t_novo, tempos, vA1)
vB1_interp = np.interp(t_novo, tempos, vB1)
vC1_interp = np.interp(t_novo, tempos, vC1)
vA2_interp = np.interp(t_novo, tempos, vA2)
vB2_interp = np.interp(t_novo, tempos, vB2)
vC2_interp = np.interp(t_novo, tempos, vC2)

# Combina os dados e salva no formato tabulado
with open("dados.txt", "w") as f:
    for i in range(len(t_novo)):
        f.write(f"{vA1_interp[i]:.6f}\t{vB1_interp[i]:.6f}\t{vC1_interp[i]:.6f}\t"
                f"{vA2_interp[i]:.6f}\t{vB2_interp[i]:.6f}\t{vC2_interp[i]:.6f}\n")

print("Arquivo 'dados.txt' gerado com sucesso!")

# Deixa os dados nesse formato:
# 103.032870,-40.532670,-62.500200,98.578882,-21.109001,-77.469980


def uniformizar_dados(entrada="Codigos_auxiliares/dados.txt", saida="Dados/dados_ATP.txt"):
    with open(entrada, 'r') as fin, open(saida, 'w') as fout:
        for linha in fin:
            # Remove espaços no começo/fim e separa por qualquer espaço/tab
            valores = linha.strip().split()
            if valores:  # evita escrever linhas vazias
                linha_formatada = ",".join(valores)
                fout.write(linha_formatada + "\n")

    print(f"Arquivo '{saida}' gerado com sucesso.")


# Executar
# Esse formato "dados_Uniformizados.txt" que será usado em mergingUnit.c
uniformizar_dados()
