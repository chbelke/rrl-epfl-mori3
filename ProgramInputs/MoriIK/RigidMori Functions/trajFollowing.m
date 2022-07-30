function trajectory = trajFollowing(rbtree, targetTraj, startCfg, mode)
%TRAJFOLLOWING Follow a trajectory specified by 4x4xn endEffector targetTraj.

n = size(targetTraj, 3);

numMod = rbtree.NumBodies;
totalTraj = struct('configs', zeros(n, numMod), 'edgeLengthMatrix', zeros(numMod, 3, n), 'targetTraj', targetTraj, 'iterations', 0, 'costs', zeros(n, 2), 'errors', zeros(n, 1), 'converged', true);

for lv1 = 1:n
    partialTrajectory = ccdIK(rbtree, targetTraj(:,:,lv1), 'startConfig', startCfg, 'mode', mode, 'maxIterations', 400, 'getTorques', true);
    % Update starting config
    startCfg = partialTrajectory.configs(end,:);
 
    %Keep converged configurations
    totalTraj.configs(totalTraj.iterations+1,:) = partialTrajectory.configs(end,:);
    totalTraj.costs(totalTraj.iterations+1) = partialTrajectory.costs(end);
    totalTraj.errors(totalTraj.iterations+1,:) = partialTrajectory.costs(end,:);
    totalTraj.edgeLengthMatrix(:,:,totalTraj.iterations+1) = partialTrajectory.edgeLengthMatrix(:,:,end);
    totalTraj.jntTorques(totalTraj.iterations+1,:) = partialTrajectory.jntTorques(end,:);
    totalTraj.iterations = totalTraj.iterations + 1;
    
    if ~partialTrajectory.converged
        totalTraj.converged = false;
        break
    end
end

% Trim arrays
totalTraj.configs = totalTraj.configs(1:totalTraj.iterations,:);
totalTraj.costs = totalTraj.costs(1:totalTraj.iterations);
totalTraj.errors = totalTraj.errors(1:totalTraj.iterations,:);
totalTraj.edgeLengthMatrix = totalTraj.edgeLengthMatrix(:,:,1:totalTraj.iterations);
totalTraj.jntTorques = totalTraj.jntTorques(1:totalTraj.iterations,:);

% Iterations is 1 less than the number of poses
totalTraj.iterations = totalTraj.iterations - 1;

trajectory = totalTraj;

% %%Plots
% figure()
% plot(trajectory.configs,'x-')
% xlabel('Waypoint #')
% ylabel('Hinge Fold Angle [rad]')
% title('Folding Hinge Trajectory')
% figure()
% plot(squeeze(reshape(trajectory.edgeLengthMatrix,[],1,100)).','x-')
% xlabel('Waypoint #')
% ylabel('Edge Extension [m]')
% title('Edge Extension Trajectory')

%%Generate trajectories
% n = 100;
% 
% % M = [0 1 0 0; 0 0 1 0; 1 0 0 0; 0 0 0 1];
% % targTraj = repmat(M,1,1,n);
% % th = linspace(-pi/2, pi/2, n);
% % x = 0.4*cos(th)+0.2;
% % y = zeros(1,n);
% % z = 0.4*sin(th)+0.186/sqrt(3);
% % targTraj(1:3,4,:) = [x; y; z];
% 
% 
% M = [0 1 0 0; 0 0 1 0; 1 0 0 0; 0 0 0 1];
% targTraj = repmat(M,1,1,n);
% th = linspace(0, 3*pi, n);
% x = 0.6*ones(1,n);
% y = 0*ones(1,n);
% z = 0.05*ones(1,n);
% u = zeros(1,n); 
% v = cos(th);
% w = sin(th);
% targTraj(1:3,4,:) = [x; y; z];
% targTraj(1:3,3,:) = [u;v;w];