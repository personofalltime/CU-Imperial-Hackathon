clc, clear vars;

acc_data = readmatrix("NoiseEmpty.csv");
acc_data = acc_data';
acc_data = arrayfun(@(x) abs(x), acc_data);

dt = 1;

length = size(acc_data);
val = length(2);

times = linspace(0, val-1, val);

% very busy = 0, busy = 1, active = 2, light = 3, empty = 0







