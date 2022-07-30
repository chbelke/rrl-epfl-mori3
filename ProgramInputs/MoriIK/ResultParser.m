function ResultParser(results_a, results_b, mode, results_a_orig, results_b_orig)
N = length(results_a.residuals);
if mode == 2
    %Targets that didn't converge with edgeExtension but did without
    indices = 1:N;
    idx1 = (results_b.residuals == 0) & ~(results_a.residuals == 0);
    fprintf('%d targets did not converge with edge extension but did without\n', sum(idx1))
    targets1 = indices(idx1);
end

if (mode == 2) && (nargin >= 5) 
    %% Compare residual costs between two runs
    idx2 = (results_a.residuals == 0 & results_a_orig.residuals == 0);
    figure();
    bar(categorical(indices(~idx2)), [results_a.residuals(~idx2), results_a_orig.residuals(~idx2)])
    title('Residual Costs With Edge Extension')
    legend('New trial', 'Baseline')
    set(gca, 'yscale', 'log')
    xlabel('Target #')
    ylabel('Residual Cost')
    fprintf('%d targets out of %d converged with edge extension\n', sum(results_a.residuals == 0), N)

    idx3 = (results_b.residuals == 0 & results_b_orig.residuals == 0);
    figure()
    bar(categorical(indices(~idx3)), [results_b.residuals(~idx3), results_b_orig.residuals(~idx3)])
    title('Residual Costs Without Edge Extension')
    legend('New trial', 'Baseline')
    set(gca, 'yscale', 'log')
    xlabel('Target #')
    ylabel('Residual Cost')
    fprintf('%d targets out of %d converged without edge extension\n', sum(results_b.residuals == 0), N)
else
    fprintf('%d targets out of %d converged for case A\n', sum(results_a.residuals == 0), N)
    fprintf('%d targets out of %d converged for case B\n', sum(results_b.residuals == 0), N)
end

%% Data visualization
samples = results_a.samples;
x = squeeze(samples(1,2,:));
y = squeeze(samples(2,2,:));
z = squeeze(samples(3,2,:));

posErr_a = results_a.errors(:,1);
posErr_b = results_b.errors(:,1);


u = squeeze(samples(1,1,:));
v = squeeze(samples(2,1,:));
w = squeeze(samples(3,1,:));
phi = atan2(v,u);
theta = acos(w);

angErr_a = results_a.errors(:,2);
angErr_b = results_b.errors(:,2);

figure()
labels = {'x' 'y' 'z' 'Position Error [mm]'};
data = [x y z posErr_b];
[h,ax] = plotmatrix(data);                        % create a 4 x 4 matrix of plots
for i = 1:4                                       % label the plots
xlabel(ax(4,i), labels{i})
ylabel(ax(i,1), labels{i})
end
title('Residual Position Error for Position Target')

hold(ax(4,1),'on')
plot(ax(4,1), x, posErr_a, '.', 'MarkerSize', 2)
hold(ax(4,2),'on')
plot(ax(4,2), y, posErr_a, '.', 'MarkerSize', 2)
hold(ax(4,3),'on')
plot(ax(4,3), z, posErr_a, '.', 'MarkerSize', 2)
if mode == 2
    legend(ax(4,3), 'No Edge Ext.', 'Edge Ext.')
else
    legend(ax(4,3),'Case B', 'Case A')
end

figure()
labels = {'x' 'y' 'z' 'Angular Error [deg]'};
data = [x y z angErr_b];
[h,ax] = plotmatrix(data);                        % create a 4 x 4 matrix of plots
for i = 1:4                                       % label the plots
xlabel(ax(4,i), labels{i})
ylabel(ax(i,1), labels{i})
end
title('Residual Angular Error for Position Target')

hold(ax(4,1),'on')
plot(ax(4,1), x, angErr_a, '.', 'MarkerSize', 2)
hold(ax(4,2),'on')
plot(ax(4,2), y, angErr_a, '.', 'MarkerSize', 2)
hold(ax(4,3),'on')
plot(ax(4,3), z, angErr_a, '.', 'MarkerSize', 2)
if mode == 2
    legend(ax(4,3),'No Edge Ext.','Edge Ext.')
else
    legend(ax(4,3),'Case B', 'Case A')
end

% figure()
% stem3(phi,theta,angErr_a)
% hold on
% stem3(phi,theta,angErr_b, 'x')
% xticks([-1*pi -1/2*pi 0 pi/2 pi])
% xticklabels({'-\pi','-1/2\pi','0','1/2\pi','\pi'})
% xlabel('\phi')
% yticks([ 0 pi/4 pi/2 3*pi/4 pi])
% yticklabels({'0', '\pi/4','1/2\pi','3\pi/4','\pi'})
% ylabel('\theta')
% axis vis3d
% zlabel('Angular Error [deg]')
% title('Angular Error vs. Target Orientation')
% 
% figure()
% stem3(phi,theta,posErr_a)
% hold on
% stem3(phi,theta,posErr_b, 'x')
% xticks([-1*pi -1/2*pi 0 pi/2 pi])
% xticklabels({'-\pi','-1/2\pi','0','1/2\pi','\pi'})
% xlabel('\phi')
% yticks([ 0 pi/4 pi/2 3*pi/4 pi])
% yticklabels({'0', '\pi/4','1/2\pi','3\pi/4','\pi'})
% ylabel('\theta')
% axis vis3d
% zlabel('Position Error [mm]')
% title('Positional Error vs. Target Orientation')

figure()
subplot(1,2,1)
scatter(phi,theta,100,posErr_a,'filled', 'MarkerFaceAlpha', 0.5)
colormap jet
xticks([-1*pi -1/2*pi 0 pi/2 pi])
xticklabels({'-\pi','-1/2\pi','0','1/2\pi','\pi'})
xlabel('\phi')
yticks([ 0 pi/4 pi/2 3*pi/4 pi])
yticklabels({'0', '\pi/4','1/2\pi','3\pi/4','\pi'})
ylabel('\theta')
colorbar
CLim = gca().CLim;
title('Positional Error vs. Target Orientation, Edge Ext.')

subplot(1,2,2)
scatter(phi,theta,100,posErr_b,'filled', 'MarkerFaceAlpha', 0.5)
colormap jet
xticks([-1*pi -1/2*pi 0 pi/2 pi])
xticklabels({'-\pi','-1/2\pi','0','1/2\pi','\pi'})
xlabel('\phi')
yticks([ 0 pi/4 pi/2 3*pi/4 pi])
yticklabels({'0', '\pi/4','1/2\pi','3\pi/4','\pi'})
ylabel('\theta')
colorbar
caxis(CLim)
title('Positional Error vs. Target Orientation, No Edge Ext.')

figure();
subplot(1,2,2);
%Find indices of samples that didn't converge
idx = results_b.residuals~=0;
data = [x(idx), posErr_b(idx)];
%WARNING The input data is not evenly distributed in x because it is
%uniformly distributed over the volume of the sphere.
[~, c] = hist3(data);
hist3(data,'Ctrs', c, 'CdataMode','auto')
xlabel('x')
ylabel('Positional Error [m]')
title('Frequency of Positional Error vs. x, No Edge Ext.')
view(2)
CLim = gca().CLim;
colorbar

subplot(1,2,1)
%Find indices of samples that didn't converge
idx = results_a.residuals~=0;
data = [x(idx), posErr_a(idx)];
%WARNING The input data is not evenly distributed in x because it is
%uniformly distributed over the volume of the sphere.
hist3(data,'Ctrs', c, 'CdataMode','auto')
xlabel('x')
ylabel('Positional Error [m]')
title('Frequency of Positional Error vs. x, Edge Ext.')
view(2)
colorbar
caxis(CLim)
