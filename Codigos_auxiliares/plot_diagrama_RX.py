import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

# Leitura do arquivo, ignorando a primeira linha (cabeçalho)
filename = 'Dados/impedancias.txt'
df = pd.read_csv(filename, skiprows=1, sep=',')

# Converte o DataFrame para um array NumPy de floats
data = df.to_numpy(dtype=float)

# Criação dos números complexos
ZAT = data[:, 0] + 1j * data[:, 1]
ZBT = data[:, 2] + 1j * data[:, 3]
ZCT = data[:, 4] + 1j * data[:, 5]
ZAB = data[:, 6] + 1j * data[:, 7]
ZBC = data[:, 8] + 1j * data[:, 9]
ZCA = data[:, 10] + 1j * data[:, 11]

# Lista de impedâncias e nomes
impedancias = [ZAT, ZBT, ZCT, ZAB, ZBC, ZCA]
nomes = ['Z_{AT}', 'Z_{BT}', 'Z_{CT}', 'Z_{AB}', 'Z_{BC}', 'Z_{CA}']
cores = ['r', 'g', 'b', 'm', 'k', 'c']
marcadores = ['o', 's', '^', 'd', 'x', 'p']

# Zona 1 - Fase-Fase
zona1_ff_x = [0.0, -29.28321, 47.45812, 43.87107, 0.0]
zona1_ff_y = [0.0, 50.72, 50.72, -17.72506, 0.0]

# Zona 1 - Fase-Terra
zona1_ft_x = [0.0, -29.28321, 69.85812, 65.8066, 0.0]
zona1_ft_y = [0.0, 50.72, 50.72, -26.58759, 0.0]

# Zona 2 - Fase-Fase
zona2_ff_x = [0.0, -43.92481, 48.78719, 43.87107, 0.0]
zona2_ff_y = [0.0, 76.08, 76.08, -17.72506, 0.0]

# Zona 2 - Fase-Terra
zona2_ft_x = [0.0, -43.92481, 71.18719, 65.8066, 0.0]
zona2_ft_y = [0.0, 76.08, 76.08, -26.58759, 0.0]

# Criando uma figura com subplots (2 linhas x 3 colunas)
fig, axes = plt.subplots(2, 3, figsize=(15, 8))
fig.suptitle('Impedâncias no Plano R-X', fontsize=16)

for i, ax in enumerate(axes.flat):
    ax.grid(True)
    ax.set_title(nomes[i])
    ax.set_xlabel('Parte Real (Ω)')
    ax.set_ylabel('Parte Imaginária (Ω)')

    # Plota impedância
    ax.plot(np.real(impedancias[i]), np.imag(impedancias[i]),
            linestyle='-', marker=marcadores[i], color=cores[i], label=nomes[i])

    # Zona 1
    ax.plot(zona1_ff_x, zona1_ff_y, '--b',
            linewidth=1.5, label='Zona 1 - Fase-Fase')
    ax.plot(zona1_ft_x, zona1_ft_y, '--r',
            linewidth=1.5, label='Zona 1 - Fase-Terra')

    # Zona 2 (temporizada)
    ax.plot(zona2_ff_x, zona2_ff_y, '--c',
            linewidth=1.5, label='Zona 2 - Fase-Fase')
    ax.plot(zona2_ft_x, zona2_ft_y, '--m',
            linewidth=1.5, label='Zona 2 - Fase-Terra')

    ax.legend(loc='best')

plt.tight_layout(rect=[0, 0, 1, 0.95])  # Ajuste para o título principal
plt.show()
