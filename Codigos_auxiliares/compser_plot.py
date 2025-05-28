import matplotlib.pyplot as plt
import numpy as np

# Lê os dados
with open("Dados/angulos.txt", "r") as f:
    linhas = f.readlines()

nomes_colunas = [nome.strip().replace('_', '/')
                 for nome in linhas[0].split(',')]  # Substitui _ por /
dados = np.genfromtxt("Dados/angulos.txt", delimiter=",", skip_header=1)
num_janelas = np.arange(dados.shape[0]) + 1

# Cria figura com subplots
fig, axs = plt.subplots(2, 3, figsize=(16, 8), sharex=True)
axs = axs.flatten()

colors = plt.cm.tab10.colors

for i, nome in enumerate(nomes_colunas):
    axs[i].plot(num_janelas, dados[:, i],
                color=colors[i % len(colors)],
                linewidth=2,
                marker='o',
                markersize=4)

    axs[i].axhline(0, color='gray', linestyle='--', linewidth=1)
    axs[i].set_title(nome, fontsize=12)
    axs[i].set_ylabel("Ângulo (°)")
    axs[i].set_xlabel("Número da Janela")  # Eixo X com rótulo em todos

    # Força a exibição dos números do eixo X
    axs[i].tick_params(axis='x', labelbottom=True)
    axs[i].grid(True, linestyle='--', alpha=0.7)

# Título geral e layout final
fig.suptitle("Ângulos da Razão dos Fasores de Tensão por Janela Processada", fontsize=16)
plt.tight_layout(rect=[0, 0.03, 1, 0.95])
plt.show()
