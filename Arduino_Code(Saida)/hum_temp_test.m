s = serialport("COM8",9600);

figure
hTemp = animatedline;
hHum = animatedline;

xlabel("Time (s)")
ylabel("Value")
legend("Temperature","Humidity")
ylim([0 100])

startTime = tic;

while true
    line = readline(s);
    vals = split(strtrim(line), ",");

    if numel(vals) == 2
        temp = str2double(vals{1});
        hum  = str2double(vals{2});
        t = toc(startTime);

        addpoints(hTemp, t, temp)
        addpoints(hHum, t, hum)
        drawnow

        fprintf("Temp: %.2f C | Humidity: %.2f %%\n", temp, hum);
    end
end
