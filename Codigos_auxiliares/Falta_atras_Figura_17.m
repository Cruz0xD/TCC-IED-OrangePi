clc; close all; clear all;

% Definição do intervalo de XL1 de 10⁻³ a 3 p.u. para permitir escala log
XL1 = logspace(-3, log10(3), 1000);  
R1 = 0.1 * XL1;  % R1 é 10% de XL1
XC = 0.01017;   % Reatância capacitiva

% Cálculo de K
K = (R1 + 1j*XL1) ./ (-1j*XC + R1 + 1j*XL1);

% Cálculo da magnitude e fase de K
K_mag = abs(K);           % Magnitude
K_phase = angle(K) * 180/pi;  % Fase em graus

% Plot da magnitude
figure;
subplot(2,1,1);
semilogx(XL1, K_mag, 'b', 'LineWidth', 2);  % Escala log no eixo X
xlabel('X''_{L1} (p.u.)');
ylabel('|V1/V2|');
title('Magnitude de V1/V2');
grid on;

% Plot da fase
subplot(2,1,2);
semilogx(XL1, K_phase, 'r', 'LineWidth', 2);  % Escala log no eixo X
xlabel('X''_{L1}  (p.u.)');
ylabel('\angle V1/V2 (graus)');
title('Fase de V1/V2');
grid on;

