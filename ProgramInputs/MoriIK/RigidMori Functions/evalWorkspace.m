function [results] = evalWorkspace(rbtree, workspace, startConfig,  numSamples, effName, maxIter, mode, inputSamples)
%EVALWORKSPACE Evaluates the performance of a configuration in a workspace
%   Determines whether the algorithm converges to a random target in a
%   spherical workspace, saves the residual cost and the final pose.
% INPUTS:
%   rbtree: rigid body tree
%   workspace: structure with fields 'center' and 'r' describing the
%   spherical workspace
%   startConfig: starting hinge angle configuration
%   numSamples: number of random samples to generate
%   effName: name of the body corresponding to the end effector
%   maxIter: Maximum number of iterations for ccdIK algorithm
%   mode: 1-> position only, 2-> position and orientation
%   inputSamples (optional): desired targets as a 3x2xNumsamples matrix
% OUTPUTS:
%   results.residuals: residual cost for each target sample, 0 if the algorithm
%   converges within maxIter
%   results.errors: the residual position and orientaiton error in m and
%   degrees respectively
%   results.samples: store the utilized targets as a 3x2xNumsamples where the first
%   column vector is the target orientation and the second column vector is
%   the target position
%   results.poses: struct which stores the final config and edgeLengthMatrix for
%   each sample
%   results.stalled: estimates at which iteration the CCD algorithm stalled
%   results.params: parameters used for CCD algorithm

% Seed the random number generator for consistency between runs
%rng(42)

Bodies = rbtree.Bodies;
numModules = rbtree.NumBodies;

center = workspace.center;

%Initialize output variables
residuals = Inf*ones(numSamples, 1);
configs = zeros(numSamples, numModules);
edgeExtensionMatrices = zeros(numModules, 3, numSamples);
errors = Inf*ones(numSamples, 2);
stalled = Inf*ones(numSamples, 1);

% Store a copy of the original edge extension values
originalEdgeLengthMatrix = zeros(numModules, 3);
for lv1 = 1:numModules %store EdgeLengths in matrix
    originalEdgeLengthMatrix(lv1,:) = Bodies{lv1}.EdgeLengths;
end

if ~exist('inputSamples', 'var') %If we didn't pass the optional inputSamples argument
        %Based on workspace description, generate random targets within spherical workspace
        [x, y, z] = sph2cart(2*pi*rand(1, numSamples),asin(1-2*rand(1, numSamples)), ones(1, numSamples));
        orientation = [x;y;z];
        [x, y, z] = sph2cart(2*pi*rand(1, numSamples),asin(1-2*rand(1, numSamples)), workspace.r*rand(1, numSamples).^(1/3));
        offset = [x;y;z];
        samples = [reshape(orientation, 3, 1, []), reshape(offset+center, 3, 1, [])];
else
    assert(all(size(inputSamples) == [3,2,numSamples]),'inputSamples has incorrect dimensions')
    samples = inputSamples;
end

p = parallel.pool.PollableDataQueue;

parfor lv = 1:numSamples
    target = eye(4);
    target(1:3,3:4) = samples(:,:,lv);

    %Run ccdIK, determine whether algorithm converges, if not store residual
    %cost
    [trajectory, params] = ccdIK(rbtree, target, 'startConfig', startConfig, 'maxIterations',...
        maxIter, 'mode', mode, 'effName', effName, 'maxStepSize', [deg2rad(1), 5E-5]);
    configs(lv,:) = trajectory.configs(end,:);
    if p.QueueLength == 0
        send(p, params)
    end
    
    edgeExtensionMatrices(:,:,lv) = trajectory.edgeLengthMatrix(:,:,end);
    % Store errors
    errors(lv,:) = trajectory.errors(end,:);
    % Store stalled value
    stalled(lv) = trajectory.stalled;
    if ~trajectory.converged %If the algorithm didn't converge within maxIter
        % Store residual cost
        residuals(lv) = trajectory.costs(end);
    else
        % Set residual cost to 0
        residuals(lv) = 0;
    end
    % Reset edge extensions to original value
    actuateAllEdges(rbtree, originalEdgeLengthMatrix);
end

%Add all fields to struct
params = poll(p); %Retrieve params from pollable queue
poses = struct('configs', configs, 'edgeExtensionMatrices', edgeExtensionMatrices);
results = struct('residuals', residuals, 'samples', samples, 'poses', poses, 'errors', errors, 'stalled', stalled, 'params', params);

end

