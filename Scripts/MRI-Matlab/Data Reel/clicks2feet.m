%% Initialize
clear
clc
format compact

%% Input

clicks = 19;
turnsPerWrap = 45;
wrapLength = [158.0614  143.0079  127.9545  112.9010  97.8475  82.7941];
lengthPerClick = [3.4361  3.1089  2.7816  2.4544  2.1271  1.7999];

%% Calcs

    totalFeet = 0;
    currentWrap = floor(clicks/turnsPerWrap)+1;
    for i = 1:(currentWrap-1)
        totalFeet = totalFeet + wrapLength(i);
    end
    partialLength = mod(clicks,((currentWrap-1)*turnsPerWrap));
    totalFeet = totalFeet + partialLength*lengthPerClick(currentWrap)


