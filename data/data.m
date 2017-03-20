%%
x1 = [567, 567, 638, 637, 567];
y1 = [405, 478, 477, 405, 405];

x2 = [541, 523, 589, 607, 541];
y2 = [331, 393, 417, 351, 331];

x3 = [576, 577, 640, 638, 576];
y3 = [352, 418, 413, 349, 352];

x4 = [572, 574, 635, 634, 572];
y4 = [410, 474, 473, 409, 410];

x5 = [553, 582, 632, 603, 553];
y5 = [415, 480, 465, 404, 415];

figure(1)
hold on
plot(x1, y1)
plot(x2, y2)
plot(x3, y3)
plot(x4, y4)
plot(x5, y5)
legend('x1','x2','x3','x4','x5')

%%
a1 = [11, 14, 68, 65, 11];
b1 = [11, 73, 65, 2, 11];

a2 = [568, 560, 636, 633, 568];
b2 = [29, 99, 102, 32, 29];

a3 = [554, 561, 640, 632, 554];
b3 = [395, 475, 473, 392, 395];

a4 = [20, 10, 97, 107, 20];
b4 = [375, 461, 466, 380, 375];

figure(2)
clf;
hold on
plot(a1, b1, ':')
plot(a2, b2, '--')
plot(a3, b3, '-.')
plot(a4, b4)
legend('höger ner', 'höger upp', 'vänster upp', 'vänster ner')
hold off
xlim([0 640])
ylim([0 480])

%% final
x1 = [564, 564, 632, 633, 564];
y1 = [371, 467, 461, 372, 371];

x2 = [550, 554, 636, 631, 550];
y2 = [0, 77, 76, 0, 0];

x3 = [1, 21, 122, 109];%, 1];
y3 = [37, 134, 133, 37];%, 37];

x4 = [28, 30, 153, 156, 28];
y4 = [333, 448, 437, 328, 333];

y1 = abs(y1 - 480);
y2 = abs(y2 - 480);
y4 = abs(y4 - 480);
y3 = abs(y3 - 480);

figure(3)
clf;
hold on
plot(x1,y1,':')
plot(x2,y2,'--')
plot(x3,y3,'-.')
plot(x4,y4)
xlim([0 640])
ylim([0 480])
text(x1+1, y1, num2str((1:numel(y1))'))
text(x2+1, y2, num2str((1:numel(y2))'))
text(x3+1, y3, num2str((1:numel(y3))'))
text(x4+1, y4, num2str((1:numel(y4))'))
title('View in the cameras perspective')
legend('left-down', 'left-up', 'right-up', 'right-down')
hold off

%%
dx = [x3(2)-x3(1), x3(3)-x3(2), x3(4)-x3(3), x3(1)-x3(4)];
dy = [y3(2)-y3(1), y3(3)-y3(2), y3(4)-y3(3), y3(1)-y3(4)];

xh = x3 + dx/2;
yh = y3 + dy/2;

figure(3)
hold on
plot(xh, yh, 'vr')
text(xh+10, yh, num2str((1:numel(yh))'))
hold off

%% 
xvekt = [xh(3)-xh(1), xh(2)-xh(4), 0];
yvekt = [yh(3)-yh(1), yh(2)-yh(4), 0];

%% 
x = [54, 42, 159, 166, 54];
y = [141, 248, 250, 146, 141];

clf;
plot(x,y)
xlim([0 640])
ylim([0 480])

text(x+1, y, num2str((1:numel(y))'))
polyarea(x,y)

