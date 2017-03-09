%% Initialize
clear
clc
format compact

%% Input

feet = 173.6059;
turnsPerWrap = 46;
wrapLength = [158.0614  143.0079  127.9545  112.9010  97.8475  82.7941];
totalLengthPerWrap = [158.0614  301.0693  429.0238  541.9248  639.7723  722.5664];
lengthPerClick = [3.4361  3.1089  2.7816  2.4544  2.1271  1.7999];

%% Calcs

for n = 1:6
    if(feet < totalLengthPerWrap(n))
        wrap = n;
        break;
    end
end

clicks = (wrap-1)*turnsPerWrap;
remaining = feet;
for n = 1:(wrap-1)
    remaining = remaining - totalLengthPerWrap(n);
end

clicks = clicks + round(remaining/lengthPerClick(wrap))


        