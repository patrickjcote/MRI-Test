graphics_toolkit('gnuplot');
f=dlmread('current.txt');
f=f(find(f(:,9)),:)
figure('visible','off');
plot(f(:,9),f(:,1));
print("currentgraph.jpg","-djpg")
