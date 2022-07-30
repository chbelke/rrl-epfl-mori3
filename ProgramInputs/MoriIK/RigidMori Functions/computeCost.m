function [cost, errors] = computeCost(rbtree, config, effName, target, mode)
%COMPUTECOST target is a 4x4 tform matrix, cost penalizes angular misalignment between
%z axes and euclidian distance between coordinate-frames
%effName is the module with the endeffector
% mode 1 is position cost only, mode 2 and 3 penalize angular misalignment
% as well
% errors is the decomposed error. Element 1 is positional error in m,
% element 2 is angular misalignment in degrees.

w = 0.002; %1 degree of error has the same cost as w meters of translational error
EffT = getEffTform(rbtree, effName, config);

switch mode
    case {2, 3}
        % Position and orientation control
        errors = [norm(EffT(:,end)-target(:,end)), abs(acosd(target(:,3).'*EffT(:,3)))];
        cost = (w*errors(2))^2+errors(1)^2;
    otherwise
        % Position control
        errors = [norm(EffT(:,end)-target(:,end)), NaN];
        cost = errors(1)^2;
end