%% Battery_Analysis.m
% Nipun Gunawardena
% Estimate battery life for BTEMS
% TODO: Add sleepTime/wakeTime low and high?


clear all, close all, clc


%% Initialize variables
numBatsSeries = 4;      % Natural number
numBatsParallel = 2;    % Natural number, don't use 0
voltsPerBat = 1.5;      % V
batCapacity = 2500;     % mAh
sleepLow = 2.02;        % mA - Lowest reading measured while sleeping
sleepHigh = 4.40;       % mA - Highest reading measured while sleeping
wakeLow = 12;           % mA - Lowest reading measured while awake
wakeHigh = 40;          % mA - Highest reading measured while awake


%% Vary sleeptime to see effect on battery life
wakeTime = 0.5/3600;       % Convert seconds to hours
sleepTime = (5:0.5:60)/3600;
totCapacity = numBatsParallel*batCapacity;
avgCurrentLow = (1./(sleepTime + wakeTime)) .* (sleepLow*sleepTime + wakeLow*wakeTime);
avgCurrentHigh = (1./(sleepTime + wakeTime) .* (sleepHigh*sleepTime + wakeHigh*wakeTime));
highEstimate = totCapacity./avgCurrentLow;
lowEstimate = totCapacity./avgCurrentHigh;


%% Plot
wakeTime = wakeTime * 3600;     % Get back to seconds
sleepTime = sleepTime * 3600;

figure()
hold all
plot(sleepTime, lowEstimate/24);
plot(sleepTime, highEstimate/24);
title(sprintf('Battery Life Estimates |::| %d mAh total capacity with %f second waketime', totCapacity, wakeTime));
xlabel('Sleep Time (Seconds)');
ylabel('Battery Life (Days)');
legend('Low Estimate','High Estimate');
grid on