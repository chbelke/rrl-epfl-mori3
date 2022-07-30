function [results] = evalSensitivity(rbtree, config,  numSamples, effName, mode)
%EVALSENSITIVITY generates a cloud composed of numSamples of endEffector
%positions for random joint variations
% INPUTS:
%   rbtree: rigid body tree
%   config: nominal hinge angle configuration
%   numSamples: number of random samples to generate
%   effName: name of the body corresponding to the end effector
%   mode: 1-> position only, 2-> position and orientation
% OUTPUTS:
%   results.costs: The cost for each sample
%   results.errors: The positional and angular error for each sample
%   results.endEffLocations: end effector positions for each sample


% Seed the random number generator for consistency between runs
%rng(42)

% 14mm potentiometer, ADC precision 4096, theoretical resolution of 0.0035mm
% linearity of sensor is +/-0.5% -> 0.07mm FS
edgePerturb = 7E-5; %in m

% Precision of AS5048B sensor, worst case nonlinearity +/- 1.2deg if the magnet is 500um offset 
hingePerturb = deg2rad(1.2);

Bodies = rbtree.Bodies;
numModules = rbtree.NumBodies;

% Assume end effector is at the target position
target = getEffTform(rbtree, effName, config);

% Store a copy of the original edge extension values
originalEdgeLengthMatrix = zeros(numModules, 3);
for lv1 = 1:numModules %store EdgeLengths in matrix
    originalEdgeLengthMatrix(lv1,:) = Bodies{lv1}.EdgeLengths;
end

costs = zeros(numModules, 1);
errors = zeros(numModules, 2);

switch mode
    case {2, 3} %Position and orientation
        endEffLocations = zeros(4,4,numSamples);
    otherwise %Case 1, position only
        endEffLocations = zeros(3,numSamples);
end

for lv = 1:numSamples
    perturbedConfig = config+hingePerturb*(2*rand(1, numModules)-1);
    perturbedEdgeExt = originalEdgeLengthMatrix + edgePerturb*(2*rand(numModules,3)-1);
    actuateAllEdges(rbtree, perturbedEdgeExt);
    [costs(lv), errors(lv,:)] = computeCost(rbtree, perturbedConfig, effName, target, mode);
    
    effTform = getEffTform(rbtree, effName, perturbedConfig);
    
    switch mode
        case {2, 3} %Position and orientation
            endEffLocations(:,:,lv) = effTform;
        otherwise %Case 1, position only
            endEffLocations(:,lv) = tform2trvec(effTform).';
    end

end

% Plot point cloud and/or quiver
%Initialize figure
figure()
ax = show(rbtree, config, 'Collisions','on','Visuals','off');
% Find all patch objects with the default collision mesh color
patches = ax.findobj('FaceColor',[0.2000 0.7000 0.2000]);
set(patches, 'FaceColor', 'b')
alpha(ax,0.2); %Set transparency
hold on

switch mode
    case {2, 3} %Position and orientation
        temp = zeros(numSamples, 6);
        for lv = 1:numSamples
            temp(lv,:) = [endEffLocations(1:3,4,lv); endEffLocations(1:3,3,lv)].';
        end
            
        quiver3(temp(:,1),temp(:,2),temp(:,3),temp(:,4),temp(:,5),temp(:,6), 100, ':b');
        plot3(temp(:,1),temp(:,2),temp(:,3), 'xr', 'MarkerSize', 12, 'LineWidth', 1.5)
        
        endEffPos = squeeze(endEffLocations(1:3,4,:)).';
        posCov = cov(endEffPos);
        % rows of coeff represent the principal component axes, latent are the
        % variances along the principal directions
        [coeff, ~, latent] = pca(endEffPos);
        %TODO statistics on orientation
    otherwise %Case 1, position only
        plot3(endEffLocations(1,:), endEffLocations(2,:), endEffLocations(3,:), 'xr')
        posCov = cov(endEffLocations.');
        % rows of coeff represent the principal component axes, latent are the
        % variances along the principal directions
        [coeff, ~, latent] = pca(endEffLocations.');
        
end

results = struct('costs', costs, 'endEffLocations', endEffLocations, 'errors', errors, 'posCov', posCov, 'coeff', coeff, 'latent', latent);