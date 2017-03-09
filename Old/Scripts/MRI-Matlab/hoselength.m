%% Initialize
clear
clc
format compact

%% Input
hoseOD = .625; %inches
drumOD = 6.25; %inches
discOD = 16.5; %inches
drumWidth = 29; %inches
clearance = .5; %inches

%% Calculations
discRadius = discOD/2;
drumRadius = drumOD/2;
workingRadius = discRadius - drumRadius - clearance;

totalWraps = floor(workingRadius/hoseOD);
turns = floor(drumWidth/hoseOD);

length = 0;

for wrapNmbr = 1:totalWraps
    wrapRadius = drumRadius+(hoseOD*wrapNmbr)-(hoseOD/2);
    length = length + (2*pi*wrapRadius)*turns;
    wrapLength(wrapNmbr) = (2*pi*wrapRadius*turns)/12;
    totalLength(wrapNmbr) = length/12;
    lengthPerClick(wrapNmbr) = (2*pi*wrapRadius)/12;
    
end

%% Output
length_ft = length/12
wrapLength
totalLength
lengthPerClick
