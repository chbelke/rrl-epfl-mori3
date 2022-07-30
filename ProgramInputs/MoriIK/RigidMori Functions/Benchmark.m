function [trajectory, rbtree] = Benchmark(mode, iterations, samplesPerStep)
%BENCHMARK Allow the speed of the code to be measured and evaluated
%   Mode 1: replicate setup in Chris's code

%Results:
%   mode    maxIter     time    parallel
%   2       50          182     no
%   2       100         355     no
%   2       200         768     no
%   2       200         560     yes
%   2       400         1073    yes
%   1       100         460     yes %broken cost cost function 100 iter
%   1       100         319     yes % fixed cost ~70 iter

%Chris' code 161s 


addpath("RigidMori Functions\")

rbtree = RigidMoriTree('DataFormat', 'row');
basename = rbtree.BaseName;
switch mode
    case 1
        body = RigidMori('b1');
        jnt = robotics.Joint(strcat('b1_jnt'),'revolute');
        jnt.JointAxis = [1 0 0];
        jnt.PositionLimits = [-deg2rad(180-69), deg2rad(180-69)];
        tform = [0 1 0 0; 0 0 1 0; 1 0 0 0; 0 0 0 1]*axang2tform([0 0 1 deg2rad(-30)]);
        setFixedTransform(jnt,tform);
        body.Joint = jnt;
        addVisual(body, "Mesh", 'MoriBound.stl');
        updateCollisionMesh(body);
        addBody(rbtree, body, basename);
        
        spawnMori('b2', 'b1', 2, [0.186 0.186 0.186], rbtree);
        spawnMori('b3', 'b2', 3, [0.186 0.186 0.186], rbtree);
        spawnMori('b4', 'b3', 3, [0.186 0.186 0.186], rbtree);
        spawnMori('b5', 'b4', 2, [0.186 0.186 0.186], rbtree);
        spawnMori('b6', 'b5', 3, [0.186 0.186 0.186], rbtree);
        spawnMori('b7', 'b6', 3, [0.186 0.186 0.186], rbtree);
        spawnMori('b8', 'b7', 2, [0.186 0.186 0.186], rbtree);
        spawnMori('b9', 'b8', 2, [0.186 0.186 0.186], rbtree);
        spawnMori('b10', 'b9', 2, [0.186 0.186 0.186], rbtree);
        
        target = trvec2tform(([4.157355251890671e+02,2.922073295595544e+02,7.756954558357418e+02]-[0 0 550])./1000);
        
        %Perform IK in position mode
%         trajectory = ccdIK(rbtree, target, 'maxIterations', iterations, 'mode', 1);
        trajectory = ccdIK(rbtree, target, 'maxIterations', iterations, 'mode', 1, 'samplesPerStep', 10, 'maxStepSize', [deg2rad(0.25), 1E-3]);
        
    otherwise     
        spawnMori('b1', basename, 3, [0.186 0.186 0.186], rbtree);
        spawnMori('b2', 'b1', 2, [0.186 0.186 0.186], rbtree);
        spawnMori('b3', 'b2', 3, [0.186 0.186 0.186], rbtree);
        spawnMori('b4', 'b3', 2, [0.186 0.186 0.186], rbtree);
        spawnMori('b5', 'b4', 3, [0.186 0.186 0.186], rbtree);
        spawnMori('b6', 'b5', 2, [0.186 0.186 0.186], rbtree);
        spawnMori('b7', 'b6', 3, [0.186 0.186 0.186], rbtree);
        %spawnMori('b8', 'b7', 2, [0.186 0.186 0.186], rbtree);
        
        %target = trvec2tform([-0.3 -0.3 0.1]);
        %target = [0 0 0.2];
        %target = diag([0.5 0.5 0.7])*rand(3,1); %Random target
        %effTform = getEffTform(rbtree, 'b7', zeros(1,7));
        %target = tform2trvec(effTform) + [0.02, 0.02 , 0];
        % FIXME Find a proper target that shows improvement in repeatability with increased samples/step
        target = eye(4);
        %target(1:3,3:4) = [-0.456256304397608,0.507901129245746;0.098343858966893,-0.015895663339700;0.884397348537939,0.129426260621114];
        %target(1:3,3:4) = [-0.425864401294045,0.485562092884888;-0.407931060717753,-0.007238401369835;-0.807608668485024,0.128601704451407];
        target(1:3,3:4) = [-0.302684182301326,0.501101110189476;-0.399033618507743,0.003917607015052;-0.865537091686541,0.109594979681372];
        if exist('samplesPerStep', 'var')
            %trajectory = ccdIK(rbtree, target, 'maxIterations', iterations, 'mode', 2, 'samplesPerStep', samplesPerStep);
            trajectory = ccdIK(rbtree, target, 'maxIterations', iterations, 'mode', 2, 'samplesPerStep', samplesPerStep, 'maxStepSize', [deg2rad(0.25), 1E-3]);%, 'startConfig', [-0.4040    0.5618   -0.4612    0.5629   -0.2988    0.5629   -0.0102]);
        else
            trajectory = ccdIK(rbtree, target, 'maxIterations', iterations, 'mode', 2);
        end
end

end

