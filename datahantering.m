%%
cd ~/Dropbox/Dokument/Kandidat/Optical/

%% I/O load

file = 'accel2';
A = importdata(file,'\t',1);

acc_x = A.data(:,2);
acc_y = A.data(:,3);
acc_z = A.data(:,4);

%% Measurement intervals

m1 = 8600:11400;
m2 = 15000:17800;
m3 = 25600:28200;
m4 = 31800:34600;

%% Linear Acceleration
majs = m2;
time_stepsize = 0.01; % sec
t0 = 1:length(majs);
t0 = t0*time_stepsize;

ds_x = acc_x(majs).*time_stepsize^2/2;% + acc_x(majs-1).*time_stepsize;
ds_y = acc_y(majs).*time_stepsize^2/2;% + acc_y(majs-1).*time_stepsize;
ds_z = acc_z(majs).*time_stepsize^2/2;% + acc_z(majs-1).*time_stepsize;
dv_x = zeros(length(ds_x),1);
dv_y = dv_x;
dv_z = dv_x;
for i=2:length(ds_x) 
    dv_x(i) = dv_x(i-1) + acc_x(majs(i))*time_stepsize;   
    dv_y(i) = dv_y(i-1) + acc_y(majs(i))*time_stepsize;
    dv_z(i) = dv_z(i-1) + acc_z(majs(i))*time_stepsize;
end
s_x = zeros(length(ds_x),1);
s_y = s_x;
s_z = s_x;
for i=1:length(ds_x)
   s_x(i) = sum(ds_x(1:i))+dv_x(i)*time_stepsize;
   s_y(i) = sum(ds_y(1:i))+dv_y(i)*time_stepsize;
   s_z(i) = sum(ds_z(1:i))+dv_z(i)*time_stepsize;
end

clf;
hold on
plot3(s_x,s_y,s_z)
plot3(s_x(1),s_y(1),s_z(1),'*r')
plot3(s_x(end),s_y(end),s_z(end),'*m')
text(0.93*s_x(1),1.05*s_y(1),s_z(1),'Start')
text(0.93*s_x(end),1.05*s_y(end),s_z(end),'Slut')
xlabel('sträcka-x (m)')
ylabel('sträcka-y (m)')
zlabel('sträcka-z (m)')
hold off

%% 

clf;
hold on
plot(t0,s_x)
plot(t0,s_y)
plot(t0,s_z,'--')
xlabel('Mätpunkt')
ylabel('Sträcka (m)')
legend('s_x','s_y','s_z')

