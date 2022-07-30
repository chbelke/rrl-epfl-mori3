function [trajectory, S] = ccdIK(rbtree, target, varargin) %#codegen
%CCDIK given an rbtree, start config, and target endeffector tfm, perform IK 
%using ccd method. 
%   Inputs: Max step size, samples per step, max num iterations, convergence tolerance, flag to
%   animate. Output: returns the trajectory composed of a matrix of configs, edge
%   extensions for each step, number of iterations and final error.

    numModules = rbtree.NumBodies;
    Bodies = rbtree.Bodies;

    defaultStartConfig = homeConfiguration(rbtree);
    %FIXME IK starts with current edge extensions
    defaultEffName = rbtree.BodyNames{rbtree.NumBodies}; %Take the last body
    defaultMode = 2;
    defaultMaxStepSize = [deg2rad(0.25), 0.0125E-3]; %angle in rad, length in m
    defaultSamplesPerStep = 20;
    defaultMaxIterations = 200; %FIXME
    defaultCnvrgTol = 4E-6;
    defaultAnimate = false;
    defaultGetTorques = false;
    defaultOptimDir = 'leaf2root';
    expectedOptimDir = {'leaf2root', 'root2leaf'};

    p = inputParser;
    validScalarPosNum = @(x) isnumeric(x) && isscalar(x) && (x > 0);
    validPosNum = @(x) isnumeric(x) && all(x > 0);
    addOptional(p, 'startConfig', defaultStartConfig, @isnumeric);
    addParameter(p, 'effName', defaultEffName, @isstring);
    addParameter(p, 'mode', defaultMode, @isnumeric);
    addParameter(p, 'maxStepSize', defaultMaxStepSize, validPosNum);
    addParameter(p, 'samplesPerStep', defaultSamplesPerStep, validScalarPosNum);
    addParameter(p, 'maxIterations', defaultMaxIterations, validScalarPosNum);
    addParameter(p, 'cnvrgTol', defaultCnvrgTol, validScalarPosNum);
    addParameter(p, 'animate', defaultAnimate, @islogical);
    addParameter(p, 'getTorques', defaultGetTorques, @islogical);
    addParameter(p, 'optimDir', defaultOptimDir,...
                @(x) any(validatestring(x,expectedOptimDir)));
    parse(p,varargin{:});

    S = p.Results;
    effName = S.effName; %to avoid broadcast variable for parallelization
    mode = S.mode;
    
    maxHingeAngle = deg2rad(180-69);
    minHingeAngle = -maxHingeAngle;
    maxEdgeExt = 0.192;
    minEdgeExt = 0.18;
    
    %TODO stall detection doesn't work, needs tuning.
    stallCounter = 0;
    stallTol = 1E-9; %1E-10;
    stallIter = 5;
    
    if false %numModules > 3 %FIXME only works for configurations other than the straight line
        colThr = deg2rad(360/numModules); %don't check for collisions if all hinge angles below this value
    else
        colThr = 0;
    end
    
    %rng(0); %initalize with seed=0
    
    trajectory = struct('configs', zeros(S.maxIterations+1, numModules),...
                        'edgeLengthMatrix', zeros(numModules, 3, S.maxIterations+1), 'iterations', 0,...
                        'costs', Inf*ones(S.maxIterations,1), 'target', target, 'converged', false,...
                        'errors', Inf*ones(S.maxIterations,2), 'stalled', Inf, 'jntTorques', Inf*ones(S.maxIterations+1, numModules));
    trajectory.configs(1,:) = S.startConfig; %trajectory.configs(n,:) is the confguration at the end of the n-1th iteration
    if S.getTorques
     trajectory.jntTorques(1,:) = inverseDynamics(rbtree,S.startConfig); 
    end
    for lv0 = 1:numModules
        trajectory.edgeLengthMatrix(lv0,:,1) = Bodies{lv0}.EdgeLengths;
    end
    
    tic;
    for iter = 1:S.maxIterations
        trajectory.configs(iter+1,:) = trajectory.configs(iter,:);
        currentModule = rbtree.getBody(S.effName); %Start at end effector TODO add optimDir functionality
        for lv1 = 1:numModules  % iterate accross modules
            currentModuleName = currentModule.Name;
            if true %for lv2 = 1:3
                %iterate over 3 independent dof of a module: 1 joint plus two edge
                %extensions (edges 2&3)
                moriIdx = find(contains(rbtree.BodyNames, currentModule.Name), 1); %Determine what index the current module corresponds to
                %generate random samples for line search   
                hingeSampleConfigs = repmat(trajectory.configs(iter+1,:), S.samplesPerStep, 1); %Initialize all sample configs to previous iteration value 
                hingeSampleConfigs(:, moriIdx) = genRndSamples(S.maxStepSize(1), trajectory.configs(iter+1, moriIdx), maxHingeAngle, minHingeAngle, S.samplesPerStep);
                
                costs = Inf*ones(1,S.samplesPerStep); %Initialize costs of each sample to Inf
                
                for lv3 = 1:S.samplesPerStep % can be a parfor
                    costs(lv3) = computeCost(rbtree, hingeSampleConfigs(lv3,:), effName, target, mode);
                end
                %find the feasible config with min cost
                [~,I] = sort(costs);
                for idx = I
                    if all(abs(trajectory.configs(iter+1,:))<colThr)
                        trajectory.configs(iter+1,:) = hingeSampleConfigs(idx,:); %only update if there is no collision
                        break
                    elseif ~checkCollision(rbtree, hingeSampleConfigs(idx,:))
                        trajectory.configs(iter+1,:) = hingeSampleConfigs(idx,:); %only update if there is no collision
                        break
                    end
                end
                
                if S.mode ~= 3
                    %line search over edge 2 extension
                    tmpEdgeExtensions = Bodies{moriIdx}.EdgeLengths;
                    edge2SampleConfigs = repmat(tmpEdgeExtensions, S.samplesPerStep, 1);
                    edge2SampleConfigs(:, 2) = genRndSamples(S.maxStepSize(2), tmpEdgeExtensions(2), maxEdgeExt, minEdgeExt, S.samplesPerStep);
                    startConfig = trajectory.configs(iter+1,:);
                    for lv3 = 1:S.samplesPerStep % can be a parfor
                        %compute cost
                        actuateEdges(currentModuleName, edge2SampleConfigs(lv3,:), rbtree);
                        costs(lv3) = computeCost(rbtree, startConfig, effName, target, mode);
                    end
                    %find the feasible edge extension with min cost
                    [~,I] = sort(costs);
                    for idx = I
                        actuateEdges(currentModule.Name, edge2SampleConfigs(idx,:), rbtree);
                        updateCollisionMeshes(Bodies,numModules)
                        if all(abs(trajectory.configs(iter+1,:))<colThr)
                            tmpEdgeExtensions = edge2SampleConfigs(idx,:); %only update if there is no collision
                            break
                        elseif ~checkCollision(rbtree, trajectory.configs(iter+1,:))
                            tmpEdgeExtensions = edge2SampleConfigs(idx,:); %only update if there is no collision
                            break
                        end
                    end

                    %line search over edge 3 extension
                    edge3SampleConfigs = repmat(tmpEdgeExtensions, S.samplesPerStep, 1);
                    edge3SampleConfigs(:, 3) = genRndSamples(S.maxStepSize(2), tmpEdgeExtensions(3), maxEdgeExt, minEdgeExt, S.samplesPerStep);
                    startConfig = trajectory.configs(iter+1,:);
                    for lv3 = 1:S.samplesPerStep % can be a parfor
                        %compute cost
                        actuateEdges(currentModuleName, edge3SampleConfigs(lv3,:), rbtree);
                        costs(lv3) = computeCost(rbtree, startConfig, effName, target, mode);
                    end
                    %find the feasible edge extension with min cost
                    [~,I] = sort(costs);
                    for idx = I
                        actuateEdges(currentModule.Name, edge3SampleConfigs(idx,:), rbtree);
                        updateCollisionMeshes(Bodies,numModules)
                        if all(abs(trajectory.configs(iter+1,:))<colThr)
                            tmpEdgeExtensions = edge3SampleConfigs(idx,:); %only update if there is no collision
                            break
                        elseif ~checkCollision(rbtree, trajectory.configs(iter+1,:))
                            tmpEdgeExtensions = edge3SampleConfigs(idx,:); %only update if there is no collision
                            break
                        end
                    end
                    actuateEdges(currentModule.Name, tmpEdgeExtensions, rbtree);
                end
            end
            currentModule = currentModule.Parent; %Traverse tree %TODO update when implementing root2leaf
        end
        
        for lv4 = 1:numModules %store EdgeLengths in matrix
            trajectory.edgeLengthMatrix(lv4,:,iter+1) = Bodies{lv4}.EdgeLengths;
        end
        
        if S.getTorques
            trajectory.jntTorques(iter+1,:) = inverseDynamics(rbtree,trajectory.configs(iter+1,:)); 
        end
        
        [trajectory.costs(iter+1), trajectory.errors(iter+1,:)] = computeCost(rbtree, trajectory.configs(iter+1,:), S.effName, target, S.mode); %FIXME doesn't handle case where all guesses are infeasible correctly
        if (trajectory.costs(iter)-trajectory.costs(iter+1)) < stallTol
            stallCounter = stallCounter+1;
            if stallCounter > stallIter
                trajectory.stalled = iter;
                stallTol = 0;%break;
            end
        else
            stallCounter = 0;
        end
        
        if trajectory.costs(iter+1) < S.cnvrgTol
            trajectory.converged = true;
            break;
        end
    end
    T = toc;
    toc
    
    trajectory.iterations = iter;
    trajectory.configs =  trajectory.configs(1:iter+1, :); %trim zero values
    trajectory.costs = trajectory.costs(1:iter+1);
    trajectory.errors = trajectory.errors(1:iter+1,:);
    trajectory.edgeLengthMatrix = trajectory.edgeLengthMatrix(:,:,1:iter+1);
    trajectory.jntTorques = trajectory.jntTorques(1:iter+1,:);
    trajectory.compTime = T;
end