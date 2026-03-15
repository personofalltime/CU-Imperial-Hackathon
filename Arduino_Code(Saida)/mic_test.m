a = arduino("COM8","Nano3");

figure
h = animatedline;
xlabel("Time (s)")
ylabel("Volume over limit?")
ylim([-0.1 1.1])

startTime = tic;

while true
    value = readDigitalPin(a,"D4");
    t = toc(startTime);

    addpoints(h,t,value)
    drawnow limitrate

    pause(0.01)
end