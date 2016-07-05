graphics_toolkit('gnuplot');
data_in=dlmread('current.txt');
data_in=data_in(2:end-1,:);
figure('visible','off');
plot(data_in(:,9),data_in(:,1));
print -djpg currentgraph.jpg
