%% BTEMS_Plotter.m
% Nipun Gunawardena

clear all, close all, clc


%% Load Data
[FileName, PathName, FilterIndex] = uigetfile('~/Dropbox/Research/BTEMS/Initial Data/*.csv', 'Select the Data File');
rawData = csvread(strcat(PathName, FileName),1,0);


%% Reassign Variables
% Year, Month, Day, Hour, Minute, Second, SHT_Amb_C, SHT_Hum_Pct, MLX_IR_C, MLX_Amb_C, Bat_Lvl_V
% 1   , 2    , 3  , 4   , 5     , 6     , 7        , 8          , 9       , 10       , 11

dates = datenum(rawData(:,1:6));
shtAmb = rawData(:, 7);
shtHum = rawData(:, 8);
mlxSur = rawData(:, 9);
mlxAmb = rawData(:, 10);
batLvl = rawData(:, 11);


%% Plot Ambient Temperature
figure()
hold on
plot(dates, shtAmb, dates, mlxAmb, dates, mlxSur);
xlabel('Date');
ylabel('Temperature (C)');
title('Temperature');
dynamicDateTicks();
% datetick('x');        % Uncomment and comment above if dynamicDateTicks() not installed
legend('SHT15', 'MLX90614 Ambient', 'MLX90614 Surface');


%% Plot Humidity
figure()
hold on
plot(dates, shtHum);
xlabel('Date');
ylabel('Humidity (% RH)');
title('Humidity');
dynamicDateTicks();
% datetick('x');        % Uncomment and comment line above if dynamicDateTicks() not installed
legend('SHT15');


%% Plot Battery Level
figure()
hold on
plot(dates, batLvl, '.');
xlabel('Date');
ylabel('Battery Level (V)');
title('Battery Level');
dynamicDateTicks();
% datetick('x');        % Uncomment and comment line above if dynamicDateTicks() not installed
legend('4x AA Batteries');


%% Temperature 1-to-1
p = polyfit(shtAmb, mlxAmb, 1);
figure()
hold on
plot(shtAmb, mlxAmb, 'o');
fplot(@(x) p(1)*x + p(2), [min(shtAmb) max(shtAmb)], 'r');
xlabel('SHT Ambient Temp (C)');
ylabel('MLX Ambient Temp (C)');
title('Ambient Temperature 1-to-1');
legend('Data', strcat(num2str(p(1)),'x + ',num2str(p(2))))

