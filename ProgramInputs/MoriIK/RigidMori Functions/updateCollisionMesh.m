function updateCollisionMesh(body)
%UPDATECOLLISIONMESH updates the collision mesh and CoM of a selected body using
%the body's stored edgeLength and xyt1t2 properties
if body.updateColMesh
    body.clearCollision;
    M = genCollisionMesh(body);
    addCollision(body,"Mesh", M);
    [x, y, ~, ~] = deal(body.xyt1t2{:});
    p = [x+body.EdgeLengths(1), y, 0]/3; %Compute the centroid of the Mori by taking average of the vertices
    body.CenterOfMass = p; %m
    %TODO dynamically update moments of inertia
    body.updateColMesh = false;
end

end

