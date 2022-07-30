%Constants %Load constants from .m file
%global L
%L = 0.186;
clear all
close all

addpath("RigidMori Functions\")

rbtree = RigidMoriTree('DataFormat', 'row');
basename = rbtree.BaseName;

edge_conn_arr = [3,2,3,2];
spawnMori('b1', basename, edge_conn_arr(1), [0.192 0.186 0.186], rbtree);
spawnMori('b2', 'b1', edge_conn_arr(2), [0.186 0.186 0.186], rbtree);
spawnMori('b3', 'b2', edge_conn_arr(3), [0.186 0.186 0.186], rbtree);
spawnMori('b4', 'b3', edge_conn_arr(4), [0.186 0.186 0.186], rbtree);
effName = "b4";

showdetails(rbtree);
show(rbtree,'Collisions','on','Visuals','off');
%% Actuate the Mori structure hinges
%config = homeConfiguration(rbtree);
%config = deg2rad(30)*[0, ones(1,rbtree.NumBodies-1)];
% config = deg2rad([30 30 -30]);
% figure('Name', 'Target Config')
% show(rbtree, config, 'Collisions','on','Visuals','off');
% 
% isSelfColliding = checkCollision(rbtree, config);
% 
% effTform = getEffTform(rbtree, 'b3', config);
% startConfig = homeConfiguration(rbtree);
%% Generate random start config
%startConfig = deg2rad(60)*(2*rand(1,7)-1);\
startConfig = deg2rad([30, -60, 60, -60]);
startConfig = deg2rad([45, -115, 115, -115]);
startConfig = deg2rad([0, 0, 0, 0]);
figure('Name', 'Starting Config')
show(rbtree, startConfig, 'Collisions','on','Visuals','off');
%% Test CCD algorithm
if 0
    target = [0 0 0.2];
    target = ([4.157355251890671e+02;2.922073295595544e+02;7.756954558357418e+02]-[0;0;550])./1000;
    target = diag([0.5 0.5 0.7])*rand(3,1); %Random target
    workspace = struct('center', [0.45; -0.15; -0.2], 'r', 0.2);
    orientation = [0.3;0.3;0.1];
    orientation = [0; 1.0; 0];
    offset = [0.35;  0.0;  0.05671];

    workspace = struct('center', [0.45; -0.15; -0.2], 'r', 0.2);
%     samples = [reshape(orientation, 3, 1, []), reshape(offset+workspace.center, 3, 1, [])];
    
    samples = [reshape(orientation, 3, 1, []), reshape(offset, 3, 1, [])];

    % target = effTform;
    target = eye(4);
    target(1:3,3:4) = samples(:,:);
    disp(target);

    % effTform = getEffTform(rbtree, 'b7', zeros(1,7));
    % target = tform2trvec(effTform) + [0.02, 0.02 , 0];
    % 
    % trajectory = ccdIK(rbtree, target, 'maxIterations', 200);
    % 
    % 
    % trajectory = ccdIK(rbtree, target, 'startConfig', startConfig, 'maxIterations', 800);
    maxIter = 100;
    mode = 3;
    deg_step = deg2rad(1);
    deg_step = deg2rad(2);
    lin_step = 5E-5;
    lin_step = 5E-4;
    [trajectory, params] = ccdIK(rbtree, target, 'startConfig', startConfig, 'maxIterations',...
            maxIter, 'mode', mode, 'effName', effName, 'maxStepSize', [deg_step, lin_step]);
end
    
%% New CCD algorithm
% % workspace_2 = struct('center', [0.5; 0; 0.1074], 'r', 0.05);
% % startConfig_2 = homeConfiguration(rbtree);
% % numSamples_2 = 500;
% % maxIter_2 = 400;
% % [results_2a] = evalWorkspace(rbtree, workspace_2, startConfig_2,  numSamples_2, effName_2, maxIter_2, 2);

%% Run trajectories
up = 0.1;

dn = 0.35;
up = 0.075;
dn = 0.035;
dn = 0.015;

up = 0.05671;
dn = 0.0;

x_up = 0.278;

if 0
    workspace = struct('center', [0.45; -0.15; -0.2], 'r', 0.2);
    orientation = transpose([
                            [0, 1.0, 0] %straighten out
                            [0, 1.0, 0] %to the left
                            [0, 1.0, 0] %down
                            ]);
    offset = transpose([
              [x_up,   0.0, up]    %straighten out
              [0.25,   0.1,  up]     %to the left
              [0.25,  0.1,   dn]   %down
              ]);

    % orientation = repmat([0; 1.0; 0], 1, length(offset));

    samples = [reshape(orientation, 3, 1, []), reshape(offset, 3, 1, [])];

    % target = effTform;
    target_array = zeros(4,4,size(offset, 2));
    for i = 1:size(offset, 2)
        target = eye(4);
        target(1:3,3:4) = samples(:,:,i);
        target_array(:, :, i) = target;
    end


    maxIter = 100;
    mode = 2;
    deg_step = deg2rad(1);
    deg_step = deg2rad(2);
    lin_step = 5E-5;
    lin_step = 5E-4;
    maxStepSize = [deg_step, lin_step];
    trajectory = trajFollowingAll(rbtree, target_array, startConfig, mode, maxIter, effName, maxStepSize);
    % if totalTraj.converged == false
    %     disp("Did not reach target");
    %     return
    % end
end

%% Chris' trajectory
up = 0.05671;
% dn = 0.02;
dn = 0.025;

x_up = 0.278;
% x_up = 0.25;
x_out = 0.24;
x_out = 0.26;

if 1
    interpolate_one = 6;
    interpolate = 4;
    totalpositions = 3 + interpolate_one + 4 + interpolate + 4 + interpolate + 1;
    offset = zeros(3,totalpositions);
    orientation = zeros(3,totalpositions);
    % flat to first push
    offset(:,1:3) = transpose([
        [x_up,   0.0, up]   %straighten out
        [x_out,   0.1,  up]  %to the left
        [x_out,  0.1,   dn]  %down
        ]);
    % first to second push
    offset(:,(3+interpolate_one+1):(3+interpolate_one+1 + 3)) = transpose([
        [x_out,  -0.01,  dn]  %to the middle
        [x_up,  -0.01,  up]   %up
        [x_out,  -0.1,  up]  %to the right
        [x_out,  -0.1,  dn]  %down
        ]);
    % second to third push
    offset(:,(7+interpolate_one+interpolate+1):(7+interpolate_one+interpolate+1 + 3)) = transpose([
        [x_out,  -0.025,   dn] %to the middle
        [x_up,  -0.025,   up]  %up
        [0.165,  0.0,   up] %curl in
        [0.165,  0.0,   dn] %down
        ]);
    % third push
    offset(:,11+interpolate_one+2*interpolate+1) = transpose([
        [x_out,  0.0,    dn]
        ]);
    for j = 4:1:(4+interpolate_one-1)
        offset(:,j) = offset(:,j-1) + (offset(:,(4+interpolate_one))-offset(:,3))/(interpolate_one+1);
    end
    for k = (8+interpolate_one):1:(8+interpolate_one+interpolate-1)
        offset(:,k) = offset(:,k-1) + (offset(:,(8+interpolate_one+interpolate))-offset(:,(7+interpolate_one)))/(interpolate+1);
    end
    for l = (12+interpolate_one+interpolate):1:(12+interpolate_one+2*interpolate-1)
        offset(:,l) = offset(:,l-1) + (offset(:,(12+interpolate_one+2*interpolate))-offset(:,(11+interpolate_one+interpolate)))/(interpolate+1);
    end
    for m = 1:1:(9+interpolate_one+interpolate)
        orientation(:,m) =  transpose([
            [0, 1.0, 0]
            ]);
    end
    for n = (10+interpolate_one+interpolate):1:totalpositions
        orientation(:,n) =  transpose([
            [-1.0, 0, 0]
            ]);
    end
    
    offset = offset(:,2:end);
    orientation = orientation(:,2:end);

%     offset = offset(:,17:end-1);
%     orientation = orientation(:,17:end-1);    
    
    
    samples = [reshape(orientation, 3, 1, []), reshape(offset, 3, 1, [])];

    % target = effTform;
    target_array = zeros(4,4,size(offset, 2));
    for i = 1:size(offset, 2)
        target = eye(4);
        target(1:3,3:4) = samples(:,:,i);
        target_array(:, :, i) = target;
    end    
    
    maxIter = 50;
    mode = 2;
    deg_step = deg2rad(1);
%     deg_step = deg2rad(2);
    lin_step = 5E-5;
    lin_step = 5E-4;
    maxStepSize = [deg_step, lin_step];
    tmp = reshape(offset, 3, 1, []);
    tmp = transpose(offset);
%     trajectory = trajFollowingAll(rbtree, target_array, startConfig, mode, maxIter, effName, maxStepSize);    
end

figure()
plot3(tmp(:,1),tmp(:,2),tmp(:,3),'x-');

return

%% Save file
save(sprintf("Outputs/%s.mat", datestr(now,'yyyy-mm-dd_HH-MM')), 'trajectory');

%% Trajectory Plots
%%Plots
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

%% Plot end result
% config = trajectory.configs(end,:);
% show(rbtree, config, 'Collisions','on','Visuals','off');
% hold on
% quiver3(trajectory.target(1,4)*ones(3,1), trajectory.target(2,4)*ones(3,1), trajectory.target(3,4)*ones(3,1),...
%         trajectory.target(1,1:3)', trajectory.target(2,1:3)',trajectory.target(3,1:3)', 0.2, 'r');
% hold off
% 
% figure()
% semilogy(trajectory.costs)
% xlabel('Iterations')
% ylabel('Residual Cost')
% title('Error vs. Iterations')


%% Export CSVs and JSON

writematrix(trajectory.configs, "Outputs/angles.csv");
writematrix(trajectory.edgeLengthMatrix, "Outputs/edges.csv");



names = ["Alfred" "Bob" "Carl" "Darlene" "Elaine"];
names = ["Nancy" "Alfred" "Maurice" "Lucille" "Karen"];
edge_conn_arr_close =   [0,2,1,2,2];
edge_conn_arr_far =     [1,3,3,3,0];
% names = ["Alfred" "Bob" "Carl"];
max_spd = 15;
min_spd = 2;
% names = ["Alfred"]
saveJson(edge_conn_arr_far, edge_conn_arr_close, trajectory, names, max_spd,  min_spd, edge_conn_arr);


%% Test animation
animateIK(rbtree, trajectory);
