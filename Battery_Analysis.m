%% Battery_Analysis.m
% Nipun Gunawardena
% Estimate battery life for BTEMS


clear all, close all, clc


%% Initialize variables
numBatsSeries = 4;      % Natural number
numBatsParallel = 1;    % Natural number, don't use 0
voltsPerBat = 1.5;      % V
batCapacity = 2500;     % mAh
sleepLow = 3.60;        % mA
sleepHigh = 3.80;       % mA
wakeLow = 12;           % mA
wakeHigh = 40;          % mA
sleepTime = 9.5;        % s
wakeTime = 0.5;         % s


%% Calculate battery lifespan
sleepTime = sleepTime/3600;     % Convert to hours
wakeTime = wakeTime/3600;       % Convert to hours
totCapacity = numBatsParallel*batCapacity;
avgCurrentLow = (1/(sleepTime + wakeTime)) * (sleepLow*sleepTime + wakeLow*wakeTime);
avgCurrentHigh = (1/(sleepTime + wakeTime) * (sleepHigh*sleepTime + wakeHigh*wakeTime));
highEstimate = totCapacity/avgCurrentLow;
lowEstimate = totCapacity/avgCurrentHigh;


%% Display
he = sprintf('The high life estimate is %f hours (%f days)', highEstimate, highEstimate/24);
le = sprintf('The low life estimate is %f hours (%f days)', lowEstimate, lowEstimate/24);
disp(he);
disp(le);