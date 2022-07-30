function spawnMori(name, parent, edge, edgeLengths, rbtree)
%SPAWNMORI Spawns a Mori module at the specified location
%   Inputs:
%       name: string used to name the rigid body and its joint
%       parent: string coresponding to Name of Parent rigid body
%       edge: which edge on the parent to connect the child
%       edgeLengths: Edge lengths of a module (m)
%       rbtree: rigidbodytree object describing robot
    body = RigidMori(name);
    body.EdgeLengths = edgeLengths; %TODO only evalute if edgeLengths specified, varargin for default case
    %TODO ensure parent and child edge lengths match -> update parent edge or overwrite
    %child
    [body.xyt1t2{:}] = solveTriangle(edgeLengths);
    %jnt = robotics.Joint(strcat(name,'_jnt'),'revolute');
    jnt = robotics.Joint(sprintf("%s_jnt", name),'revolute'); %NOTE for codegen
    jnt.JointAxis = [1 0 0];
    jnt.PositionLimits = [-deg2rad(180-69), deg2rad(180-69)];
    if ~strcmp(parent, rbtree.BaseName) %FIXME how do we deal with parent=base?
        parentBody = rbtree.getBody(parent);
        [x, y, theta1, theta2] = deal(parentBody.xyt1t2{:}) ;
        parentBody.ChildLocation(end+1) = edge;
        switch edge
            case 3
                tform = trvec2tform([x, y, 0])*axang2tform([0 0 1 -theta2]); %translation then rotation combined in a single transform
            case 2
                tform = axang2tform([0 0 1 theta1]);
            otherwise
                error('Invalid edge')
        end
    else
        %TODO assert edgeLengths(1)=0.186?
        [x, y, theta1, theta2] = solveTriangle([0.186, 0.186, 0.186]);
        tform = [0 1 0 0; 0 0 1 0; 1 0 0 0; 0 0 0 1]*axang2tform([0 0 1 deg2rad(-30)]);
    end

    setFixedTransform(jnt,tform);
    body.Joint = jnt;
    
    %Add mass and inertia properties for dynamics
    body.Mass = 0.25; %kg
    [x, y, ~, ~] = deal(body.xyt1t2{:});
    p = [x+body.EdgeLengths(1), y, 0]/3; %Compute the centroid of the Mori by taking average of the vertices
    body.CenterOfMass = p; %m
    body.Inertia = [1084.125, 2525.625, 3603.75, 0, 0, 1248.37562]/1e6; %kgm^2, assumes 250g equilateral triangle 186mm edge length, 12mm thick, uniform density
    
    %addVisual(body, "Mesh", 'MoriBound.stl'); %TODO remove?
    updateCollisionMesh(body);
    addBody(rbtree, body, parent);
end