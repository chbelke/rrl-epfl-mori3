function tf = getEffTform(rbtree, leafName, config)
%GETEFFTFORM Computes the transformation matrix from the base frame to end effector at
%the center of a module
%   Inputs:
%       rbtree: rigidBodyTree object 
%       leafName: name of rigidBody corresponding to module at end of the chain containing the end-effector
%       config: rigidBodyTree configuration describing hinge values
%   Output: homogeneous tranformation matrix from base frame (world origin) to end effector
leafBody = rbtree.getBody(leafName);
[x, y, ~, ~] = deal(leafBody.xyt1t2{:});
p = [x+leafBody.EdgeLengths(1), y, 0]/3; %Compute the centroid of the Mori by taking average of the vertices
tform1 = trvec2tform(p);
tform2 = getTransform (rbtree, config, leafName);
tf = tform2*tform1; %Transform to leafBody frame, then from leafBody frame to the centroid of the leafBody
end