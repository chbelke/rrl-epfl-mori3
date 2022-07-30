n = 5;
m = 10; %number of repeats
step = 5;

costsCell = cell(n,m);
compTime = zeros(n,m);
converged = true(n,m);
iterations = zeros(n,m);

figure()
hold on
colors = lines(n);

for lv1 = 1:n
    parfor lv2 = 1:m %parfor
        [trajectory, rbtree] = Benchmark(2, 400, step*lv1);
        costsCell{lv1, lv2} = trajectory.costs;
        compTime(lv1, lv2) = trajectory.compTime;
        iterations(lv1, lv2) = trajectory.iterations;
        converged(lv1,lv2) = trajectory.converged;
        semilogy(trajectory.costs, 'Color', colors(lv1, :));
        drawnow();
    end
end

% boxplot(compTime.')
% hold on
% plot(mean(compTime.'))
% xticklabels(step*(1:n))
% xlabel('Samples per step')
% ylabel('Computation time [s]')
% title('Computation Time vs Samples per Step')