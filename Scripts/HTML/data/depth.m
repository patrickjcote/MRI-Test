graphics_toolkit('gnuplot');

#Parameters
atm_pressure = 633; #mmHg at 0 depth

f=dlmread('current.txt');
f=f(find(f(:,9)),:);
f(:,9)=((f(:,9)-atm_pressure)/.735559121015)/30.48; #mmHg to ft

figure('visible','off');
plot(f(:,1),f(:,9));
axis('ij');
xlabel('Time [HHMMSS]');
ylabel('Depth [ft]');
print -djpg "-S640,480" currentdepth.jpg
