clc, clear classes;
a = arduino('COM4', 'Nano3', 'Libraries', 'I2C');
% I2C Config
accelerometer = device(a, 'I2CAddress', 0x1C);
s = serialport('COM4', 115200);
% Configure pinout for arduino LEDs
configurePin(a, 'D9', 'pullup');
configurePin(a, 'D10', 'pullup');
configurePin(a, 'D11', 'pullup');
% Pinout for noise sensor
configurePin(a, 'D4', 'digitalInput');
disp("configured");
while true
    writeRegister(accelerometer, 0X0E, 0x0, 'uint8'); % set to active mode
    writeRegister(accelerometer, 0x2A, 0x19, 'uint8'); % set to +/- 2g mode, most precise
    % get accelerometer readings
    output = read(accelerometer, 6, 'uint8');
    outputX = uint16(bitshift(uint16(output(1)), 8)) + uint16(output(2));
    outputY = uint16(bitshift(uint16(output(3)), 8)) + uint16(output(4));
    outputZ = uint16(bitshift(uint16(output(5)), 8)) + uint16(output(6));    
    %processX
    if outputX > 0x7FFF
        sign = -1;
    else
        sign = 1;
    end
    integerVal = bitshift((bitand(outputX, 0x4000)), -14);
    accelerationX = bitand(outputX, 0x3FFF)/(0.4096);
    accelerationX = accelerationX* sign;
    accelerationX = accelerationX + integerVal;
    % Process Y Acceleration
    if outputY > 0x7FFF
        sign = -1;
    else
        sign = 1;
    end
    integerVal = bitshift(bitand(outputY, typecast(0x4000, 'uint16')), -14);
    accelerationY = (typecast(bitand(outputY,typecast(0x3FFF, 'uint16')), 'uint16')/(0.4096));
    accelerationY = accelerationY* sign;
    accelerationY = accelerationY + integerVal;
    % Process Z acceleration
    if outputZ > 0x7FFF
        sign = -1;
    else
        sign = 1;
    end
    integerVal = bitshift(bitand(outputZ, typecast(0x4000, 'uint16')), -14);
    accelerationZ = (typecast(bitand(outputZ, typecast(0x3FFF, 'uint16')), 'uint16')/(0.4096));
    accelerationZ = accelerationZ* sign;
    accelerationZ = typecast(accelerationZ + integerVal, 'uint16');
    out.ax = accelerationX;
    out.ay = accelerationY;
    out.az = accelerationZ;
    out.humidity = digitalRead(a, 'D4');
    out.temp = 0;
    out.humidity = 0;
    out.mic = 0;
    writeline(s, val);
    %main body of code to process sensor values
end




