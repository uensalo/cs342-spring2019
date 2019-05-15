clc
clear

M = 10000000;
N = 2.^(0:12);
read_count = ceil(M ./ N);


real = [34.265, 17.295, 9.738, 4.679, 2.330, 1.315, 0.642, 0.367, 0.447, 0.282, 0.196, 0.151, 0.130];
user = [5.558, 2.818, 1.643, 0.722, 0.509, 0.247, 0.188, 0.120, 0.146, 0.103, 0.093, 0.099, 0.103];
sys = [28.465, 14.354, 7.495, 3.774, 1.750, 0.946, 0.442, 0.235, 0.290, 0.169, 0.092, 0.042, 0.017];

x1 = figure;
hold on
fit = polyfit(log2(N), log2(real), 1);
est = polyval(fit, log2(N));
plot(log2(N), est);
plot(log2(N), log2(real), 'x');
title('Real Time Graph');
legend('best fit curve','real time plot');
xlabel('Log N');
ylabel('Log Real Time');
%saveas(x1, 'real.png');
hold off

x2 = figure;
hold on
fit = polyfit(log2(N), log2(user), 1);
est = polyval(fit, log2(N));
plot(log2(N), est);
plot(log2(N), log2(user), 'x');
title('User Time Graph');
legend('best fit curve','real time plot');
xlabel('Log N');
ylabel('Log User Time');
%saveas(x2, 'user.png');
hold off

x3 = figure;
hold on
fit = polyfit(log2(N), log2(sys), 1);
est = polyval(fit, log2(N));
plot(log2(N), est);
plot(log2(N), log2(sys), 'x');
title('System Time Graph');
legend('best fit curve','real time plot');
xlabel('Log N');
ylabel('Log System Time');
%saveas(x3, 'system.png');
hold off

x4 = figure;
hold on
fit = polyfit(read_count, real, 1);
est = polyval(fit, read_count);
plot(read_count, est);
plot(read_count, real, 'x');
title('Real Time vs Read Count');
legend('best fit line','real time plot');
xlabel('Read Count');
ylabel('Real Time');
%saveas(x4, 'realread.png');
hold off

x5 = figure;
hold on
fit = polyfit(read_count, user, 1);
est = polyval(fit, read_count);
plot(read_count, est);
plot(read_count, user, 'x');
title('User Time vs Read Count');
legend('best fit line','user time plot');
xlabel('Read Count');
ylabel('User Time');
%saveas(x5, 'userread.png');
hold off

x6 = figure;
hold on
fit = polyfit(read_count, sys, 1);
est = polyval(fit, read_count);
plot(read_count, est);
plot(read_count, sys, 'x');
title('System Time vs Read Count');
legend('best fit line','system time plot');
xlabel('Read Count');
ylabel('System Time');
%saveas(x6, 'systemread.png');
hold off


