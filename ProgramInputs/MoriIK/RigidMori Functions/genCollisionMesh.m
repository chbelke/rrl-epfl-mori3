function M = genCollisionMesh(body)
%GENCOLLISIONMESH Generates a collision mesh for a Mori module
%   Input: RigidMori object body
%   Output: 12x3 matrix containing vertices of a chamfered-triangular prism
%   collision mesh
    clrL = 28.55/1000; % Chamfer dimension (m)
    d = 0.006; %Half thickness of the module (m)
    T = [1 0 0; 0 1 0; 0 0 -1];
    edgeLengths = body.EdgeLengths;
    [x, y, theta1, theta2] = deal(body.xyt1t2{:});
    trig = [ cos(theta1), sin(theta1); cos(theta2), sin(theta2)];
    offset = trig*clrL;
    M = zeros(12,3);
    M(1:6,:) = [clrL, 0, d;...
                edgeLengths(1)-clrL, 0, d;...
                offset(1,1), offset(1,2), d;...
                x-offset(1,1), y-offset(1,2), d;...
                x+offset(2,1), y-offset(2,2), d;...
                edgeLengths(1)-offset(2,1), offset(2,2), d;];
    M(7:end,:) = M(1:6,:)*T; %Mirror about the z-axis
end