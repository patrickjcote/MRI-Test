graphics_toolkit('gnuplot');

#Parameters

f=dlmread('current.txt');
f=f(find(f(:,8)),:);

figure('visible','off');
plot(f(:,1),f(:,8));
axis('ij');
xlabel('Time [HHMMSS]');
ylabel('Depth [ft]');
print -djpg "-S640,480" currentdepth.jpg
