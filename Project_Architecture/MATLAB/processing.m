clc, clear vars;

noise_data = readmatrix("noise.csv");
acc_data = readmatrix("acc.csv");

dt = 1;

length = size(noise_data);
val = length(2);

times = linspace(0, val-1, val);

timeranges = [0, 3, 6, 9, 12];
div = 3;

acc_output = zeros(val, div);

% very busy = 0, busy = 1, active = 2, light = 3, empty = 0
count = 2;
timestep = 0;
for index = 1:val
    disp(timestep);
    timestep = timestep + dt;
    if(timestep * dt >= timeranges(count))
        count = count +1;
        disp(times(((count-2)/dt):((count)/dt)));
        disp(acc_output)
        acc_output(count-2) = times(((count-1)/dt):((count)/dt));
        acc_tmp = acc_data(((count-2)/dt):((count-1)/dt));
        disp(acc_output);
    end
end   





