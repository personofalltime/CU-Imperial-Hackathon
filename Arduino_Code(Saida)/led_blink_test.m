a = arduino("COM8","Nano3");

while true
    % Red
    writePWMDutyCycle(a,"D9",0.0);
    writePWMDutyCycle(a,"D10",1.0);
    writePWMDutyCycle(a,"D11",1.0);
    pause(1);

    % Green
    writePWMDutyCycle(a,"D9",1.0);
    writePWMDutyCycle(a,"D10",0.0);
    writePWMDutyCycle(a,"D11",1.0);
    pause(1);

    % Blue
    writePWMDutyCycle(a,"D9",1.0);
    writePWMDutyCycle(a,"D10",1.0);
    writePWMDutyCycle(a,"D11",0.0);
    pause(1);
end