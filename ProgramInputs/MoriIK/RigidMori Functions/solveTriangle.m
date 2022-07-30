function [x, y, theta1, theta2] = solveTriangle(edgeLengths)
%SOLVETRIANGLE Solve for the parameters of a module given its edge extensions
%   Input: 
%       edgeLengths: Edge lengths of a module (m)
%   Output:
%       x & y: coordinates of the top vertex
%       theta 1 & 2: interior angle of vertex near the origin and right vertex(rad) respectively
    a = edgeLengths(1);
    b = edgeLengths(2);
    c = edgeLengths(3);
    theta1 = acos((a^2+b^2-c^2)/(2*a*b));
    x = b*cos(theta1);
    y = b*sin(theta1);
    theta2 = asin(y/c);
end
