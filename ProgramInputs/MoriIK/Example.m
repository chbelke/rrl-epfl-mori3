%Include path to RigidMori funcitons
addpath("RigidMori Functions\")

%Initialize an rbtree object
rbtree = RigidMoriTree('DataFormat', 'row');
%Specify gravity, required for computation of quasi-static hinge torques
rbtree.Gravity = [0 0 -9.81];
%Extract the name of base element
basename = rbtree.BaseName;

% Generate Mori structures
% %Straight line of 7 modules
spawnMori('b1', basename, 3, [0.186 0.186 0.186], rbtree);
spawnMori('b2', 'b1', 2, [0.186 0.186 0.186], rbtree);
spawnMori('b3', 'b2', 3, [0.186 0.186 0.186], rbtree);
spawnMori('b4', 'b3', 2, [0.186 0.186 0.186], rbtree);
spawnMori('b5', 'b4', 3, [0.186 0.186 0.186], rbtree);
spawnMori('b6', 'b5', 2, [0.186 0.186 0.186], rbtree);
spawnMori('b7', 'b6', 3, [0.186 0.186 0.186], rbtree);

%Chris's configuration
% spawnMori('b1', basename, 3, [0.186 0.186 0.186], rbtree);
% spawnMori('b2', 'b1', 2, [0.186 0.186 0.186], rbtree);
% spawnMori('b3', 'b2', 3, [0.186 0.186 0.186], rbtree);
% spawnMori('b4', 'b3', 3, [0.186 0.186 0.186], rbtree);
% spawnMori('b5', 'b4', 2, [0.186 0.186 0.186], rbtree);
% spawnMori('b6', 'b5', 3, [0.186 0.186 0.186], rbtree);
% spawnMori('b7', 'b6', 3, [0.186 0.186 0.186], rbtree);
% spawnMori('b8', 'b7', 2, [0.186 0.186 0.186], rbtree);
% spawnMori('b9', 'b8', 2, [0.186 0.186 0.186], rbtree);
% spawnMori('b10', 'b9', 2, [0.186 0.186 0.186], rbtree);

%Generate Alex's Hyper flexible arm config
% spawnMori('b1', basename, 3, [0.186 0.186 0.186], rbtree);
% spawnMori('b2', 'b1', 2, [0.186 0.186 0.186], rbtree);
% spawnMori('b3', 'b2', 3, [0.186 0.186 0.186], rbtree);
% spawnMori('b4', 'b3', 2, [0.186 0.186 0.186], rbtree);
% spawnMori('b5', 'b4', 2, [0.186 0.186 0.186], rbtree);
% spawnMori('b6', 'b5', 2, [0.186 0.186 0.186], rbtree);
% spawnMori('b7', 'b6', 3, [0.186 0.186 0.186], rbtree);
% spawnMori('b8', 'b7', 2, [0.186 0.186 0.186], rbtree);
% spawnMori('b9', 'b8', 3, [0.186 0.186 0.186], rbtree);
% spawnMori('b10', 'b9', 3, [0.186 0.186 0.186], rbtree);
% spawnMori('b11', 'b10', 3, [0.186 0.186 0.186], rbtree);
% spawnMori('b12', 'b11', 2, [0.186 0.186 0.186], rbtree);
% phi = acos(-1/3);
% startConfig = [0,0,phi,-(pi-phi),-(pi-phi),phi,0,-phi,(pi-phi),(pi-phi),-phi,0];

%Print details of Mori structure to command window
showdetails(rbtree);

% Show the intial Mori configuration
figure('Name', 'Initial Config')
show(rbtree,'Collisions','on','Visuals','off');
%% Actuate the Mori structure

% Store the default/home configuration
homeConfig = homeConfiguration(rbtree);

% Get the number of modules
numModules = rbtree.NumBodies;

% Get the name of the last body
EffName = rbtree.BodyNames{rbtree.NumBodies};

% Set edge extension on edge 2 of module 'b2' to 0.15m 
actuateEdges('b2', [0.186 0.15 0.186], rbtree);

% Actuate folding hinge of edge 1 on module 'b1'
config = homeConfig;
config(1) = deg2rad(30);

% Display the actuated Mori configuration
figure('Name', 'Actuated Config')
show(rbtree, config, 'Collisions','on','Visuals','off');

% Check for collisions between Mori modules
isSelfColliding = checkCollision(rbtree, config);

% Find base to end effector transformation
effTform = getEffTform(rbtree, EffName, config);

% Determine the static torque on each Mori folding hinge
tau = inverseDynamics(rbtree,config);

%Reset edge extension values to original length of 0.186m
edgeLengthMatrix = 0.186*ones(numModules, 3);
actuateAllEdges(rbtree, edgeLengthMatrix)

%% Test CCD algorithm
%Generate start config
startConfig = homeConfiguration(rbtree);

target = [1 0 0 0.4;...
          0 1 0 0.1;...
          0 0 1 0.2;...
          0 0 0 1];


defaultMode = 2;

% Run CCD algorithm
    % Mode 1: Position only
    % Mode 2: Position and orientation
    % Mode 3: Position and orientation without edge extension
[trajectory, params] = ccdIK(rbtree, target, 'startConfig', startConfig,...
    'maxIterations', 500, 'maxStepSize', [deg2rad(1), 5E-5], 'mode', 2, 'getTorques', true, 'samplesPerStep', 15);
%% Plot end result
config = trajectory.configs(end,:);
figure('Name', 'Final Config')
show(rbtree, config, 'Collisions','on','Visuals','off');
hold on
% Add the target to plot
quiver3(trajectory.target(1,4)*ones(3,1), trajectory.target(2,4)*ones(3,1), trajectory.target(3,4)*ones(3,1),...
        trajectory.target(1,1:3)', trajectory.target(2,1:3)',trajectory.target(3,1:3)', 0.2, 'r');
hold off

% Plot convergence properties
figure()
semilogy(trajectory.costs)
xlabel('Iterations')
ylabel('Residual Cost')
title('Cost vs. Iterations')

% Plot convergence properties
figure()
plot(trajectory.jntTorques)
xlabel('Iterations')
ylabel('Torque [Nm]')
title('Hinge Torque Evolution')

%% Animate the trajectory, results saved in Animations folder
% Ensure an Animations folder exists in the .git directory
animateIK(rbtree, trajectory);

%% Trajectory following
%Reset edge extension values to original length of 0.186m
actuateAllEdges(rbtree, edgeLengthMatrix)

% Generate target trajectory
n = 50;

M = [1 0 0 0; 0 1 0 0; 0 0 1 0; 0 0 0 1];
targetTraj = repmat(M,1,1,n);
x = 0.4*ones(1,n);
y = linspace(-0.1, 0.1, n);
z = 0.2*ones(1,n);
targetTraj(1:3,4,:) = [x; y; z];

% Run trajectory following algorithm
mode = 2;
trajectory = trajFollowing(rbtree, targetTraj, startConfig, mode);

%%Plots
figure()
plot(trajectory.configs,'x-')
xlabel('Waypoint #')
ylabel('Hinge Fold Angle [rad]')
title('Folding Hinge Trajectory')
figure()
plot(squeeze(reshape(trajectory.edgeLengthMatrix,[],1,n)).','x-')
xlabel('Waypoint #')
ylabel('Edge Extension [m]')
title('Edge Extension Trajectory')

%% Animate the trajectory, results saved in Animations folder 
animateIK(rbtree, trajectory);
