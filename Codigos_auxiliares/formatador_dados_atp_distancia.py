# Gere no ATP Plot um arquivo dados.adf no seguinte formato:

# //ADF file created by "MC's PlotXY" program
# t	vX0004a	vX0004b	vX0004c	iX0004aX0003a	iX0004bX0003b	iX0004cX0003c
# 0	447969	-176229	-271740	-590.862	330.912	259.949
# 1e-006	447949	-176073	-271876	-590.877	330.727	260.15
# 2e-006	447928	-175916	-272012	-590.893	330.542	260.351
# 3e-006	447907	-175759	-272148	-590.908	330.357	260.551
# 4e-006	447886	-175602	-272283	-590.923	330.171	260.752

# A primeira coluna [0] é o isntante de tempo da amostra, as colunas [1] a [3] sao as tensoes VA, VB e VC e as colunas [4] a [6] sao as correntes IA, IB e IC
# O código interpola do dados em uma nova grade de tempo com frequência de amostragem fs

import numpy as np

# Constantes
RTP = 500000 / 115  # Escala para tensão
RTC = 600 / 5       # Escala para corrente
fs = 1920          # Frequência de amostragem. Ajuste aqui conforme o tamanho da janela de amostras definida em mergingUnit.h

# Ler arquivo de dados
with open("dados.adf", "r") as file:
    lines = file.readlines()

# Remover comentários e cabeçalhos
lines = [line.strip() for line in lines if line.strip()
         and not line.startswith("//")]

# Inicializar listas para cada coluna
tempos = []
vA, vB, vC = [], [], []
iA, iB, iC = [], [], []

# Processar linhas de dados (ignorando a primeira, que é o cabeçalho de colunas)
for line in lines[1:]:
    valores = line.split()
    if len(valores) >= 7:
        tempos.append(float(valores[0]))
        vA.append(float(valores[1]) / RTP)
        vB.append(float(valores[2]) / RTP)
        vC.append(float(valores[3]) / RTP)
        iA.append(float(valores[4]) / RTC)
        iB.append(float(valores[5]) / RTC)
        iC.append(float(valores[6]) / RTC)

# Nova grade de tempo
t_min, t_max = tempos[0], tempos[-1]
t_novo = np.arange(t_min, t_max, 1/fs)

# Interpolação linear
vA_interp = np.interp(t_novo, tempos, vA)
vB_interp = np.interp(t_novo, tempos, vB)
vC_interp = np.interp(t_novo, tempos, vC)
iA_interp = np.interp(t_novo, tempos, iA)
iB_interp = np.interp(t_novo, tempos, iB)
iC_interp = np.interp(t_novo, tempos, iC)

# Combina os dados e salva no formato tabulado
with open("dados.txt", "w") as f:
    for i in range(len(t_novo)):
        f.write(f"{vA_interp[i]:.6f}\t{vB_interp[i]:.6f}\t{vC_interp[i]:.6f}\t"
                f"{iA_interp[i]:.6f}\t{iB_interp[i]:.6f}\t{iC_interp[i]:.6f}\n")

print("Arquivo 'dados.txt' gerado com sucesso!")

# Deixa os dados nesse formato:
# 103.032870,-40.532670,-62.500200,-4.923850,2.757600,2.166242
# 98.578882,-21.109001,-77.469980,-4.895847,1.906019,2.989828


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
